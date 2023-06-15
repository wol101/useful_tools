#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QFileInfo>
#include <QErrorMessage>
#include <QTreeWidgetItem>
#include <QElapsedTimer>
#include <QThread>
#include <QStringList>

#include "filesyncpairobject.h"
#include "ErrorReporterDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

signals:

public slots:
    void browse1_clicked();
    void browse2_clicked();
    void addPair_clicked();
    void list_clicked();
    void synchronize_clicked();
    void copy1to2_clicked();
    void copy2to1_clicked();
    void abort_clicked();

    void process_fileSyncPairObject_list();
    void list_clicked_2();
    void synchronize_clicked_2();
    void copy1to2_clicked_2();
    void copy2to1_clicked_2();

    void warningHandler(QString message);
    void failureHandler(QString message);
    void abortHandler(QString message);

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionQuit_triggered();
    void on_actionPreferences_triggered();
    void on_actionAbout_triggered();

private:
    void copyList(QMap<QString, QFileInfo> *fileinfo_map, QFileInfo *source_folder, QFileInfo *destination_folder);
    void fill_tree_widget(QTreeWidgetItem *treeWidget, QMap<QString, QFileInfo> *fileinfo_map);
    void print_QTreeWidgetItem(QTreeWidgetItem* item, int depth);
    QTreeWidgetItem *add_to_folder_list(QTreeWidgetItem *root_item, QMap<QString, QTreeWidgetItem *> *folder_list, const QString *substring, bool isDir);
    bool save();
    bool saveAs();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);

    Ui::MainWindow *ui;
    QErrorMessage *errorMessageDialog;
    ErrorReporterDialog *errorReporterDialog;

    // file lists
    QStringList folder1_paths;
    QStringList folder2_paths;
    QList<FileSyncPairObject *> m_fileSyncPairObject_list;
    int m_current_fileSyncPairObject;

    QList<QMap<QString, QFileInfo> *> fileinfo_map_list;
    QList<QFileInfo *> source_folder_list;
    QList<QFileInfo *> destination_folder_list;

    QElapsedTimer timer;
    QThread *workerThread;

    QString curFile;

    bool preflight;
    bool listRunning;
    bool syncRunning;

};

#endif // MAINWINDOW_H
