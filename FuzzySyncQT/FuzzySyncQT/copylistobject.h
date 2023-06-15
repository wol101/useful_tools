#ifndef COPYLISTOBJECT_H
#define COPYLISTOBJECT_H

#include <QObject>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QList>

class CopyListObject : public QObject
{
    Q_OBJECT
public:
    explicit CopyListObject(QObject *parent = 0);
    ~CopyListObject();

    void set_fileinfo_map_list(QList<QMap<QString, QFileInfo> *> *fileinfo_map_list) { m_fileinfo_map_list = fileinfo_map_list; }
    void set_source_folder_list(QList<QFileInfo *> *source_folder_list) { m_source_folder_list = source_folder_list; }
    void set_destination_folder_list(QList<QFileInfo *> *destination_folder_list) { m_destination_folder_list = destination_folder_list; }

    void setPreflight(bool preflight) { m_preflight = preflight; }

signals:
    void endWork();
    void warn(QString errorMessage);
    void fail(QString errorMessage);
    void abort(QString errorMessage);

public slots:
    void doWork();

private:
    void copyList(QMap<QString, QFileInfo> *fileinfo_map, QFileInfo *source_folder, QFileInfo *destination_folder);
    void copyExtendedAttributes(QString &src, QString &dst);
    void copyAttributes(QString &src, QString &dst);

    QList<QMap<QString, QFileInfo> *> *m_fileinfo_map_list;
    QList<QFileInfo *> *m_source_folder_list;
    QList<QFileInfo *> *m_destination_folder_list;

    bool m_preflight;
};

#endif // COPYLISTOBJECT_H
