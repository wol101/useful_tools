#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QDir>
#include <QList>
#include <QFileDialog>
#include <QAction>
#include <QPushButton>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // user interface signals
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->pushButtonStart, &QPushButton::clicked, this, &MainWindow::start);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &MainWindow::cancel);

    // progress signals from the search
//    connect(&m_listFileHelper, &ListFilesHelper::possibleMatchFound, this, &MainWindow::insertNewMatch, Qt::QueuedConnection);
    connect(&m_listFileHelper, &ListFilesHelper::currentlySearching, this, &MainWindow::displaySearchFolder, Qt::QueuedConnection);

    // and this watches the running process
    connect(&m_watcher, &QFutureWatcher<int>::finished, this, &MainWindow::handleFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{

    QString folder = QFileDialog::getExistingDirectory(this, tr("Choose root folder to search for duplicates"));
    if (folder.isNull() == false)
    {
        m_rootFolder = QFileInfo(folder);
        statusBar()->showMessage(QString("Searching \"%1\"").arg(m_rootFolder.absoluteFilePath()));
    }
    else
    {
        statusBar()->showMessage("User cancelled");
    }
}

void MainWindow::start()
{
    Q_ASSERT(!m_watcher.isRunning());
    Q_ASSERT(m_rootFolder.isDir());
    statusBar()->showMessage("Search process running");
    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonStart->setEnabled(false);
    m_listFileHelper.setRootInfo(m_rootFolder);
#if QT_VERSION >= 0x060000
    QFuture<int> future = QtConcurrent::run(&ListFilesHelper::startScan, &m_listFileHelper);
#else
    QFuture<int> future = QtConcurrent::run(&m_listFileHelper, &ListFilesHelper::startScan);
#endif
    m_watcher.setFuture(future);
}

void MainWindow::cancel()
{
    m_listFileHelper.cancelScan();
}

void MainWindow::insertNewMatch(QFileInfo fileInfo)
{
    statusBar()->showMessage(QString("Found ") + fileInfo.absoluteFilePath());
}

void MainWindow::handleFinished()
{
    statusBar()->showMessage("Search process finished");
    QMultiHash<QString, QFileInfo> possibleMatchMultiMap = m_listFileHelper.possibleMatchMultiMap();
    QList<QString> keys = possibleMatchMultiMap.uniqueKeys();
    QList<QBrush> brushList = { QColor::fromRgb(255, 0, 0), QColor::fromRgb(0, 255, 0) };
    for (qsizetype i = 0; i < keys.size(); i++)
    {
        QList<QFileInfo> values = possibleMatchMultiMap.values(keys[i]);
        for (qsizetype j = 0; j < values.size(); j++)
        {
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->setRowCount(row + 1);
            int col = 0;
            QTableWidgetItem *item = new QTableWidgetItem(values[j].absoluteFilePath());
            item->setBackground(brushList[i % brushList.size()]);
            ui->tableWidget->setItem(row, col, item);

        }
    }

    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonStart->setEnabled(true);
}

void MainWindow::displaySearchFolder(QString dirPath)
{
    statusBar()->showMessage(QString("Searching ") + dirPath);
}


