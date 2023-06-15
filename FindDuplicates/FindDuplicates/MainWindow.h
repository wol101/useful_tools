#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ListFilesHelper.h"

#include <QMainWindow>
#include <QMultiMap>
#include <QString>
#include <QRegularExpression>
#include <QFutureWatcher>
#include <QFileInfo>
#include <QAtomicInt>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void open();
    void start();
    void cancel();
    void insertNewMatch(QFileInfo fileInfo);
    void displaySearchFolder(QString dirPath);
    void handleFinished();

signals:

private:
    Ui::MainWindow *ui;

    QFileInfo m_rootFolder;
    QList<QRegularExpression> m_ignoreRegexList;
    QFutureWatcher<int> m_watcher;

    ListFilesHelper m_listFileHelper;

};
#endif // MAINWINDOW_H
