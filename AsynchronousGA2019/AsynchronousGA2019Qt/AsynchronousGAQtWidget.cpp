#include "AsynchronousGAQtWidget.h"
#include "ui_AsynchronousGAQtWidget.h"

#ifdef USE_TCP
#include "GATCP.h"
#endif
#ifdef USE_UDP
#include "GAUDP.h"
#endif
#ifdef USE_TCP_PTYPES
#include "GATCPptypes.h"
#endif
#ifdef USE_ENET
#include "GAENET.h"
#endif
#ifdef USE_UDT
#include "GAUDT.h"
#endif
#ifdef USE_ASIO
#include "GAASIO.h"
#endif

#include "MergeXML.h"
#include "Utilities.h"
#include "TextEditDialog.h"

#include "pystring.h"

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QStringList>
#include <QByteArray>
#include <QSettings>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <qglobal.h>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDateTime>
#include <QDomDocument>
#include <QFontDialog>
#include <QRegularExpression>

using namespace std::string_literals;

AsynchronousGAQtWidget::AsynchronousGAQtWidget(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AsynchronousGAQtWidget)
{
    ui->setupUi(this);

    m_redirect = std::make_unique<cerrRedirect>(m_capturedCerr.rdbuf());

    m_editorFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    m_mergeXMLTimer = new QTimer(this); // this timer
    connect(m_mergeXMLTimer, SIGNAL(timeout()), this, SLOT(runMergeXML()));

    // adjust the appearance of the main window
    ui->mainToolBar->hide();

    // set up the menus
    createActions();
    createMenus();

    // set up the file browse slots
    connect(ui->pushButtonParameterFile, SIGNAL(clicked()), this, SLOT(pushButtonParameterFileClicked()));
    connect(ui->pushButtonStartingPopulationFile, SIGNAL(clicked()), this, SLOT(pushButtonStartingPopulationFileClicked()));
    connect(ui->pushButtonXMLMasterFile, SIGNAL(clicked()), this, SLOT(pushButtonXMLMasterFileClicked()));
    connect(ui->pushButtonOutputFolder, SIGNAL(clicked()), this, SLOT(pushButtonOutputFolderClicked()));
    connect(ui->pushButtonModelConfigurationFile, SIGNAL(clicked()), this, SLOT(pushButtonModelConfigurationFileClicked()));
    connect(ui->pushButtonModelPopulationFile, SIGNAL(clicked()), this, SLOT(pushButtonModelPopulationFileClicked()));
    connect(ui->pushButtonDriverFile, SIGNAL(clicked()), this, SLOT(pushButtonDriverFileClicked()));
    connect(ui->pushButtonWorkingFolder, SIGNAL(clicked()), this, SLOT(pushButtonWorkingFolderClicked()));
    connect(ui->pushButtonMergeXMLFile, SIGNAL(clicked()), this, SLOT(pushButtonMergeXMLFileClicked()));
    connect(ui->pushButtonGaitSymExecutable, SIGNAL(clicked()), this, SLOT(pushButtonGaitSymExecutableClicked()));
    connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(pushButtonStartClicked()));
    connect(ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(pushButtonStopClicked()));

    // handle the check box
    connect(ui->checkBoxMergeXMLActivate, SIGNAL(clicked()), this, SLOT(checkBoxMergeXMLActivateClicked()));

    // and this watches the running process
    connect(&m_watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

    // and this updates the buttons too
    connect(ui->lineEditOutputFolder, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditOutputFolderTextChanged(const QString &)));
    connect(ui->lineEditParameterFile, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditParameterFileTextChanged(const QString &)));
    connect(ui->lineEditXMLMasterFile, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditXMLMasterFileTextChanged(const QString &)));
    connect(ui->lineEditStartingPopulationFile, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditStartingPopulationFileTextChanged(const QString &)));
    connect(ui->lineEditModelConfigurationFile, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditModelConfigurationFileTextChanged(const QString &)));
    connect(ui->lineEditModelPopulationFile, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditModelPopulationFileTextChanged(const QString &)));
    connect(ui->lineEditDriverFile, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditDriverFileTextChanged(const QString &)));
    connect(ui->lineEditMergeXMLFile, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditMergeXMLFileTextChanged(const QString &)));
    connect(ui->lineEditGaitSymExecutable, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditGaitSymExecutableTextChanged(const QString &)));
    connect(ui->spinBoxLogLevel, SIGNAL(textChanged(const QString &)), this, SLOT(spinBoxTextChanged(const QString &)));
    connect(ui->spinBoxPort, SIGNAL(textChanged(const QString &)), this, SLOT(spinBoxTextChanged(const QString &)));
    connect(ui->doubleSpinBoxEndValue, SIGNAL(textChanged(const QString &)), this, SLOT(spinBoxTextChanged(const QString &)));
    connect(ui->doubleSpinBoxOutputCycle, SIGNAL(textChanged(const QString &)), this, SLOT(spinBoxTextChanged(const QString &)));
    connect(ui->doubleSpinBoxStartValue, SIGNAL(textChanged(const QString &)), this, SLOT(spinBoxTextChanged(const QString &)));
    connect(ui->doubleSpinBoxStepValue, SIGNAL(textChanged(const QString &)), this, SLOT(spinBoxTextChanged(const QString &)));
    connect(ui->checkBoxMergeXMLActivate, SIGNAL(stateChanged(int)), this, SLOT(checkBoxStateChanged(int)));
    connect(ui->checkBoxCycleTime, SIGNAL(stateChanged(int)), this, SLOT(checkBoxStateChanged(int)));

    // put a new context menu on the linedit widgets that point to file names
    QList<QLineEdit *> listQLineEdit = this->findChildren<QLineEdit *>(QString(), Qt::FindChildrenRecursively);
    for (QList<QLineEdit *>::iterator it = listQLineEdit.begin(); it != listQLineEdit.end(); it++)
    {
        if ((*it) == ui->lineEditWorkingFolder || (*it) == ui->lineEditOutputFolder || (*it) == ui->lineEditGaitSymExecutable) continue;
        (*it)->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect((*it), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(menuRequestPath(QPoint)));
    }

    readSettings();
    setEditorFonts();

    QString asynchronousGAFileName = m_asynchronousGAFileName;
    newFile();
    QFileInfo fileInfo(asynchronousGAFileName);
    if (fileInfo.exists() && fileInfo.isFile()) open(asynchronousGAFileName);

    // and sort out the buttons
    activateButtons();

    ui->statusBar->showMessage("Ready");
    m_asynchronousGAFileModified = false;
    setCustomTitle();

    // and this timer just makes sure that buttons are regularly updated
    m_basicTimer.start(1000, Qt::CoarseTimer, this);
}

AsynchronousGAQtWidget::~AsynchronousGAQtWidget()
{
    delete ui;
}

