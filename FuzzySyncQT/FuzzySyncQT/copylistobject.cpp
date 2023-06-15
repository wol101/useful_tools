#include <QtDebug>
#include <QDateTime>
#include <QProcess>
#include <QDir>

#include <unistd.h>
#include <sys/xattr.h>
#include <sys/attr.h>
#include <sys/vnode.h>

#include "copylistobject.h"
#include "MainWindow.h"

extern QAtomicInt abortFlag;

CopyListObject::CopyListObject(QObject *parent) :
    QObject(parent)
{
    m_preflight = true;
}

CopyListObject::~CopyListObject()
{
}

void CopyListObject::doWork()
{
    for (int i = 0; i < m_fileinfo_map_list->size(); i++)
    {
        if (abortFlag.testAndSetOrdered(1, 1)) break;

        copyList(m_fileinfo_map_list->at(i), m_source_folder_list->at(i), m_destination_folder_list->at(i));
    }
    if (abortFlag.testAndSetOrdered(1, 1)) emit abort("Copy Aborted");
    emit endWork();
}

// source_folder is set to 0 if we only want to move the destination file to archive
void CopyListObject::copyList(QMap<QString, QFileInfo> *fileinfo_map, QFileInfo *source_folder, QFileInfo *destination_folder)
{
    QString source_path, destination_path, archive_path;
    QString command;
    QDateTime dateTime =  QDateTime::currentDateTime();
    QString time_stamp = dateTime.toString("yyyy-MM-dd_hh-mm-ss.zzz");
    QProcess *process;
    QStringList arguments;
    QDir dir;
    int pathLen;
    char pathBuf[10000];

    for (QMap<QString, QFileInfo>::const_iterator iter = fileinfo_map->constBegin(); iter != fileinfo_map->constEnd(); iter++)
    {
        if (abortFlag.testAndSetOrdered(1, 1)) return;

        if (source_folder)
            source_path = QString("%1/%2").arg(source_folder->absoluteFilePath()).arg(iter.key());
        else
            source_path = "";
        destination_path = QString("%1/%2").arg(destination_folder->absoluteFilePath()).arg(iter.key());
        archive_path = QString("%1/.fuzzysync_archive/%2/%3").arg(destination_folder->absoluteFilePath()).arg(time_stamp).arg(iter.key());

        QFileInfo destination(destination_path);
        QFileInfo source(source_path); // do I need to check again here?

        if (m_preflight)
        {
            qDebug() << QString("Move %1 to %2").arg(destination_path).arg(archive_path);
            qDebug() << QString("Copy %1 to %2").arg(source_path).arg(destination_path);
        }
        else
        {

            // handle the archive process
            if (destination.exists())
            {
                QFileInfo archive(archive_path);
                if (archive.exists())
                {
                    emit warn(QString(tr("%1 already exists - move not performed")).arg(archive_path));
                    continue;
                }
                if (dir.mkpath(archive.absolutePath()) == false)
                {
                    emit warn(QString(tr("Unable to create or verify path %1 - move not performed")).arg(archive.absolutePath()));
                    continue;
                }
                else
                {
                    command = "mv";
                    arguments.clear();
                    arguments.append(destination_path);
                    arguments.append(archive.absolutePath());
                    process = new QProcess();
                    process->start(command, arguments);
                    if (process->waitForFinished(30000) == false)
                    {
                        process->kill();
                        emit warn(QString(tr("Process hung: unable to move %1 to archive")).arg(destination_path));
                        delete process;
                        continue;
                    }
                    if (process->exitCode())
                    {
                        emit warn(QString(tr("Failed exit code: unable to move %1 to archive")).arg(destination_path));
                        delete process;
                        continue;
                    }
                    delete process;
                }
            }

            if (source_folder == 0) continue; // this is the move only case

            if (source.exists() == false && source.isSymLink() == false)
            {
                emit warn(QString(tr("Source file does not exist %1")).arg(source.absoluteFilePath()));
                continue;
            }

            // create the destination parent path if required
            if (dir.mkpath(destination.absolutePath()) == false)
            {
                emit warn(QString(tr("Unable to create or verify path %1 - folder not created")).arg(destination.absolutePath()));
                continue;
            }

            // now the special cases of symlinks
            if (source.isSymLink())
            {
                pathLen = readlink(source_path.toUtf8(), pathBuf, sizeof(pathBuf) - 1);
                if (pathLen >= 0) // not an alias
                {
                    if (pathLen == 0 || pathLen >= int(sizeof(pathBuf) - 1))
                    {
                        emit warn(QString(tr("%1 link read failed")).arg(source_path));
                        continue;
                    }
                    *(pathBuf + pathLen) = 0; // readlink does not append a 0 but symlink needs it
                    pathLen = symlink(pathBuf, destination_path.toUtf8());
                    if (pathLen == -1)
                    {
                        emit warn(QString(tr("%1 link create failed")).arg(destination_path));
                        continue;
                    }
                    copyAttributes(source_path, destination_path);
                    copyExtendedAttributes(source_path, destination_path);
                    continue;
                }
            }

            // now the special cases of regular folders
            if (source.isDir() && source.isSymLink() == false)
            {
                if (dir.mkpath(destination.absoluteFilePath()) == false)
                {
                    emit warn(QString(tr("Unable to create path %1 - folder not created")).arg(destination.absoluteFilePath()));
                    continue;
                }

                // now copy the attribues and extended attributes
                copyAttributes(source_path, destination_path);
                copyExtendedAttributes(source_path, destination_path);

                continue;
            }

            // now we handle regular files
            command = "ditto";
            arguments.clear();
            arguments.append(source_path);
            arguments.append(destination.absolutePath());
            process = new QProcess();
            process->start(command, arguments);
            if (process->waitForFinished(3600000) == false) // using 1 hour as the timeout
            {
                process->kill();
                emit warn(QString(tr("Process hung: unable to copy file %1")).arg(source_path));
                delete process;
                continue;
            }
            if (process->exitCode())
            {
                emit warn(QString(tr("Failed exit code: unable to copy file %1")).arg(source_path));
                delete process;
                continue;
            }
            delete process;
        }

    }
}

