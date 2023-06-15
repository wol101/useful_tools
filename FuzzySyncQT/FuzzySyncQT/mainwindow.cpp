#include <QtDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>
#include <QSettings>
#include <QErrorMessage>
#include <QDateTime>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHash>
#include <QProcess>
#include <QDateTime>
#include <QDir>
#include <QThread>
#include <QMessageBox>
#include <QAtomicInt>
#include <QPlainTextEdit>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "copylistobject.h"
#include "filesyncpairobject.h"
#include "ErrorReporterDialog.h"

QAtomicInt abortFlag;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("AnimalSimulation", "FuzzySyncQT");

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    ui->lineEdit_Folder1->setText(settings.value("LastFolder1", QString("")).toString());
    ui->lineEdit_Folder2->setText(settings.value("LastFolder2", QString("")).toString());

    // this sets the horizontal scroll bar to do something sensible when there is only 1 column
    ui->treeWidget_FilesToCopy1->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->treeWidget_FilesToCopy1->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget_FilesToCopy1->header()->setStretchLastSection(false);
    ui->treeWidget_FilesToCopy2->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->treeWidget_FilesToCopy2->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget_FilesToCopy2->header()->setStretchLastSection(false);
    ui->treeWidget_FilesToOverwrite1->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->treeWidget_FilesToOverwrite1->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget_FilesToOverwrite1->header()->setStretchLastSection(false);
    ui->treeWidget_FilesToOverwrite2->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->treeWidget_FilesToOverwrite2->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->treeWidget_FilesToOverwrite2->header()->setStretchLastSection(false);

    errorMessageDialog = new QErrorMessage(this);

    errorReporterDialog = new ErrorReporterDialog(this);
    errorReporterDialog->hide();

    timer.start();
    workerThread = 0;
    m_current_fileSyncPairObject = 0;

    curFile = settings.value("LastOpenedFile", QString("")).toString();
    if (curFile.size() > 0)
    {
        loadFile(curFile);
        setWindowTitle(curFile); // shouldn't be necessary but setWindowFilePath seems flaky
    }
    preflight = false;
    listRunning = false;
    syncRunning = false;

    abortFlag = 0;
}

MainWindow::~MainWindow()
{
    for (int i = 0; i < m_fileSyncPairObject_list.size(); i++) m_fileSyncPairObject_list[i]->deleteLater();

    delete errorMessageDialog;
    delete errorReporterDialog;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
    if (listRunning || syncRunning)
    {
        QMessageBox::warning(this, tr("Application Busy"), tr("Synchronisation still running. Please allow to finish, or abort before quitting."));
        return;
    }

    if (maybeSave() == false) return;

    QSettings settings("AnimalSimulation", "FuzzySyncQT");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.sync();

    QMainWindow::closeEvent(event);
 }

void MainWindow::browse1_clicked()
{
    QSettings settings("AnimalSimulation", "FuzzySyncQT");
    QFileInfo info = settings.value("LastFolder1", QString("")).toString();
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select folder 1 for sync"), info.absoluteFilePath(), options);
    if (!directory.isEmpty())
    {
        ui->lineEdit_Folder1->setText(directory);
        settings.setValue("LastFolder1", directory);
        settings.sync();
    }
}

void MainWindow::browse2_clicked()
{
    QSettings settings("AnimalSimulation", "FuzzySyncQT");
    QFileInfo info = settings.value("LastFolder2", QString("")).toString();
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select folder 2 for sync"), info.absoluteFilePath(), options);
    if (!directory.isEmpty())
    {
        ui->lineEdit_Folder2->setText(directory);
        settings.setValue("LastFolder2", directory);
        settings.sync();
    }
}