void AsynchronousGAQtWidget::pushButtonParameterFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Parameter File"), ui->lineEditParameterFile->text(), tr("Text Files (*.txt *.tab);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        ui->lineEditParameterFile->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonXMLMasterFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select XML Master File"), ui->lineEditXMLMasterFile->text(), tr("GaitSym XML Files (*.gaitsym *.xml);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        ui->lineEditXMLMasterFile->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonStartingPopulationFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Starting Population File"), ui->lineEditStartingPopulationFile->text(), tr("Text Files (*.txt *.tab);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        ui->lineEditStartingPopulationFile->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonOutputFolderClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select GA Output Folder"), ui->lineEditOutputFolder->text());
    if (!fileName.isEmpty())
    {
        ui->lineEditOutputFolder->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonModelConfigurationFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Model Configuration File"), ui->lineEditModelConfigurationFile->text(), tr("GaitSym XML Files (*.gaitsym *.xml);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        ui->lineEditModelConfigurationFile->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonModelPopulationFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Model Population File"), ui->lineEditModelPopulationFile->text(), tr("Text Files (*.txt *.tab);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        ui->lineEditModelPopulationFile->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonDriverFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the Driver File"), ui->lineEditDriverFile->text(), tr("GaitSym XML Files (*.gaitsym *.xml);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        ui->lineEditDriverFile->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonWorkingFolderClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select MergeXML Working Folder"), ui->lineEditWorkingFolder->text());
    if (!fileName.isEmpty())
    {
        QString lastText = ui->lineEditWorkingFolder->text();
        if (lastText != fileName)
        {
            if (ui->lineEditParameterFile->text().startsWith(lastText)) ui->lineEditParameterFile->setText(ui->lineEditParameterFile->text().replace(lastText, fileName));
            if (ui->lineEditStartingPopulationFile->text().startsWith(lastText)) ui->lineEditStartingPopulationFile->setText(ui->lineEditStartingPopulationFile->text().replace(lastText, fileName));
            if (ui->lineEditXMLMasterFile->text().startsWith(lastText)) ui->lineEditXMLMasterFile->setText(ui->lineEditXMLMasterFile->text().replace(lastText, fileName));
            if (ui->lineEditOutputFolder->text().startsWith(lastText)) ui->lineEditOutputFolder->setText(ui->lineEditOutputFolder->text().replace(lastText, fileName));
            if (ui->lineEditModelConfigurationFile->text().startsWith(lastText)) ui->lineEditModelConfigurationFile->setText(ui->lineEditModelConfigurationFile->text().replace(lastText, fileName));
            if (ui->lineEditModelPopulationFile->text().startsWith(lastText)) ui->lineEditModelPopulationFile->setText(ui->lineEditModelPopulationFile->text().replace(lastText, fileName));
            if (ui->lineEditDriverFile->text().startsWith(lastText)) ui->lineEditDriverFile->setText(ui->lineEditDriverFile->text().replace(lastText, fileName));
            if (ui->lineEditMergeXMLFile->text().startsWith(lastText)) ui->lineEditMergeXMLFile->setText(ui->lineEditMergeXMLFile->text().replace(lastText, fileName));
            if (m_asynchronousGAFileName.startsWith(lastText)) m_asynchronousGAFileName = m_asynchronousGAFileName.replace(lastText, fileName);
            m_asynchronousGAFileNameValid = false;
            m_asynchronousGAFileModified = true;
            ui->lineEditWorkingFolder->setText(fileName);
            setCustomTitle();
        }
    }
}

void AsynchronousGAQtWidget::pushButtonMergeXMLFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the MergeXML File"), ui->lineEditMergeXMLFile->text(), tr("Text Files (*.txt *.tab);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        ui->lineEditMergeXMLFile->setText(fileName);
    }
}

void AsynchronousGAQtWidget::pushButtonGaitSymExecutableClicked()
{
#if defined(_WIN32) || defined(WIN32)
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the GaitSym executable"), ui->lineEditGaitSymExecutable->text(), tr("Executable Files (*.exe);;Any File (*.* *)"));
#else
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select the GaitSym executable"), ui->lineEditGaitSymExecutable->text(), tr("Executable Files (*)"));
#endif
    if (!fileName.isEmpty())
    {
        ui->lineEditGaitSymExecutable->setText(fileName);
    }
}


void AsynchronousGAQtWidget::checkBoxMergeXMLActivateClicked()
{
    activateButtons();
}

void AsynchronousGAQtWidget::pushButtonStartClicked()
{
    Q_ASSERT(!m_watcher.isRunning());
    Q_ASSERT(!ui->lineEditParameterFile->text().isEmpty());
    writeSettings();

    if (ui->checkBoxMergeXMLActivate->isChecked())
    {
        m_runMergeXML = 1;
        m_currentLoopValue = ui->doubleSpinBoxStartValue->value();
        m_mergeXMLTimer->start(1000);
    }
    else
    {
        runGA();
    }
    m_tryToStopGA = false;
    activateButtons();
}

void AsynchronousGAQtWidget::runGA()
{
    Q_ASSERT(!m_watcher.isRunning());
    Q_ASSERT(!ui->lineEditParameterFile->text().isEmpty());

    if (ui->spinBoxLogLevel->value() > 0) appendProgress("Running the GA");
    ui->statusBar->showMessage("Running the GA");
    ui->lineEditBestScore->setText("");
    ui->lineEditResultNumber->setText("");
    ui->lineEditGenerationsPerSecond->setText("");
    m_lastResultsTime = QDateTime::currentDateTime();
    m_lastResultsNumber = -1;
    // just run the GA
    QFileInfo parameterFile(ui->lineEditParameterFile->text());
    QFileInfo startingPopulationFile(ui->lineEditStartingPopulationFile->text());
    QFileInfo xmlMasterFile(ui->lineEditXMLMasterFile->text());
    QFileInfo outputFolder(ui->lineEditOutputFolder->text());
    int logLevel = ui->spinBoxLogLevel->value();

    QString parentDir = parameterFile.canonicalPath();
    if (m_runMergeXML) parentDir = ui->lineEditWorkingFolder->text();
    QDir::setCurrent(parentDir);

    m_ga = std::make_unique<GAMain>(this);
    m_ga->SetLogLevel(logLevel);
    m_ga->SetServerPort(ui->spinBoxPort->value());
    m_xmlMasterFile = xmlMasterFile.absoluteFilePath().toStdString();
    m_ga->LoadBaseXMLFile(m_xmlMasterFile);
    m_outputFolder = outputFolder.absoluteFilePath().toStdString();
    m_parameterFile = parameterFile.absoluteFilePath().toStdString();
    m_startingPopulationFile = startingPopulationFile.absoluteFilePath().toStdString();
#if QT_VERSION >= 0x060000
    QFuture<int> future = QtConcurrent::run(&GAMain::Process, m_ga.get(), m_parameterFile, m_outputFolder, m_startingPopulationFile);
#else
    QFuture<int> future = QtConcurrent::run(m_ga.get(), &GAMain::Process, m_parameterFile, m_outputFolder, m_startingPopulationFile);
#endif
    m_watcher.setFuture(future);
}

