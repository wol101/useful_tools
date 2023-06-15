#ifndef LISTFILESHELPER_H
#define LISTFILESHELPER_H

#include <QObject>
#include <QFileInfo>
#include <QList>
#include <QRegularExpression>
#include <QMutex>
#include <QMultiHash>

class ListFilesHelper : public QObject
{
    Q_OBJECT
public:
    explicit ListFilesHelper(QObject *parent = nullptr);

    int startScan();
    void cancelScan();

    const QFileInfo &rootInfo() const;
    int setRootInfo(const QFileInfo &newRootInfo);

    QMultiHash<QString, QFileInfo> possibleMatchMultiMap();

signals:
    void possibleMatchFound(QFileInfo fileInfo);
    void currentlySearching(QString dirPath);

private:
    int readFileInfoTree(const QFileInfo &rootInfo);

    QFileInfo m_rootInfo;
    QMultiHash<QString, QFileInfo> m_allFilesMultiMap;
    QList<QRegularExpression> m_ignoreRegexList;

    QMultiHash<QString, QFileInfo> m_possibleMatchMultiMap;

    QMutex m_mutex;
    QAtomicInt m_cancel = 0;
};

#endif // LISTFILESHELPER_H