void CopyListObject::copyAttributes(QString &src, QString &dst)
{
    int st;
    struct attrlist attrList;

    attrList.bitmapcount = ATTR_BIT_MAP_COUNT; /* number of attr. bit sets in list */
    attrList.reserved = 0;                     /* (to maintain 4-byte alignment) */


    attrList.volattr = 0;     /* volume attribute group */
    attrList.dirattr = 0;     /* directory attribute group */
    attrList.fileattr = 0;    /* file attribute group */
    attrList.forkattr = 0;    /* fork attribute group */

    unsigned long options = FSOPT_NOFOLLOW | FSOPT_REPORT_FULLSIZE;

    /* common attribute group */
    attrList.commonattr = ATTR_CMN_OBJTYPE | ATTR_CMN_CRTIME | ATTR_CMN_MODTIME | ATTR_CMN_FNDRINFO | ATTR_CMN_OWNERID | ATTR_CMN_GRPID | ATTR_CMN_ACCESSMASK | ATTR_CMN_FLAGS;
    // this struct is customised to accommodate what has been asked for in attrList.commonattr
    struct requestedAttributes
    {
        u_int32_t totalLength;
        int32_t objtype;
        struct timespec crtime;
        struct timespec modtime;
        uint8_t fndrinfo[32];
        uid_t ownerid;
        gid_t grpid;
        u_int32_t accessmask;
        u_int32_t flags;
    } attrBuf;
    size_t attrBufSize = sizeof(attrBuf);

    st = getattrlist(src.toUtf8(), &attrList, &attrBuf, attrBufSize, options);
    if (st != 0)
    {
        emit warn(QString(tr("getattrlist error for %1")).arg(src));
        return;
    }

    // only interested in symlinks and directories
    if (attrBuf.objtype != (int32_t)VDIR && attrBuf.objtype != (int32_t)VLNK)
    {
        emit warn(QString(tr("Wrong file type: unable to copy atrributes for %1")).arg(src));
        return;
    }

    if (geteuid() == 0) // we are root so we are allowed to alter the ATTR_CMN_OWNERID and ATTR_CMN_GRPID
    {
        attrList.commonattr = ATTR_CMN_CRTIME | ATTR_CMN_MODTIME | ATTR_CMN_FNDRINFO | ATTR_CMN_OWNERID | ATTR_CMN_GRPID | ATTR_CMN_ACCESSMASK | ATTR_CMN_FLAGS;
        options = FSOPT_NOFOLLOW;
        struct setAttributesRoot
        {
            struct timespec crtime;
            struct timespec modtime;
            uint8_t fndrinfo[32];
            uid_t ownerid;
            gid_t grpid;
            u_int32_t accessmask;
            u_int32_t flags;
        } attrBufRoot;
        attrBufSize = sizeof(setAttributesRoot);
        attrBufRoot.crtime = attrBuf.crtime;
        attrBufRoot.modtime = attrBuf.modtime;
        for (int i = 0; i < 32; i++) attrBufRoot.fndrinfo[i] = attrBuf.fndrinfo[i];
        attrBufRoot.ownerid = attrBuf.ownerid;
        attrBufRoot.grpid = attrBuf.grpid;
        attrBufRoot.accessmask = attrBuf.accessmask;
        attrBufRoot.flags = attrBuf.flags;

        st = setattrlist(dst.toUtf8(), &attrList, &attrBufRoot, attrBufSize, options);
    }
    else // as a regular user we are not allowed to alter the ATTR_CMN_OWNERID and ATTR_CMN_GRPID can be troublesome (we can only change it to certain groups and finding out which is a pain)
    {
        attrList.commonattr = ATTR_CMN_CRTIME | ATTR_CMN_MODTIME | ATTR_CMN_FNDRINFO | ATTR_CMN_ACCESSMASK | ATTR_CMN_FLAGS;
        options = FSOPT_NOFOLLOW;
        struct setAttributesUser
        {
            struct timespec crtime;
            struct timespec modtime;
            uint8_t fndrinfo[32];
            u_int32_t accessmask;
            u_int32_t flags;
        } attrBufUser;
        attrBufSize = sizeof(attrBufUser);
        attrBufUser.crtime = attrBuf.crtime;
        attrBufUser.modtime = attrBuf.modtime;
        for (int i = 0; i < 32; i++) attrBufUser.fndrinfo[i] = attrBuf.fndrinfo[i];
        attrBufUser.accessmask = attrBuf.accessmask;
        attrBufUser.flags = attrBuf.flags;

        st = setattrlist(dst.toUtf8(), &attrList, &attrBufUser, attrBufSize, options);
    }

    if (st != 0)
    {
        emit warn(QString(tr("setattrlist error for %1")).arg(dst));
        return;
    }
}