void AsynchronousGAQtWidget::runMergeXML()
{
    if (m_watcher.isRunning()) return;
    if (m_capturedCerr.str().size()) { appendProgress(QString::fromStdString(m_capturedCerr.str())); m_capturedCerr.clear(); }

    double startValue = ui->doubleSpinBoxStartValue->value();
    double stepValue = ui->doubleSpinBoxStepValue->value();
    double endValue = ui->doubleSpinBoxEndValue->value();
    if ((stepValue >= 0 && m_currentLoopValue >= endValue) || (stepValue < 0 && m_currentLoopValue <= endValue))
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress("MergeXML finished");
        ui->statusBar->showMessage("MergeXML finished");
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        activateButtons();
        return;
    }

    if (ui->spinBoxLogLevel->value() > 0) appendProgress("Running MergeXML");
    ui->statusBar->showMessage("Running MergeXML");
    QString lastPopulation;
    QString lastConfig;
    if (m_runMergeXML == 1)
    {
        // get the population and config from the models
        m_runMergeXML = 2;
        QFileInfo modelConfigurationFile(ui->lineEditModelConfigurationFile->text());
        QFileInfo modelPopulationFile(ui->lineEditModelPopulationFile->text());
        lastPopulation = modelPopulationFile.absoluteFilePath();
        lastConfig = modelConfigurationFile.absoluteFilePath();
        m_currentLoopValue = startValue;
        m_currentLoopCount = 0;
    }
    else
    {
        // get the population and config from the output of the previous runGA
        runGaitSym(); // run gaitsym and generate ModelState.xml
        QString outputFolder = ui->lineEditOutputFolder->text();
        QDir dir(outputFolder);
        lastConfig = dir.filePath("ModelState.xml");
        if (dir.exists("ModelState.xml") == false)
        {
            m_mergeXMLTimer->stop();
            m_runMergeXML = 0;
            if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("MergeXML: Unable to find ModelState.xml in:\n%1").arg(outputFolder));
            QMessageBox::warning(this, tr("Directory Read Error"), QString("MergeXML: Unable to find ModelState.xml in:\n%1").arg(outputFolder));
            activateButtons();
            return;
        }
        QStringList files = dir.entryList(QStringList() << "Population*.txt", QDir::Files, QDir::Name);
        if (files.isEmpty())
        {
            m_mergeXMLTimer->stop();
            m_runMergeXML = 0;
            if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("MergeXML: Unable to find Population*.txt in:\n%1").arg(outputFolder));
            QMessageBox::warning(this, tr("Directory Read Error"), QString("MergeXML: Unable to find Population*.txt in:\n%1").arg(outputFolder));
            activateButtons();
            return;
        }
        lastPopulation = dir.filePath(files.last());
        m_currentLoopCount++;
        m_currentLoopValue += stepValue;
        if (stepValue >= 0 && (m_currentLoopValue > endValue || fabs(m_currentLoopValue - endValue) < fabs(stepValue) * 0.001)) m_currentLoopValue = endValue;
        if (stepValue < 0 && (m_currentLoopValue < endValue || fabs(m_currentLoopValue - endValue) < fabs(stepValue) * 0.001)) m_currentLoopValue = endValue;
    }
    ui->lineEditCurrentLoopValue->setText(QString::number(m_currentLoopValue, 'f', 3));
    ui->lineEditCurrentLoopCount->setText(QString::number(m_currentLoopCount, 'f', 3));
    QFileInfo driverFile(ui->lineEditDriverFile->text());
    QFileInfo workingFolder(ui->lineEditWorkingFolder->text());
    QFile mergeXMLFile(ui->lineEditMergeXMLFile->text());
    if (mergeXMLFile.open(QFile::ReadOnly) == false)
    {
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("MergeXML: Unable to open file:\n%1").arg(ui->lineEditMergeXMLFile->text()));
        QMessageBox::warning(this, tr("Open File Error"), QString("MergeXML: Unable to open file:\n%1").arg(ui->lineEditMergeXMLFile->text()));
        activateButtons();
        return;
    }
    QByteArray mergeXMLFileData = mergeXMLFile.readAll();
    mergeXMLFile.close();
    QString mergeXMLCommands = QString::fromUtf8(mergeXMLFileData);
    QDateTime time = QDateTime::currentDateTime();
    QString subFolder(QString("%1_Run_%2").arg(m_currentLoopCount, 4, 10, QChar('0')).arg(time.toString("yyyy-MM-dd_HH.mm.ss")));
    QString outputFolder = QDir(workingFolder.absoluteFilePath()).filePath(subFolder);
    ui->lineEditOutputFolder->setText(outputFolder);
    QDir().mkpath(outputFolder);
    if (QFileInfo(outputFolder).isDir() == false)
    {
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("MergeXML: Unable to create folder:\n%1").arg(outputFolder));
        QMessageBox::warning(this, tr("Create Folder Error"), QString("MergeXML: Unable to create folder:\n%1").arg(outputFolder));
        activateButtons();
        return;
    }
    QString workingConfig = QDir(outputFolder).filePath("workingConfig.xml");
    ui->lineEditXMLMasterFile->setText(workingConfig);
    QFileInfo firstConfigFile(ui->lineEditModelConfigurationFile->text());
    QString replace1 = mergeXMLCommands.replace("MODEL_CONFIG_FILE", lastConfig);
    QString replace2 = replace1.replace("DRIVER_CONFIG_FILE", driverFile.absoluteFilePath());
    QString replace3 = replace2.replace("WORKING_CONFIG_FILE", workingConfig);
    QString replace4 = replace3.replace("CURRENT_LOOP_VALUE", QString::number(m_currentLoopValue, 'g', 17));
    QString replace5 = replace4.replace("FIRST_CONFIG_FILE", firstConfigFile.absoluteFilePath());
    MergeXML mergeXML;
    mergeXML.ExecuteInstructionFile(replace5.toUtf8().constData());
    if (mergeXML.errorMessageList().size())
    {
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0)
        {
            appendProgress(QString("MergeXML: Unable to parse file:\n%1").arg(ui->lineEditMergeXMLFile->text()));
            for (auto &&it : mergeXML.errorMessageList())
            {
                appendProgress(QString("Line %1: %2\n").arg(it.first).arg(QString::fromStdString(it.second)));
                if (ui->spinBoxLogLevel->value() > 1)
                {
                    auto tokensIt = mergeXML.errorList().find(it.first);
                    if (tokensIt != mergeXML.errorList().end())
                    {
                        std::string tokens = pystring::join(" "s, tokensIt->second);
                        appendProgress(QString("%1").arg(QString::fromStdString(tokens)));
                    }
                }
            }
        }
        QMessageBox::warning(this, tr("Create Folder Error"), QString("MergeXML: Unable to parse file:\n%1").arg(ui->lineEditMergeXMLFile->text()));
        activateButtons();
        return;
    }
    QString newMergeXML = QDir(outputFolder).filePath("workingMergeXML.txt");
    QFile file(newMergeXML);
    file.open(QFile::WriteOnly);
    file.write(replace4.toUtf8());
    file.close();
    QString mergeXMLStatusFile = QDir(outputFolder).filePath("mergeXMLStatus.txt");
    QFile file2(mergeXMLStatusFile);
    file2.open(QFile::WriteOnly);
    QTextStream stream(&file2);
    double outputCycle = ui->doubleSpinBoxOutputCycle->value();
    bool cycle = ui->checkBoxCycleTime->isChecked();
    stream << "startValue " << startValue << "\n";
    stream << "stepValue " << stepValue << "\n";
    stream << "endValue " << endValue << "\n";
    stream << "currentLoopCount " << m_currentLoopCount << "\n";
    if (cycle) stream << "outputCycle " << outputCycle << "\n";
    if (cycle) stream << "outputTime " << outputCycle << "\n";
    stream << "CURRENT_LOOP_VALUE " << m_currentLoopValue << "\n";
    stream << "MODEL_CONFIG_FILE " << lastConfig << "\n";
    stream << "DRIVER_CONFIG_FILE " << driverFile.absoluteFilePath() << "\n";
    stream << "WORKING_CONFIG_FILE " << workingConfig << "\n";
    stream << "FIRST_CONFIG_FILE " << firstConfigFile.absoluteFilePath() << "\n";
    file2.close();
    ui->lineEditXMLMasterFile->setText(workingConfig);
    QString newPopulation = QDir(outputFolder).filePath("workingPopulation.txt");
    QFile::copy(lastPopulation, newPopulation);
    ui->lineEditStartingPopulationFile->setText(newPopulation);
    runGA();
}

