#include "ListFilesHelper.h"

#include <QDir>
#include <QMutexLocker>

ListFilesHelper::ListFilesHelper(QObject *parent) : QObject(parent)
{

}

const QFileInfo &ListFilesHelper::rootInfo() const
{
    return m_rootInfo;
}

int ListFilesHelper::setRootInfo(const QFileInfo &newRootInfo)
{
    if (newRootInfo.isDir() == false) return __LINE__;
    m_rootInfo = newRootInfo;
    return 0;
}

int ListFilesHelper::readFileInfoTree(const QFileInfo &rootInfo)
{
    if (m_cancel) return __LINE__;
    if (rootInfo.isDir() == false) return __LINE__;
    QString absoluteFilePath = rootInfo.absoluteFilePath();
    emit currentlySearching(absoluteFilePath);
    QDir dir = QDir(absoluteFilePath);
    QDir::Filters filter = QDir::Dirs | QDir::Files | QDir::Drives | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;
    QFileInfoList infoList = dir.entryInfoList(filter, QDir::Name);
    for (qsizetype i = 0; i < infoList.count(); i++)
    {
        if (infoList[i].isFile() == true)
        {
            // ignore special cases
            QRegularExpressionMatch match;
            for (qsizetype j = 0; j < m_ignoreRegexList.size(); j++)
            {
                QRegularExpressionMatch match = m_ignoreRegexList.at(j).match(infoList[i].fileName());
                if (match.hasMatch()) break;
            }
            if (match.hasMatch()) continue;

            QString mungedName = QString("%1/%2").arg(infoList[i].size()).arg(infoList[i].fileName());

            QList<QFileInfo> matchList = m_allFilesMultiMap.values(mungedName);
            if (matchList.size() == 1)
            {
                QMutexLocker locker(&m_mutex);
                m_possibleMatchMultiMap.insert(mungedName, matchList[0]);
                locker.unlock();
                emit possibleMatchFound(matchList[0]);
            }
            if (matchList.size())
            {
                QMutexLocker locker(&m_mutex);
                m_possibleMatchMultiMap.insert(mungedName, infoList[i]);
                locker.unlock();
                emit possibleMatchFound(infoList[i]);
            }
            m_allFilesMultiMap.insert(mungedName, infoList[i]);
        }
        else
        {
            // we recurse for folders but not for symbolic links to folders
            if (infoList[i].isSymLink() == false && infoList[i].isDir() == true)
            {
                int err = readFileInfoTree(infoList[i]);
                if (err) return err;
            }
        }
    }
    return 0;
}


QMultiHash<QString, QFileInfo> ListFilesHelper::possibleMatchMultiMap()
{
    QMutexLocker locker(&m_mutex);
    return m_possibleMatchMultiMap;
}

int ListFilesHelper::startScan()
{
    if (m_rootInfo.isDir() == false) return __LINE__;
    m_cancel = 0;
    int err = readFileInfoTree(m_rootInfo);
    return err;
}

void ListFilesHelper::cancelScan()
{
    m_cancel = 1;
}

