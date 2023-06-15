#include <QDir>
#include <QDateTime>

#include <unistd.h>

#include "filesyncpairobject.h"
#include "MainWindow.h"

extern QAtomicInt abortFlag;

FileSyncPairObject::FileSyncPairObject(QObject *parent) :
    QObject(parent)
{
    m_time_threshold_ms = 5000;
    m_keep_biggest = true;
}

void FileSyncPairObject::setFolderPaths(QString folder1_path, QString folder2_path)
{
    m_folder1_path = folder1_path;
    m_folder2_path = folder2_path;
}

void FileSyncPairObject::fillFromFolderPaths()
{
    m_folder1_info.setFile(m_folder1_path);
    m_folder2_info.setFile(m_folder2_path);

    m_folder1_map.clear();
    m_folder2_map.clear();

    if (abortFlag.testAndSetOrdered(1, 1) == false) read_fileinfo_tree(&m_folder1_info, &m_folder1_map, m_folder1_info.absoluteFilePath().size());
    if (abortFlag.testAndSetOrdered(1, 1) == false) read_fileinfo_tree(&m_folder2_info, &m_folder2_map, m_folder2_info.absoluteFilePath().size());

    if (abortFlag.testAndSetOrdered(1, 1) == false) resolve_trees();

    if (abortFlag.testAndSetOrdered(1, 1)) emit abort("List Aborted");
    emit endFillFromFolderPaths();
}

void FileSyncPairObject::read_fileinfo_tree(QFileInfo *root_info, QMap<QString, QFileInfo> *info_map, int prefix_length)
{
    if (abortFlag.testAndSetOrdered(1, 1)) return;
    if (root_info->isDir() == false)
    {
        fail(QString(tr("%1 not found or  not a folder")).arg(root_info->absoluteFilePath()));
        return;
    }
    QString key;
    QDir dir = QDir(root_info->absoluteFilePath());
    QDir::Filters filter = QDir::Dirs | QDir::Files | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;
    QFileInfoList info_list = dir.entryInfoList(filter, QDir::Name);
    for (int i = 0; i < info_list.count(); i++)
    {
        if (abortFlag.testAndSetOrdered(1, 1)) return;
        // ignore special cases
        if (info_list[i].fileName() == ".DS_Store" ||
                info_list[i].fileName() == ".fuzzysync_archive" ||
                info_list[i].fileName().startsWith("._")) continue;

        key = info_list[i].absoluteFilePath().remove(0, prefix_length + 1);
        info_map->insert(key, info_list[i]);

        // we recurse for folders but not for symbolic links to folders
        if (info_list[i].isSymLink() == false && info_list[i].isDir() == true)
        {
            read_fileinfo_tree(&info_list[i], info_map, prefix_length);
        }
    }
}

void FileSyncPairObject::resolve_trees()
{
    char f1_buffer[10000], f2_buffer[10000];
    int st;
    m_folder1_to_copy.clear();
    m_folder1_to_overwrite.clear();
    m_folder2_to_copy.clear();
    m_folder2_to_overwrite.clear();
    for (QMap<QString, QFileInfo>::const_iterator f1 = m_folder1_map.constBegin(); f1 != m_folder1_map.constEnd(); f1++)
    {
        if (abortFlag.testAndSetOrdered(1, 1)) return;
        // check whether the key is in folder2_info
        QMap<QString, QFileInfo>::const_iterator f2 = m_folder2_map.find(f1.key());
        if (f2 == m_folder2_map.constEnd())
        {
            m_folder1_to_copy.insert(f1.key(), f1.value());
        }
        else
        {
            // two files exist with the same subpaths

            if ((f1.value().isBundle() != f2.value().isBundle()) ||
                (f1.value().isDir() != f2.value().isDir()) ||
                (f1.value().isFile() != f2.value().isFile()) ||
                (f1.value().isSymLink() != f2.value().isSymLink()))
            {
                emit fail(QString(tr("%1 and %2 are mismatched - sync aborted")).arg(f1.value().absoluteFilePath()).arg(f2.value().absoluteFilePath()));
                return;
            }

            // don't copy matching folders
            // actually this will miss the case that a folder has been updated with new attribues
            // should check modification dates and copy attributes from newer to older
            if (f1.value().isDir() == true && f1.value().isSymLink() == false) continue;

            // special case because both aliases and symlinks return true for isSymLink()
            // checks if two symlinks are the same
            // actually this will miss the case that a folder has been updated with new attribues
            // should check modification dates and copy attributes from newer to older
            if (f1.value().isSymLink() == true)
            {
                st = readlink(f1.value().absoluteFilePath().toUtf8(), f1_buffer, sizeof(f1_buffer) - 1);
                if (st >= 0) // true symlinks
                {
                    *(f1_buffer + st) = 0;
                    st = readlink(f2.value().absoluteFilePath().toUtf8(), f2_buffer, sizeof(f1_buffer) - 1);
                    if (st >= 0)
                    {
                        *(f2_buffer + st) = 0;
                        if (strcmp(f1_buffer, f2_buffer) == 0)
                            continue;
                    }
                    else
                    {
                        emit fail(QString(tr("%1 and %2 are mismatched (symlink, non-symlink) - sync aborted")).arg(f1.value().absoluteFilePath()).arg(f2.value().absoluteFilePath()));
                        return;
                    }
                }
                else // macosx aliases
                {
                    st = readlink(f2.value().absoluteFilePath().toUtf8(), f2_buffer, sizeof(f1_buffer) - 1);
                    if (st >= 0) // but the second one is a true symlink
                    {
                        emit fail(QString(tr("%1 and %2 are mismatched (non-symlink, symlink) - sync aborted")).arg(f1.value().absoluteFilePath()).arg(f2.value().absoluteFilePath()));
                        return;
                    }
                }
            }

            // find youngest
            qint64 diff = f1.value().lastModified().toMSecsSinceEpoch() - f2.value().lastModified().toMSecsSinceEpoch();
            if (diff > m_time_threshold_ms)
            {
                m_folder1_to_overwrite.insert(f1.key(), f1.value());
            }
            else
            {
                if (diff < -m_time_threshold_ms)
                {
                    m_folder2_to_overwrite.insert(f2.key(), f2.value());
                }
                else
                {
                    if (f1.value().isFile())
                    {
                        // same age so sanity check using size
                        diff = f1.value().size() - f2.value().size();
                        if (diff != 0 && m_keep_biggest)
                        {
                            if (diff > 0) m_folder1_to_overwrite.insert(f1.key(), f1.value());
                            if (diff < 0) m_folder2_to_overwrite.insert(f2.key(), f2.value());
                        }
                        if (diff != 0 && (m_keep_biggest == false))
                        {
                            emit warn(QString(tr("%1 and %2 are not the same size - copy not performed")).arg(f1.value().absoluteFilePath()).arg(f2.value().absoluteFilePath()));
                        }
                    }
                }
            }
        }
    }

    for (QMap<QString, QFileInfo>::const_iterator f2 = m_folder2_map.constBegin(); f2 != m_folder2_map.constEnd(); f2++)
    {
        if (abortFlag.testAndSetOrdered(1, 1)) return;
        // check whether the key is in folder1_info
        QMap<QString, QFileInfo>::const_iterator f1 = m_folder1_map.find(f2.key());
        if (f1 == m_folder1_map.constEnd())
        {
            m_folder2_to_copy.insert(f2.key(), f2.value());
        }
    }

}
