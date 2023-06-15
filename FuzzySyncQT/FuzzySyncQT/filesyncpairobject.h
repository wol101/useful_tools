#ifndef FILESYNCPAIROBJECT_H
#define FILESYNCPAIROBJECT_H

#include <QObject>
#include <QFileInfo>
#include <QMap>
#include <QString>

class FileSyncPairObject : public QObject
{
    Q_OBJECT
public:
    explicit FileSyncPairObject(QObject *parent = 0);
    
    void setFolderPaths(QString folder1_path, QString folder2_path);

    QString  *get_folder1_path() { return &m_folder1_path; }
    QString  *get_folder2_path() { return &m_folder2_path; }

    QFileInfo  *get_folder1_info() { return &m_folder1_info; }
    QFileInfo  *get_folder2_info() { return &m_folder2_info; }

    QMap<QString, QFileInfo>  *get_folder1_map() { return &m_folder1_map; }
    QMap<QString, QFileInfo>  *get_folder2_map() { return &m_folder2_map; }

    QMap<QString, QFileInfo>  *get_folder1_to_copy() { return &m_folder1_to_copy; }
    QMap<QString, QFileInfo>  *get_folder1_to_overwrite() { return &m_folder1_to_overwrite; }
    QMap<QString, QFileInfo>  *get_folder2_to_copy() { return &m_folder2_to_copy; }
    QMap<QString, QFileInfo>  *get_folder2_to_overwrite() { return &m_folder2_to_overwrite; }

signals:
    void endFillFromFolderPaths();
    void warn(QString errorMessage);
    void fail(QString errorMessage);
    void abort(QString errorMessage);

public slots:
    void fillFromFolderPaths();

private:
    void read_fileinfo_tree(QFileInfo *root_info, QMap<QString, QFileInfo> *info_map, int prefix_length);
    void resolve_trees();

    QString m_folder1_path;
    QString m_folder2_path;

    QFileInfo m_folder1_info;
    QFileInfo m_folder2_info;

    QMap<QString, QFileInfo> m_folder1_map;
    QMap<QString, QFileInfo> m_folder2_map;

    QMap<QString, QFileInfo> m_folder1_to_copy;
    QMap<QString, QFileInfo> m_folder1_to_overwrite;
    QMap<QString, QFileInfo> m_folder2_to_copy;
    QMap<QString, QFileInfo> m_folder2_to_overwrite;

    qint64 m_time_threshold_ms;
    bool m_keep_biggest;
};

#endif // FILESYNCPAIROBJECT_H