void AsynchronousGAQtWidget::runGaitSym()
{
    if (ui->spinBoxLogLevel->value() > 0) appendProgress("Running GaitSym");
    ui->statusBar->showMessage("Running GaitSym");
    double outputCycle = ui->doubleSpinBoxOutputCycle->value();
    bool cycleFlag = ui->checkBoxCycleTime->isChecked();
    QString outputFolder = ui->lineEditOutputFolder->text();
    QDir dir(outputFolder);
    QStringList files = dir.entryList(QStringList() << "BestGenome*.txt", QDir::Files, QDir::Name);
    if (files.size() < 1)
    {
        m_tryToStopGA = true;
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("runGaitSym: Unable to find BestGenome*.txt"));
        QMessageBox::warning(this, tr("Run GaitSym Error"), QString("runGaitSym: Unable to find BestGenome*.txt"));
        return;
    }
    QString inputGenome = dir.filePath(files.last());
    QString inputXML = dir.filePath("workingConfig.xml");
    QFileInfo inputGenomeFile(inputGenome);
    QString outputXML = dir.filePath(inputGenomeFile.completeBaseName() + QString(".xml"));
    GAMain::ApplyGenome(inputGenome.toStdString(), inputXML.toStdString(), outputXML.toStdString());
    QString modelStateFileName = dir.filePath("ModelState.xml");
//    QFile configFile(outputXML);
//    configFile.open(QFile::ReadOnly);
//    QByteArray data = configFile.readAll();
//    configFile.close();

    QString gaitSymExecutable = ui->lineEditGaitSymExecutable->text();
    QFileInfo executableInfo(gaitSymExecutable);
    bool gaitSymExecutableValid = false;
    if (executableInfo.exists() && executableInfo.isFile() && executableInfo.isExecutable()) gaitSymExecutableValid = true;
    if (!gaitSymExecutableValid)
    {
        m_tryToStopGA = true;
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("runGaitSym: Unable to find \"%1\"").arg(gaitSymExecutable));
        QMessageBox::warning(this, tr("Run GaitSym Error"), QString("runGaitSym: Unable to find \"%1\"").arg(gaitSymExecutable));
        return;
    }
    QStringList arguments;
    QString outputCycleArgument;
    if (cycleFlag) outputCycleArgument = "--outputModelStateAtCycle";
    else outputCycleArgument = "--outputModelStateAtTime";
    arguments << "--config" << outputXML
              << outputCycleArgument << QString::number(outputCycle)
              << "--modelState" << modelStateFileName;
    QProcess gaitsym;
    gaitsym.start(gaitSymExecutable, arguments);
    if (!gaitsym.waitForStarted())
    {
        m_tryToStopGA = true;
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("runGaitSym: Unable to start \"%1\"").arg(gaitSymExecutable));
        QMessageBox::warning(this, tr("Run GaitSym Error"), QString("runGaitSym: Unable to start \"%1\"").arg(gaitSymExecutable));
        return;
    }
    int msecs = 60 * 60 * 1000;
    if (!gaitsym.waitForFinished(msecs)) // this should work although is gaitsym finished really quickly it is not guaranteed
    {
        m_tryToStopGA = true;
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("runGaitSym: Timeout \"%1\"").arg(gaitSymExecutable));
        QMessageBox::warning(this, tr("Run GaitSym Error"), QString("runGaitSym: Timeout \"%1\"").arg(gaitSymExecutable));
        return;
    }
    QByteArray stdOut = gaitsym.readAllStandardOutput();
    if (ui->spinBoxLogLevel->value() > 1) appendProgress(QString::fromStdString(stdOut.toStdString()));
    QByteArray stdError = gaitsym.readAllStandardError();
    if (ui->spinBoxLogLevel->value() > 1) appendProgress(QString::fromStdString(stdError.toStdString()));
    QFileInfo modelStateInfo(modelStateFileName);
    if (!modelStateInfo.exists() && !modelStateInfo.isFile())
    {
        m_tryToStopGA = true;
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("runGaitSym: Unable to create:\n%1").arg(modelStateFileName));
        QMessageBox::warning(this, tr("Run GaitSym Error"), QString("runGaitSym: Unable to create:\n%1").arg(modelStateFileName));
        return;
    }
}

void AsynchronousGAQtWidget::pushButtonStopClicked()
{
    if (ui->spinBoxLogLevel->value() > 0) appendProgress("Stop Button Clicked - trying to abort GA");
    ui->statusBar->showMessage("Stop Button Clicked - trying to abort GA");
    m_tryToStopGA = true;
    m_mergeXMLTimer->stop();
    m_runMergeXML = 0;
    activateButtons();
}

void AsynchronousGAQtWidget::handleFinished()
{
    int result = 0;
    if (m_watcher.future().isValid())
    {
        try
        {
            // this prevents an unhandled exception if there are no results to wait for (which is almost certainly an error anyway)
            result = m_watcher.future().result();
        }
        catch (...)
        {
            appendProgress("Error m_watcher.future().result() exception");
            result = __LINE__;
        }
    }
    else
    {
        appendProgress("Error m_watcher.future().isValid() is false");
        result = __LINE__;
    }
    if (m_capturedCerr.str().size()) { appendProgress(QString::fromStdString(m_capturedCerr.str())); m_capturedCerr.clear(); }
    if (result == 0)
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress("GA finished");
        ui->statusBar->showMessage("GA finished");
        m_tryToStopGA = false;
        ui->progressBar->setValue(0);
    }
    else
    {
        appendProgress("Error running GA");
        ui->statusBar->showMessage("Error running GA");
        m_tryToStopGA = false;
        m_mergeXMLTimer->stop();
        m_runMergeXML = 0;
    }
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditOutputFolderTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditParameterFileTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditXMLMasterFileTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditStartingPopulationFileTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditModelConfigurationFileTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditModelPopulationFileTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditDriverFileTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditMergeXMLFileTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditWorkingFolderTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::lineEditGaitSymExecutableTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::spinBoxTextChanged(const QString & /*text*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::checkBoxStateChanged(int /*state*/)
{
    m_asynchronousGAFileModified = true;
    activateButtons();
}