void MainWindow::addPair_clicked()
{
    QFileInfo folder1_info = QFileInfo(ui->lineEdit_Folder1->text());
    if (folder1_info.isDir() == false)
    {
        errorMessageDialog->showMessage(QString(tr("Folder 1 error: %1 is not a valid folder")).arg(folder1_info.absoluteFilePath()));
        return;
    }
    QFileInfo folder2_info = QFileInfo(ui->lineEdit_Folder2->text());
    if (folder2_info.isDir() == false)
    {
        errorMessageDialog->showMessage(QString(tr("Folder 2 error: %1 is not a valid folder")).arg(folder2_info.absoluteFilePath()));
        return;
    }

    folder1_paths.append(folder1_info.absoluteFilePath());
    folder2_paths.append(folder2_info.absoluteFilePath());

    setWindowModified(true);

    statusBar()->showMessage(QString(tr("Sync Pair %1 added")).arg(m_fileSyncPairObject_list.size() + 1));
}

void MainWindow::list_clicked()
{
     abortFlag = 0;
    if (folder1_paths.size() == 0) addPair_clicked();

    for (int i = 0; i < m_fileSyncPairObject_list.size(); i++) m_fileSyncPairObject_list[i]->deleteLater();
    m_fileSyncPairObject_list.clear();

    for (int i = 0; i < folder1_paths.size(); i++)
    {
        FileSyncPairObject *fileSyncPairObject = new FileSyncPairObject();
        fileSyncPairObject->setFolderPaths(folder1_paths[i], folder2_paths[i]);
        m_fileSyncPairObject_list.append(fileSyncPairObject);
    }

    ui->treeWidget_FilesToCopy1->clear();
    ui->treeWidget_FilesToOverwrite1->clear();
    ui->treeWidget_FilesToCopy2->clear();
    ui->treeWidget_FilesToOverwrite2->clear();

    m_current_fileSyncPairObject = -1;
    process_fileSyncPairObject_list();
}

void MainWindow::process_fileSyncPairObject_list()
{
    m_current_fileSyncPairObject++;
    statusBar()->showMessage(QString(tr("Processing Pair %1")).arg(m_current_fileSyncPairObject + 1));
    ui->lineEdit_Folder1->setText(*m_fileSyncPairObject_list[m_current_fileSyncPairObject]->get_folder1_path());
    ui->lineEdit_Folder2->setText(*m_fileSyncPairObject_list[m_current_fileSyncPairObject]->get_folder2_path());

    workerThread = new QThread(this);
    connect(workerThread, &QThread::started, m_fileSyncPairObject_list[m_current_fileSyncPairObject], &FileSyncPairObject::fillFromFolderPaths);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(m_fileSyncPairObject_list[m_current_fileSyncPairObject], &FileSyncPairObject::endFillFromFolderPaths, workerThread, &QThread::quit);
    connect(m_fileSyncPairObject_list[m_current_fileSyncPairObject], &FileSyncPairObject::warn, this, &MainWindow::warningHandler);
    connect(m_fileSyncPairObject_list[m_current_fileSyncPairObject], &FileSyncPairObject::fail, this, &MainWindow::failureHandler);
    connect(m_fileSyncPairObject_list[m_current_fileSyncPairObject], &FileSyncPairObject::abort, this, &MainWindow::abortHandler);


    if (m_current_fileSyncPairObject < m_fileSyncPairObject_list.size() - 1)
        connect(m_fileSyncPairObject_list[m_current_fileSyncPairObject], &FileSyncPairObject::endFillFromFolderPaths, this, &MainWindow::process_fileSyncPairObject_list);
    else
        connect(m_fileSyncPairObject_list[m_current_fileSyncPairObject], &FileSyncPairObject::endFillFromFolderPaths, this, &MainWindow::list_clicked_2);

    m_fileSyncPairObject_list[m_current_fileSyncPairObject]->moveToThread(workerThread);
    workerThread->start(); // Starts an event loop, and emits workerThread->started()
    listRunning = true;
}