void CopyListObject::copyExtendedAttributes(QString &src, QString &dst)
{
    int nameListSize;
    int dataSize;
    int offset;
    char *nameListBuffer;
    char *dataBuffer;
    int st;

    nameListSize = listxattr(src.toUtf8(), 0, 0, XATTR_NOFOLLOW); // XATTR_SHOWCOMPRESSION isn't necessary since this is just for folders and symlinks
    if (nameListSize < 0)
    {
        emit warn(QString(tr("Unable to read extended attribute list from %1")).arg(src));
        return;
    }
    if (nameListSize == 0) return; // no extended attributes to copy

    nameListBuffer = new char[nameListSize];
    nameListSize = listxattr(src.toUtf8(), nameListBuffer, nameListSize, XATTR_NOFOLLOW);
    if (nameListSize < 0)
    {
        emit warn(QString(tr("Unable to read extended attribute list into buffer from %1")).arg(src));
        free(nameListBuffer);
        return;
    }

    offset = 0;
    while (offset < nameListSize)
    {
        dataSize = getxattr(src.toUtf8(), nameListBuffer + offset, 0, 0, 0, XATTR_NOFOLLOW);
        if (dataSize <= 0)
        {
            emit warn(QString(tr("Unable to read extended attribute %1 from %2")).arg(nameListBuffer + offset).arg(src));
        }
        else
        {
            dataBuffer = new char[dataSize];
            dataSize = getxattr(src.toUtf8(), nameListBuffer + offset, dataBuffer, dataSize, 0, XATTR_NOFOLLOW);
            if (dataSize <= 0)
            {
                emit warn(QString(tr("Unable to read extended attribute %1 into buffer from %2")).arg(nameListBuffer + offset).arg(src));
            }
            else
            {
                st = setxattr(dst.toUtf8(), nameListBuffer + offset, dataBuffer, dataSize, 0, XATTR_NOFOLLOW);
                if (st != 0)
                    emit warn(QString(tr("Unable to write extended attribute %1 into buffer from %2")).arg(nameListBuffer + offset).arg(dst));
            }
            delete [] dataBuffer;
        }
        while (offset < nameListSize && *(nameListBuffer + offset)) offset++;
        offset++; // clears the 0
    }

    delete [] nameListBuffer;

}