void AsynchronousGAQtWidget::activateButtons()
{
    ui->pushButtonParameterFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    ui->pushButtonStartingPopulationFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    ui->pushButtonXMLMasterFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    ui->pushButtonOutputFolder->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    ui->pushButtonModelConfigurationFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->pushButtonModelPopulationFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->pushButtonDriverFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->pushButtonWorkingFolder->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->pushButtonMergeXMLFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->pushButtonGaitSymExecutable->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditOutputFolder->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && !ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditParameterFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    ui->lineEditXMLMasterFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && !ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditStartingPopulationFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && !ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditModelConfigurationFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditModelPopulationFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditDriverFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditWorkingFolder->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditMergeXMLFile->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditGaitSymExecutable->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditCurrentLoopCount->setEnabled(ui->checkBoxMergeXMLActivate->isChecked());
    ui->lineEditCurrentLoopValue->setEnabled(ui->checkBoxMergeXMLActivate->isChecked());
    ui->spinBoxLogLevel->setEnabled(true);
    ui->spinBoxPort->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    ui->doubleSpinBoxStartValue->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->doubleSpinBoxStepValue->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->doubleSpinBoxEndValue->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->doubleSpinBoxOutputCycle->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->checkBoxCycleTime->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && ui->checkBoxMergeXMLActivate->isChecked());
    ui->pushButtonStop->setEnabled(m_watcher.isRunning() || m_runMergeXML != 0);
    ui->checkBoxMergeXMLActivate->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);

    QList<QLabel *> labels = ui->groupBoxMergeXMLControls->findChildren<QLabel *>();
    for (auto &&it : labels) it->setEnabled(ui->checkBoxMergeXMLActivate->isChecked());

    if (ui->checkBoxMergeXMLActivate->isChecked())
    {
        bool gaitSymExecutableValid = false;
        QFileInfo executableInfo(ui->lineEditGaitSymExecutable->text());
        if (executableInfo.exists() && executableInfo.isFile() && executableInfo.isExecutable()) gaitSymExecutableValid = true;
        ui->pushButtonStart->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && !ui->lineEditModelConfigurationFile->text().isEmpty() &&
                                        !ui->lineEditModelPopulationFile->text().isEmpty() && !ui->lineEditDriverFile->text().isEmpty() &&
                                        !ui->lineEditMergeXMLFile->text().isEmpty() && gaitSymExecutableValid);
    }
    else
    {
        ui->pushButtonStart->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && !ui->lineEditParameterFile->text().isEmpty() &&
                                        !ui->lineEditStartingPopulationFile->text().isEmpty() && !ui->lineEditXMLMasterFile->text().isEmpty());
    }

    newAct->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    openAct->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    saveAct->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && m_asynchronousGAFileModified && !m_asynchronousGAFileName.isEmpty() && m_asynchronousGAFileNameValid);
    saveAsAct->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0);
    editSettingsAct->setEnabled(!m_watcher.isRunning() && m_runMergeXML == 0 && !m_asynchronousGAFileName.isEmpty() && m_asynchronousGAFileNameValid);
    exitAct->setEnabled(true);

    if (ui->checkBoxCycleTime->isChecked()) ui->labelCycleTime->setText("Cycle");
    else ui->labelCycleTime->setText("Time");

//    if (m_watcher.isRunning())
//    {
//        QList<QWidget *> childWidgets = this->findChildren<QWidget *>(QString(), Qt::FindChildrenRecursively);
//        for (QList<QWidget *>::iterator it = childWidgets.begin(); it != childWidgets.end(); it++) (*it)->setEnabled(false);
//        ui->pushButtonStop->setEnabled(true);
//        ui->spinBoxLogLevel->setEnabled(true);
//        exitAct->setEnabled(false);
//    }
//    else
//    {
//        QList<QWidget *> childWidgets = this->findChildren<QWidget *>(QString(), Qt::FindChildrenRecursively);
//        for (QList<QWidget *>::iterator it = childWidgets.begin(); it != childWidgets.end(); it++) (*it)->setEnabled(true);
//        ui->pushButtonStop->setEnabled(false);
//    }
}

void AsynchronousGAQtWidget::appendProgress(const QString &text)
{
    ui->plainTextEditLog->appendPlainText(text);
}

bool AsynchronousGAQtWidget::tryToStopGA()
{
    return m_tryToStopGA;
}

int AsynchronousGAQtWidget::logLevel()
{
    return ui->spinBoxLogLevel->value();
}

void AsynchronousGAQtWidget::setProgressValue(int value)
{
    ui->progressBar->setValue(value);
}

void AsynchronousGAQtWidget::setResultNumber(int number)
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 millisecondsDiff = m_lastResultsTime.msecsTo(now);
    if (millisecondsDiff == 0) return;
    ui->lineEditResultNumber->setText(QString::number(number));
    int resultsDiff = number - m_lastResultsNumber;
    m_lastResultsTime = now;
    m_lastResultsNumber = number;
    double generationsPerSecond = 1000.0 * double(resultsDiff) / double(millisecondsDiff);
    ui->lineEditGenerationsPerSecond->setText(QString::number(generationsPerSecond));
}

void AsynchronousGAQtWidget::setBestScore(double value)
{
    ui->lineEditBestScore->setText(QString::number(value, 'g', 6));
}

void AsynchronousGAQtWidget::readSettings()
{
    QSettings settings("ASL", "AsynchronousGAQt2019");
    m_asynchronousGAFileName = settings.value("asynchronousGAFileFileName", "").toString();
    m_editorFont = settings.value("editorFont", QFont()).value<QFont>();
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    setCustomTitle();
}

void AsynchronousGAQtWidget::writeSettings()
{
    QSettings settings("ASL", "AsynchronousGAQt2019");
    settings.setValue("asynchronousGAFileFileName", m_asynchronousGAFileName);
    settings.setValue("editorFont", m_editorFont);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.sync();
}

void AsynchronousGAQtWidget::closeEvent (QCloseEvent *event)
{
    if (m_watcher.isRunning())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, "AsynchronousGAQt2019", "GA is running.\nAre you sure you want to close?.", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No)
        {
            event->ignore();
            return;
        }
        ui->statusBar->showMessage("Waiting for clean GA shutdown");
        ui->statusBar->repaint();
        qApp->processEvents();
        pushButtonStopClicked();
        auto start = std::chrono::system_clock::now();
        while (m_watcher.isRunning())
        {
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> diff = end-start;
            if (diff.count() > 10) break;
        }
    }

    if (maybeSave())
    {
        writeSettings();
        event->accept(); // pass the event to the default handler
    }
    else
    {
        event->ignore();
    }
}