void MainWindow::list_clicked_2()
{
    listRunning = false;

    // and now fill the tree widgets
    statusBar()->showMessage(tr("Filling widgets"));

    for (int i = 0; i < m_fileSyncPairObject_list.size(); i++)
    {
        QTreeWidgetItem *folder1_to_copy_root_item = new QTreeWidgetItem();
        folder1_to_copy_root_item->setText(0, m_fileSyncPairObject_list[i]->get_folder1_info()->absoluteFilePath());
        fill_tree_widget(folder1_to_copy_root_item, m_fileSyncPairObject_list[i]->get_folder1_to_copy());
        ui->treeWidget_FilesToCopy1->addTopLevelItem(folder1_to_copy_root_item);

        QTreeWidgetItem *folder1_to_overwrite_root_item = new QTreeWidgetItem();
        folder1_to_overwrite_root_item->setText(0, m_fileSyncPairObject_list[i]->get_folder1_info()->absoluteFilePath());
        fill_tree_widget(folder1_to_overwrite_root_item, m_fileSyncPairObject_list[i]->get_folder1_to_overwrite());
        ui->treeWidget_FilesToOverwrite1->addTopLevelItem(folder1_to_overwrite_root_item);

        QTreeWidgetItem *folder2_to_copy_root_item = new QTreeWidgetItem();
        folder2_to_copy_root_item->setText(0, m_fileSyncPairObject_list[i]->get_folder2_info()->absoluteFilePath());
        fill_tree_widget(folder2_to_copy_root_item, m_fileSyncPairObject_list[i]->get_folder2_to_copy());
        ui->treeWidget_FilesToCopy2->addTopLevelItem(folder2_to_copy_root_item);

        QTreeWidgetItem *folder2_to_overwrite_root_item = new QTreeWidgetItem();
        folder2_to_overwrite_root_item->setText(0, m_fileSyncPairObject_list[i]->get_folder2_info()->absoluteFilePath());
        fill_tree_widget(folder2_to_overwrite_root_item, m_fileSyncPairObject_list[i]->get_folder2_to_overwrite());
        ui->treeWidget_FilesToOverwrite2->addTopLevelItem(folder2_to_overwrite_root_item);
    }

    statusBar()->showMessage(tr("List completed at ") + QDateTime::currentDateTime().toString("hh:mm:ss dd:MM:yyyy"));
}

void MainWindow::synchronize_clicked()
{
    abortFlag = 0;
    statusBar()->showMessage(tr("Synchronize started"));

    fileinfo_map_list.clear();
    source_folder_list.clear();
    destination_folder_list.clear();

    for (int i = 0; i < m_fileSyncPairObject_list.size(); i++)
    {
        copyList(m_fileSyncPairObject_list[i]->get_folder1_to_copy(), m_fileSyncPairObject_list[i]->get_folder1_info(), m_fileSyncPairObject_list[i]->get_folder2_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder2_to_copy(), m_fileSyncPairObject_list[i]->get_folder2_info(), m_fileSyncPairObject_list[i]->get_folder1_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder1_to_overwrite(), m_fileSyncPairObject_list[i]->get_folder1_info(), m_fileSyncPairObject_list[i]->get_folder2_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder2_to_overwrite(), m_fileSyncPairObject_list[i]->get_folder2_info(), m_fileSyncPairObject_list[i]->get_folder1_info());
    }

    CopyListObject *worker = new CopyListObject();
    worker->set_fileinfo_map_list(&fileinfo_map_list);
    worker->set_source_folder_list(&source_folder_list);
    worker->set_destination_folder_list(&destination_folder_list);
    worker->setPreflight(preflight);

    workerThread = new QThread(this);

    connect(workerThread, &QThread::started, worker, &CopyListObject::doWork);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(worker, &CopyListObject::endWork, workerThread, &QThread::quit);
    connect(worker, &CopyListObject::endWork, worker, &CopyListObject::deleteLater);
    connect(worker, &CopyListObject::endWork, this, &MainWindow::synchronize_clicked_2);
    connect(worker, &CopyListObject::warn, this, &MainWindow::warningHandler);
    connect(worker, &CopyListObject::fail, this, &MainWindow::failureHandler);
    connect(worker, &CopyListObject::abort, this, &MainWindow::abortHandler);

    worker->moveToThread(workerThread);

    // Starts an event loop, and emits workerThread->started()
    workerThread->start();
    syncRunning = true;
 }


