#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QFileSystemModel;

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
    void onTreeViewClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    QFileSystemModel *m_dirModel;
    QFileSystemModel *m_fileModel;
};
#endif // MAINWINDOW_H
