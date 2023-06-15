#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Creates our new model and populate
    QString mPath = "C:/";
    m_dirModel = new QFileSystemModel(this);
    // Set filter
    m_dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    // QFileSystemModel requires root path
    m_dirModel->setRootPath(mPath);
    // Attach the model to the view
    ui->treeView->setModel(m_dirModel);

    // FILES
    m_fileModel = new QFileSystemModel(this);
    // Set filter
    m_fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    // QFileSystemModel requires root path
    m_fileModel->setRootPath(mPath);
    // Attach the model to the view
    ui->listView->setModel(m_fileModel);

    connect(ui->treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTreeViewClicked(const QModelIndex &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTreeViewClicked(const QModelIndex &index)
{
    // TreeView clicked
    // 1. We need to extract path
    // 2. Set that path into our ListView

    // Get the full path of the item that's user clicked on
    QString mPath = m_dirModel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(m_fileModel->setRootPath(mPath));
}