void MainWindow::synchronize_clicked_2()
{
    syncRunning = false;

    ui->treeWidget_FilesToCopy1->clear();
    ui->treeWidget_FilesToOverwrite1->clear();
    ui->treeWidget_FilesToCopy2->clear();
    ui->treeWidget_FilesToOverwrite2->clear();

    statusBar()->showMessage(tr("Synchronize completed at ") + QDateTime::currentDateTime().toString("hh:mm:ss dd:MM:yyyy"));
}

void MainWindow::copy1to2_clicked()
{
    statusBar()->showMessage(tr("Copy 1 to 2 started"));

    fileinfo_map_list.clear();
    source_folder_list.clear();
    destination_folder_list.clear();

    for (int i = 0; i < m_fileSyncPairObject_list.size(); i++)
    {
        copyList(m_fileSyncPairObject_list[i]->get_folder1_to_copy(), m_fileSyncPairObject_list[i]->get_folder1_info(), m_fileSyncPairObject_list[i]->get_folder2_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder2_to_copy(), 0, m_fileSyncPairObject_list[i]->get_folder2_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder1_to_overwrite(), m_fileSyncPairObject_list[i]->get_folder1_info(), m_fileSyncPairObject_list[i]->get_folder2_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder2_to_overwrite(), m_fileSyncPairObject_list[i]->get_folder1_info(), m_fileSyncPairObject_list[i]->get_folder2_info());
    }

    CopyListObject *worker = new CopyListObject();
    worker->set_fileinfo_map_list(&fileinfo_map_list);
    worker->set_source_folder_list(&source_folder_list);
    worker->set_destination_folder_list(&destination_folder_list);
    worker->setPreflight(preflight);

    workerThread = new QThread(this);

    connect(workerThread, &QThread::started, worker, &CopyListObject::doWork);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(worker, &CopyListObject::endWork, workerThread, &QThread::quit);
    connect(worker, &CopyListObject::endWork, worker, &CopyListObject::deleteLater);
    connect(worker, &CopyListObject::endWork, this, &MainWindow::copy1to2_clicked_2);
    connect(worker, &CopyListObject::warn, this, &MainWindow::warningHandler);
    connect(worker, &CopyListObject::fail, this, &MainWindow::failureHandler);
    connect(worker, &CopyListObject::abort, this, &MainWindow::abortHandler);

    worker->moveToThread(workerThread);

    // Starts an event loop, and emits workerThread->started()
    workerThread->start();
 }


void MainWindow::copy1to2_clicked_2()
{
    ui->treeWidget_FilesToCopy1->clear();
    ui->treeWidget_FilesToOverwrite1->clear();
    ui->treeWidget_FilesToCopy2->clear();
    ui->treeWidget_FilesToOverwrite2->clear();

    statusBar()->showMessage(tr("Copy 1 to 2 completed at ") + QDateTime::currentDateTime().toString("hh:mm:ss dd:MM:yyyy"));
}

void MainWindow::copy2to1_clicked()
{
    statusBar()->showMessage(tr("Copy 2 to 1 started"));

    fileinfo_map_list.clear();
    source_folder_list.clear();
    destination_folder_list.clear();

    for (int i = 0; i < m_fileSyncPairObject_list.size(); i++)
    {
        copyList(m_fileSyncPairObject_list[i]->get_folder1_to_copy(), 0, m_fileSyncPairObject_list[i]->get_folder1_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder2_to_copy(), m_fileSyncPairObject_list[i]->get_folder2_info(), m_fileSyncPairObject_list[i]->get_folder1_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder1_to_overwrite(), m_fileSyncPairObject_list[i]->get_folder2_info(), m_fileSyncPairObject_list[i]->get_folder1_info());
        copyList(m_fileSyncPairObject_list[i]->get_folder2_to_overwrite(), m_fileSyncPairObject_list[i]->get_folder2_info(), m_fileSyncPairObject_list[i]->get_folder1_info());
    }

    CopyListObject *worker = new CopyListObject();
    worker->set_fileinfo_map_list(&fileinfo_map_list);
    worker->set_source_folder_list(&source_folder_list);
    worker->set_destination_folder_list(&destination_folder_list);
    worker->setPreflight(preflight);

    workerThread = new QThread(this);

    connect(workerThread, &QThread::started, worker, &CopyListObject::doWork);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    connect(worker, &CopyListObject::endWork, workerThread, &QThread::quit);
    connect(worker, &CopyListObject::endWork, worker, &CopyListObject::deleteLater);
    connect(worker, &CopyListObject::endWork, this, &MainWindow::copy2to1_clicked_2);
    connect(worker, &CopyListObject::warn, this, &MainWindow::warningHandler);
    connect(worker, &CopyListObject::fail, this, &MainWindow::failureHandler);
    connect(worker, &CopyListObject::abort, this, &MainWindow::abortHandler);

    worker->moveToThread(workerThread);

    // Starts an event loop, and emits workerThread->started()
    workerThread->start();
 }