void AsynchronousGAQtWidget::createActions()
{
    newAct = new QAction(QIcon(":/Images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &AsynchronousGAQtWidget::newFile);

    openAct = new QAction(QIcon(":/Images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/Images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &AsynchronousGAQtWidget::save);

    saveAsAct = new QAction(QIcon(":/Images/save-as.png"), tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document to disk with new name"));
    connect(saveAsAct, &QAction::triggered, this, &AsynchronousGAQtWidget::saveAs);

    editSettingsAct = new QAction(QIcon(":/Images/preferences.png"), tr("Ed&it Settings..."), this);
    editSettingsAct->setShortcuts(QKeySequence::Preferences);
    editSettingsAct->setStatusTip(tr("Open the run specific settings file in an editor"));
    connect(editSettingsAct, &QAction::triggered, this, &AsynchronousGAQtWidget::editSettings);

//    printAct = new QAction(tr("&Print..."), this);
//    printAct->setShortcuts(QKeySequence::Print);
//    printAct->setStatusTip(tr("Print the document"));
//    connect(printAct, &QAction::triggered, this, &AsynchronousGAQtWidget::print);

    exitAct = new QAction(QIcon(":/Images/exit.png"), tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &AsynchronousGAQtWidget::close);

    undoAct = new QAction(QIcon(":/Images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    connect(undoAct, &QAction::triggered, this, &AsynchronousGAQtWidget::undo);

    redoAct = new QAction(QIcon(":/Images/redo.png"), tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, &QAction::triggered, this, &AsynchronousGAQtWidget::redo);

    cutAct = new QAction(QIcon(":/Images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &AsynchronousGAQtWidget::cut);

    copyAct = new QAction(QIcon(":/Images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &AsynchronousGAQtWidget::copy);

    pasteAct = new QAction(QIcon(":/Images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &AsynchronousGAQtWidget::paste);

    fontAct = new QAction(QIcon(":/Images/font.png"), tr("&Font"), this);
    fontAct->setStatusTip(tr("Select the font for the text editor windows"));
    connect(fontAct, &QAction::triggered, this, &AsynchronousGAQtWidget::font);

    aboutAct = new QAction(QIcon(":/Images/icon_design.png"), tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &AsynchronousGAQtWidget::about);

//    aboutQtAct = new QAction(tr("About &Qt"), this);
//    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
//    connect(aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);
//    connect(aboutQtAct, &QAction::triggered, this, &AsynchronousGAQtWidget::aboutQt);

//    boldAct = new QAction(tr("&Bold"), this);
//    boldAct->setCheckable(true);
//    boldAct->setShortcut(QKeySequence::Bold);
//    boldAct->setStatusTip(tr("Make the text bold"));
//    connect(boldAct, &QAction::triggered, this, &AsynchronousGAQtWidget::bold);

//    QFont boldFont = boldAct->font();
//    boldFont.setBold(true);
//    boldAct->setFont(boldFont);

//    italicAct = new QAction(tr("&Italic"), this);
//    italicAct->setCheckable(true);
//    italicAct->setShortcut(QKeySequence::Italic);
//    italicAct->setStatusTip(tr("Make the text italic"));
//    connect(italicAct, &QAction::triggered, this, &AsynchronousGAQtWidget::italic);

//    QFont italicFont = italicAct->font();
//    italicFont.setItalic(true);
//    italicAct->setFont(italicFont);

//    setLineSpacingAct = new QAction(tr("Set &Line Spacing..."), this);
//    setLineSpacingAct->setStatusTip(tr("Change the gap between the lines of a "
//                                       "paragraph"));
//    connect(setLineSpacingAct, &QAction::triggered, this, &AsynchronousGAQtWidget::setLineSpacing);

//    setParagraphSpacingAct = new QAction(tr("Set &Paragraph Spacing..."), this);
//    setParagraphSpacingAct->setStatusTip(tr("Change the gap between paragraphs"));
//    connect(setParagraphSpacingAct, &QAction::triggered,
//            this, &AsynchronousGAQtWidget::setParagraphSpacing);

//    leftAlignAct = new QAction(tr("&Left Align"), this);
//    leftAlignAct->setCheckable(true);
//    leftAlignAct->setShortcut(tr("Ctrl+L"));
//    leftAlignAct->setStatusTip(tr("Left align the selected text"));
//    connect(leftAlignAct, &QAction::triggered, this, &AsynchronousGAQtWidget::leftAlign);

//    rightAlignAct = new QAction(tr("&Right Align"), this);
//    rightAlignAct->setCheckable(true);
//    rightAlignAct->setShortcut(tr("Ctrl+R"));
//    rightAlignAct->setStatusTip(tr("Right align the selected text"));
//    connect(rightAlignAct, &QAction::triggered, this, &AsynchronousGAQtWidget::rightAlign);

//    justifyAct = new QAction(tr("&Justify"), this);
//    justifyAct->setCheckable(true);
//    justifyAct->setShortcut(tr("Ctrl+J"));
//    justifyAct->setStatusTip(tr("Justify the selected text"));
//    connect(justifyAct, &QAction::triggered, this, &AsynchronousGAQtWidget::justify);

//    centerAct = new QAction(tr("&Center"), this);
//    centerAct->setCheckable(true);
//    centerAct->setShortcut(tr("Ctrl+E"));
//    centerAct->setStatusTip(tr("Center the selected text"));
//    connect(centerAct, &QAction::triggered, this, &AsynchronousGAQtWidget::center);

//    alignmentGroup = new QActionGroup(this);
//    alignmentGroup->addAction(leftAlignAct);
//    alignmentGroup->addAction(rightAlignAct);
//    alignmentGroup->addAction(justifyAct);
//    alignmentGroup->addAction(centerAct);
//    leftAlignAct->setChecked(true);
}

void AsynchronousGAQtWidget::createMenus()
{
    fileMenu = ui->menuBar->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(editSettingsAct);
//    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = ui->menuBar->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();

    helpMenu = ui->menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(fontAct);
//    helpMenu->addAction(aboutQtAct);

//    formatMenu = editMenu->addMenu(tr("&Format"));
//    formatMenu->addAction(boldAct);
//    formatMenu->addAction(italicAct);
//    formatMenu->addSeparator()->setText(tr("Alignment"));
//    formatMenu->addAction(leftAlignAct);
//    formatMenu->addAction(rightAlignAct);
//    formatMenu->addAction(justifyAct);
//    formatMenu->addAction(centerAct);
//    formatMenu->addSeparator();
//    formatMenu->addAction(setLineSpacingAct);
//    formatMenu->addAction(setParagraphSpacingAct);
}

void AsynchronousGAQtWidget::newFile()
{
    if (maybeSave() == false) return;
    m_asynchronousGAFileName = "AsynchronousGAQ Settings.xml";
    m_asynchronousGAFileModified = false;
    m_asynchronousGAFileNameValid = false;
    ui->lineEditOutputFolder->setText("");
    ui->lineEditParameterFile->setText("");
    ui->lineEditXMLMasterFile->setText("");
    ui->lineEditStartingPopulationFile->setText("");
    ui->lineEditModelConfigurationFile->setText("");
    ui->lineEditModelPopulationFile->setText("");
    ui->lineEditDriverFile->setText("");
    ui->lineEditWorkingFolder->setText("");
    ui->lineEditMergeXMLFile->setText("");
    ui->lineEditGaitSymExecutable->setText("");
    ui->spinBoxLogLevel->setValue(1);
    ui->spinBoxPort->setValue(8086);
    ui->doubleSpinBoxStartValue->setValue(0);
    ui->doubleSpinBoxStepValue->setValue(1);
    ui->doubleSpinBoxEndValue->setValue(1);
    ui->doubleSpinBoxOutputCycle->setValue(0);
    ui->checkBoxMergeXMLActivate->setChecked(false);
    ui->checkBoxCycleTime->setChecked(false);
    writeSettings(); // makes sure the settings are all up to date
    activateButtons();
}

void AsynchronousGAQtWidget::open()
{
    if (maybeSave() == false) return;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open AsynchronousGA Private Settings File"), m_asynchronousGAFileName, tr("XML Files (*.xml);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        open(fileName);
    }
}

void AsynchronousGAQtWidget::open(const QString &fileName)
{
    m_asynchronousGAFileName = fileName;
    m_asynchronousGAFileModified = false;
    m_asynchronousGAFileNameValid = false;
    QFile file(m_asynchronousGAFileName);
    if (file.open(QFile::ReadOnly) == false)
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("open: Unable to open file (read):\n%1").arg(m_asynchronousGAFileName));
        QMessageBox::warning(this, tr("Open File Error"), QString("open: Unable to open file (read):\n%1").arg(m_asynchronousGAFileName));
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QDomDocument doc;
    if (!doc.setContent(data))
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("open: Unable to parse XML file:\n%1").arg(m_asynchronousGAFileName));
        QMessageBox::warning(this, tr("Open File Error"), QString("open: Unable to parse XML file:\n%1").arg(m_asynchronousGAFileName));
        return;
    }

    QSettings settings("ASL", "AsynchronousGAQt2019");
    settings.setValue("asynchronousGAFileFileName", m_asynchronousGAFileName);
    settings.sync();

    QDomElement docElem = doc.documentElement();
    // qWarning("Element name: %s", qPrintable(docElem.tagName()));
    if (docElem.tagName() != "ASYNCHRONOUSGA") return;
    QDomNode n = docElem.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName() == "SETTINGS")
            {
                ui->lineEditOutputFolder->setText(e.attribute("outputFolder"));
                ui->lineEditParameterFile->setText(e.attribute("parameterFile"));
                ui->lineEditXMLMasterFile->setText(e.attribute("xmlMasterFile"));
                ui->lineEditStartingPopulationFile->setText(e.attribute("startingPopulationFile"));
                ui->lineEditModelConfigurationFile->setText(e.attribute("modelConfigurationFile"));
                ui->lineEditModelPopulationFile->setText(e.attribute("modelPopulationFile"));
                ui->lineEditDriverFile->setText(e.attribute("driverFile"));
                ui->lineEditWorkingFolder->setText(e.attribute("workingFolder"));
                ui->lineEditMergeXMLFile->setText(e.attribute("mergeXMLFile"));
                ui->lineEditGaitSymExecutable->setText(e.attribute("gaitSymExecutable"));
                ui->spinBoxLogLevel->setValue(e.attribute("logLevel").toInt());
                ui->spinBoxPort->setValue(e.attribute("portNumber").toInt());
                ui->doubleSpinBoxStartValue->setValue(e.attribute("startValue").toDouble());
                ui->doubleSpinBoxStepValue->setValue(e.attribute("stepValue").toDouble());
                ui->doubleSpinBoxEndValue->setValue(e.attribute("endValue").toDouble());
                ui->doubleSpinBoxOutputCycle->setValue(e.attribute("outputCycle").toDouble());
                ui->checkBoxMergeXMLActivate->setChecked(QVariant(e.attribute("mergeXMLActivate")).toBool());
                ui->checkBoxCycleTime->setChecked(QVariant(e.attribute("cycle")).toBool());
            }
        }
        n = n.nextSibling();
    }
    if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("'%1' read").arg(m_asynchronousGAFileName));
    ui->statusBar->showMessage(QString("'%1' read").arg(m_asynchronousGAFileName));
    m_asynchronousGAFileModified = false;
    m_asynchronousGAFileNameValid = true;
    activateButtons();
    setCustomTitle();
}

void AsynchronousGAQtWidget::save()
{
    QFile file(m_asynchronousGAFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("save: Unable to open file (write):\n%1").arg(m_asynchronousGAFileName));
        QMessageBox::warning(this, tr("Save File Error"), QString("save: Unable to open file (write):\n%1").arg(m_asynchronousGAFileName));
        return;
    }

    writeSettings(); // makes sure the settings are all up to date

    QDomDocument doc("AsynchronousGA_Settings_Document_0.1");
    QDomProcessingInstruction  pi = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(pi);
    QDomElement root = doc.createElement("ASYNCHRONOUSGA");
    doc.appendChild(root);

    QDomElement dataItemsElement = doc.createElement("SETTINGS");
    root.appendChild(dataItemsElement);

    dataItemsElement.setAttribute("outputFolder", ui->lineEditOutputFolder->text());
    dataItemsElement.setAttribute("parameterFile", ui->lineEditParameterFile->text());
    dataItemsElement.setAttribute("xmlMasterFile", ui->lineEditXMLMasterFile->text());
    dataItemsElement.setAttribute("startingPopulationFile", ui->lineEditStartingPopulationFile->text());
    dataItemsElement.setAttribute("modelConfigurationFile", ui->lineEditModelConfigurationFile->text());
    dataItemsElement.setAttribute("modelPopulationFile", ui->lineEditModelPopulationFile->text());
    dataItemsElement.setAttribute("driverFile", ui->lineEditDriverFile->text());
    dataItemsElement.setAttribute("workingFolder", ui->lineEditWorkingFolder->text());
    dataItemsElement.setAttribute("mergeXMLFile", ui->lineEditMergeXMLFile->text());
    dataItemsElement.setAttribute("gaitSymExecutable", ui->lineEditGaitSymExecutable->text());
    dataItemsElement.setAttribute("logLevel", ui->spinBoxLogLevel->text());
    dataItemsElement.setAttribute("portNumber", ui->spinBoxPort->text());
    dataItemsElement.setAttribute("startValue", ui->doubleSpinBoxStartValue->text());
    dataItemsElement.setAttribute("stepValue", ui->doubleSpinBoxStepValue->text());
    dataItemsElement.setAttribute("endValue", ui->doubleSpinBoxEndValue->text());
    dataItemsElement.setAttribute("outputCycle", ui->doubleSpinBoxOutputCycle->text());
    dataItemsElement.setAttribute("mergeXMLActivate", QString::number(ui->checkBoxMergeXMLActivate->isChecked() ? 1 : 0));
    dataItemsElement.setAttribute("cycle", QString::number(ui->checkBoxCycleTime->isChecked() ? 1 : 0));

    // and now the actual xml doc
    QByteArray xmlData = doc.toByteArray();
    qint64 bytesWritten = file.write(xmlData);
    if (bytesWritten != xmlData.size())
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("save: Unable to write file (write):\n%1").arg(m_asynchronousGAFileName));
        QMessageBox::warning(this, tr("Save File Error"), QString("save: Unable to write file (write):\n%1").arg(m_asynchronousGAFileName));
    }
    file.close();
    if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("'%1' written").arg(m_asynchronousGAFileName));
    ui->statusBar->showMessage(QString("'%1' written").arg(m_asynchronousGAFileName));
    m_asynchronousGAFileModified = false;
    m_asynchronousGAFileNameValid = true;
    activateButtons();
    setCustomTitle();
}

void AsynchronousGAQtWidget::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Run Settings File"), m_asynchronousGAFileName, tr("XML Files (*.xml);;Any File (*.* *)"));
    if (!fileName.isEmpty())
    {
        m_asynchronousGAFileName = fileName;
        save();
    }
}