void MainWindow::copy2to1_clicked_2()
{
    ui->treeWidget_FilesToCopy1->clear();
    ui->treeWidget_FilesToOverwrite1->clear();
    ui->treeWidget_FilesToCopy2->clear();
    ui->treeWidget_FilesToOverwrite2->clear();

    statusBar()->showMessage(tr("Copy 2 to 1 completed at ") + QDateTime::currentDateTime().toString("hh:mm:ss dd:MM:yyyy"));
}

void MainWindow::fill_tree_widget(QTreeWidgetItem *parent_tree_item, QMap<QString, QFileInfo> *fileinfo_map)
{
    QString common_subpath = parent_tree_item->text(0);
    // QMap seems to be faster because the strings can be quite long and the list is quite short often
    // QHash<QString, QTreeWidgetItem *> folder_list;
    QMap<QString, QTreeWidgetItem *> folder_list;

    for (QMap<QString, QFileInfo>::const_iterator iter = fileinfo_map->constBegin(); iter != fileinfo_map->constEnd(); iter++)
    {
        //qDebug() << iter.key();
        add_to_folder_list(parent_tree_item, &folder_list, &iter.key(), iter.value().isDir());
    }
    //print_QTreeWidgetItem(parent_tree_item, 0);
}


QTreeWidgetItem *MainWindow::add_to_folder_list(QTreeWidgetItem *root_item, QMap<QString, QTreeWidgetItem *> *folder_list, const QString *substring, bool isDir)
{
    QTreeWidgetItem *new_item;
    int last_slash = substring->lastIndexOf("/");
    QString parent_subpath = *substring;
    parent_subpath.truncate(last_slash);
    QString filename = *substring;
    filename.remove(0, last_slash + 1);
    if (parent_subpath.size() <= 0)
    {
        new_item = new QTreeWidgetItem(root_item);
        new_item->setText(0, filename);
        if (isDir) folder_list->insert(*substring, new_item);
        return new_item;
    }
    QMap<QString, QTreeWidgetItem *>::const_iterator folder_list_iter = folder_list->find(parent_subpath);
    if (folder_list_iter != folder_list->constEnd())
    {
        new_item = new QTreeWidgetItem(folder_list_iter.value());
        new_item->setText(0, filename);
        if (isDir) folder_list->insert(*substring, new_item);
        return new_item;
    }

    // parent isn't in the folder list so put it in (recurses if necessary)
    QFileInfo newInfo(parent_subpath);
    QTreeWidgetItem *parent_item = add_to_folder_list(root_item, folder_list, &parent_subpath, true);
    new_item = new QTreeWidgetItem(parent_item);
    new_item->setText(0, filename);
    if (isDir) folder_list->insert(*substring, new_item);
    return new_item;
}

void MainWindow::print_QTreeWidgetItem(QTreeWidgetItem* item, int depth)
{
    qDebug() << depth << " " << item->text(0);
    for (int i = 0; i < item->childCount(); i++)
    {
        print_QTreeWidgetItem(item->child(i), depth + 1);
    }
}

void  MainWindow::copyList(QMap<QString, QFileInfo> *fileinfo_map, QFileInfo *source_folder, QFileInfo *destination_folder)
{
    fileinfo_map_list.append(fileinfo_map);
    source_folder_list.append(source_folder);
    destination_folder_list.append(destination_folder);
}

void MainWindow::on_actionNew_triggered()
{
    if (maybeSave())
    {
        folder1_paths.clear();
        folder2_paths.clear();
        curFile.clear();
        QSettings settings("AnimalSimulation", "FuzzySyncQT");
        settings.setValue("LastOpenedFile", curFile);
        settings.sync();

        ui->treeWidget_FilesToCopy1->clear();
        ui->treeWidget_FilesToOverwrite1->clear();
        ui->treeWidget_FilesToCopy2->clear();
        ui->treeWidget_FilesToOverwrite2->clear();

        for (int i = 0; i < m_fileSyncPairObject_list.size(); i++) m_fileSyncPairObject_list[i]->deleteLater();
        m_fileSyncPairObject_list.clear();

        setWindowModified(false);
        setWindowFilePath("");
        setWindowTitle("FuzzySyncQT");
    }
}


void MainWindow::on_actionOpen_triggered()
{
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
        {
            loadFile(fileName);
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionSave_As_triggered()
{
    saveAs();
}

void MainWindow::on_actionAbout_triggered()
{
   QMessageBox::about(this, tr("About FuzzySyncQT"),
            tr("<b>FuzzySyncQT</b> is an application for synchronising files across folders "
               "based on their modification dates and sizes.\n\n"
               "This application has been written by Bill Sellers (c) 2013 and is distributed "
               "under the GNU copyleft licence."));
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionPreferences_triggered()
{

}

bool MainWindow::save()
{
    if (curFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MainWindow::maybeSave()
{
    if (isWindowModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("FuzzySyncQT"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("FuzzySyncQT"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    folder1_paths.clear();
    folder2_paths.clear();

    ui->treeWidget_FilesToCopy1->clear();
    ui->treeWidget_FilesToOverwrite1->clear();
    ui->treeWidget_FilesToCopy2->clear();
    ui->treeWidget_FilesToOverwrite2->clear();

    for (int i = 0; i < m_fileSyncPairObject_list.size(); i++) m_fileSyncPairObject_list[i]->deleteLater();
    m_fileSyncPairObject_list.clear();

    QByteArray data = file.readAll();
    file.close();

    QString dataString(data);
    QStringList lines = dataString.split("\n", QString::SkipEmptyParts);

    for (int i = 0; i < lines.size(); i++)
    {
        QStringList tokens = lines[i].split("\t", QString::SkipEmptyParts);
        if (tokens.size() >= 2)
        {
            folder1_paths.append(tokens[0]);
            folder2_paths.append(tokens[1]);
        }
    }


    QSettings settings("AnimalSimulation", "FuzzySyncQT");
    settings.setValue("LastOpenedFile", fileName);
    settings.sync();
    curFile = fileName;
    setWindowModified(false);
    setWindowFilePath(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);

}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("FuzzySyncQT"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    for (int i = 0; i < folder1_paths.size(); i++)
    {
        out << folder1_paths[i] << "\t" << folder2_paths[i] << "\n";
    }
    file.close();

    QSettings settings("AnimalSimulation", "FuzzySyncQT");
    settings.setValue("LastOpenedFile", fileName);
    settings.sync();
    curFile = fileName;
    setWindowModified(false);
    setWindowFilePath(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::warningHandler(QString message)
{
    // QMessageBox::warning(this, tr("Non fatal error"), message);
    errorReporterDialog->show();
    errorReporterDialog->raise();
    errorReporterDialog->AppendText(message + "\n");
}

void MainWindow::failureHandler(QString message)
{
    // QMessageBox::warning(this, tr("Fatal error"), message);
    errorReporterDialog->show();
    errorReporterDialog->raise();
    errorReporterDialog->AppendText(message + "\n");
}

void MainWindow::abortHandler(QString message)
{
    // QMessageBox::warning(this, tr("Aborted"), message);
    errorReporterDialog->show();
    errorReporterDialog->raise();
    errorReporterDialog->AppendText(message + "\n");
}

void MainWindow::abort_clicked()
{
    abortFlag = 1;
}