void AsynchronousGAQtWidget::editSettings()
{
    TextEditDialog textEditDialog(this);
    QFile editFile(m_asynchronousGAFileName);
    if (editFile.open(QFile::ReadOnly) == false)
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("editSettings: Unable to open file (read):\n%1").arg(m_asynchronousGAFileName));
        QMessageBox::warning(this, tr("Open File Error"), QString("editSettings: Unable to open file (read):\n%1").arg(m_asynchronousGAFileName));
        return;
    }
    QByteArray editFileData = editFile.readAll();
    editFile.close();
    QString editFileText = QString::fromUtf8(editFileData);

    textEditDialog.useXMLSyntaxHighlighter();
    textEditDialog.setEditorText(editFileText);

    int status = textEditDialog.exec();

    if (status == QDialog::Accepted) // write the new settings
    {
        if (editFile.open(QFile::WriteOnly) == false)
        {
            if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("editSettings: Unable to open file (write):\n%1").arg(m_asynchronousGAFileName));
            QMessageBox::warning(this, tr("Open File Error"), QString("editSettings: Unable to open file (write):\n%1").arg(m_asynchronousGAFileName));
            return;
        }
        editFileData = textEditDialog.editorText().toUtf8();
        editFile.write(editFileData);
        editFile.close();
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("'%1' written").arg(m_asynchronousGAFileName));
        ui->statusBar->showMessage(QString("'%1' written").arg(m_asynchronousGAFileName));
        open(m_asynchronousGAFileName);
    }
    else
    {
        if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("'%1' not changed").arg(m_asynchronousGAFileName));
        ui->statusBar->showMessage(QString("'%1' not changed").arg(m_asynchronousGAFileName));
    }
}

void AsynchronousGAQtWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}

void AsynchronousGAQtWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_basicTimer.timerId()) { activateButtons(); }
    else { QMainWindow::timerEvent(event); }
}

void AsynchronousGAQtWidget::undo()
{
    QWidget *fw = qApp->focusWidget();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(fw);
    if (lineEdit) { lineEdit->undo(); return; }
    QPlainTextEdit *plainTextEdit = dynamic_cast<QPlainTextEdit *>(fw);
    if (plainTextEdit) { plainTextEdit->undo(); return; }
}

void AsynchronousGAQtWidget::redo()
{
    QWidget *fw = qApp->focusWidget();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(fw);
    if (lineEdit) { lineEdit->redo(); return; }
    QPlainTextEdit *plainTextEdit = dynamic_cast<QPlainTextEdit *>(fw);
    if (plainTextEdit) { plainTextEdit->redo(); return; }
}

void AsynchronousGAQtWidget::cut()
{
    QWidget *fw = qApp->focusWidget();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(fw);
    if (lineEdit) { lineEdit->cut(); return; }
    QPlainTextEdit *plainTextEdit = dynamic_cast<QPlainTextEdit *>(fw);
    if (plainTextEdit) { plainTextEdit->cut(); return; }
}

void AsynchronousGAQtWidget::copy()
{
    QWidget *fw = qApp->focusWidget();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(fw);
    if (lineEdit) { lineEdit->copy(); return; }
    QPlainTextEdit *plainTextEdit = dynamic_cast<QPlainTextEdit *>(fw);
    if (plainTextEdit) { plainTextEdit->copy(); return; }
}

void AsynchronousGAQtWidget::paste()
{
    QWidget *fw = qApp->focusWidget();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit *>(fw);
    if (lineEdit) { lineEdit->paste(); return; }
    QPlainTextEdit *plainTextEdit = dynamic_cast<QPlainTextEdit *>(fw);
    if (plainTextEdit) { plainTextEdit->paste(); return; }
}

void AsynchronousGAQtWidget::font()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_editorFont, this);
    if (ok)
    {
        m_editorFont = font;
        setEditorFonts();
        writeSettings();
    }
}

void AsynchronousGAQtWidget::about()
{
    QMessageBox msgBox;
    msgBox.setText(tr("<b>AsynchronousGAQt</b> Genetic Algorithm Server"));
    msgBox.setInformativeText(tr("This application coordinates optimisation using remote clients to do the hard work.\nCopyright William Sellers 2018.\nReleased under GPL v3."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIconPixmap(QPixmap(":/Images/icon_design.png").scaledToWidth(128));
    int ret = msgBox.exec();
    switch (ret)
    {
    case QMessageBox::Ok:
        // Ok was clicked
        break;
    default:
        // should never be reached
        break;
    }
}

bool AsynchronousGAQtWidget::maybeSave()
{
    if (m_asynchronousGAFileModified)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"), tr("'%1' has been modified.\nDo you want to save your changes?").arg(m_asynchronousGAFileName),
                                   QMessageBox::Save | QMessageBox::Discard| QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel) return false;
        if (ret == QMessageBox::Save)
        {
            if (m_asynchronousGAFileNameValid) save();
            else saveAs();
        }
    }
    return true;
}

void AsynchronousGAQtWidget::menuRequestPath(QPoint pos)
{
    // this should always come from a QLineEdit
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    if (lineEdit == nullptr) return;

    QMenu *menu = lineEdit->createStandardContextMenu();
    menu->addSeparator();
    menu->addAction(tr("Edit File..."));


    QPoint gp = lineEdit->mapToGlobal(pos);

    QAction *action = menu->exec(gp);
    if (action)
    {
        if (action->text() == tr("Edit File..."))
        {
            TextEditDialog textEditDialog(this);
            QString fileName = lineEdit->text();
            if (fileName.endsWith(".xml", Qt::CaseInsensitive) || fileName.endsWith(".gaitsym", Qt::CaseInsensitive)) textEditDialog.useXMLSyntaxHighlighter();
            QFile editFile(fileName);
            if (editFile.open(QFile::ReadOnly) == false)
            {
                if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("menuRequestPath: Unable to open file (read):\n%1").arg(fileName));
                QMessageBox::warning(this, tr("Open File Error"), QString("menuRequestPath: Unable to open file (read):\n%1").arg(fileName));
                return;
            }
            QByteArray editFileData = editFile.readAll();
            editFile.close();
            QString editFileText = QString::fromUtf8(editFileData);

            textEditDialog.setEditorText(editFileText);

            int status = textEditDialog.exec();

            if (status == QDialog::Accepted) // write the new settings
            {
                if (editFile.open(QFile::WriteOnly) == false)
                {
                    if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("menuRequestPath: Unable to open file (write):\n%1").arg(fileName));
                    QMessageBox::warning(this, tr("Open File Error"), QString("menuRequestPath: Unable to open file (write):\n%1").arg(fileName));
                    return;
                }
                editFileData = textEditDialog.editorText().toUtf8();
                editFile.write(editFileData);
                editFile.close();
                if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("'%1' written").arg(lineEdit->text()));
                ui->statusBar->showMessage(QString("'%1' written").arg(lineEdit->text()));
            }
            else
            {
                if (ui->spinBoxLogLevel->value() > 0) appendProgress(QString("'%1' not changed").arg(lineEdit->text()));
                ui->statusBar->showMessage(QString("'%1' not changed").arg(lineEdit->text()));
            }
        }
    }
    delete menu;
}

void AsynchronousGAQtWidget::setEditorFonts()
{
//    QRegularExpression regExp("lineEditOutputFolder|"
//                              "lineEditXMLMasterFile|"
//                              "lineEditStartingPopulationFile|"
//                              "lineEditParameterFile|"
//                              "lineEditModelConfigurationFile|"
//                              "lineEditModelPopulationFile|"
//                              "lineEditDriverFile|"
//                              "lineEditMergeXMLFile|"
//                              "lineEditWorkingFolder|"
//                              "lineEditResultNumber|"
//                              "lineEditGenerationsPerSecond|"
//                              "lineEditBestScore|"
//                              "lineEditCurrentLoopValue|"
//                              "lineEditCurrentLoopCount|"
//                              "plainTextEditLog");
//    QList<QWidget *> listQWidget = this->findChildren<QWidget *>(regExp, Qt::FindChildrenRecursively);
//    for (QList<QWidget *>::iterator it = listQWidget.begin(); it != listQWidget.end(); it++) (*it)->setFont(m_editorFont);

//    QList<QLineEdit *> listQLineEdit = this->findChildren<QLineEdit *>(QRegularExpression(".*", QRegularExpression::CaseInsensitiveOption), Qt::FindChildrenRecursively);
//    for (QList<QLineEdit *>::iterator it = listQLineEdit.begin(); it != listQLineEdit.end(); it++) (*it)->setFont(m_editorFont);

    QList<QPlainTextEdit *> listQPlainTextEdit = this->findChildren<QPlainTextEdit *>(QRegularExpression(".*", QRegularExpression::CaseInsensitiveOption), Qt::FindChildrenRecursively);
    for (QList<QPlainTextEdit *>::iterator it = listQPlainTextEdit.begin(); it != listQPlainTextEdit.end(); it++) (*it)->setFont(m_editorFont);
}

void AsynchronousGAQtWidget::setCustomTitle()
{
    QFileInfo fileInfo(m_asynchronousGAFileName);
    QString windowTitle = fileInfo.absoluteFilePath();
    if (m_asynchronousGAFileModified) windowTitle += "*";
    if (!m_asynchronousGAFileNameValid) windowTitle += "+";
    this->setWindowTitle(windowTitle);
}
