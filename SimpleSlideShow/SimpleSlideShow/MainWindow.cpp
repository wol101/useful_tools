#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QImageReader>
#include <QDirIterator>

#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("wol101", "SimpleSlideShow");
    QString photosPath = settings.value("photosPath", "").toString();
    if (photosPath.size() == 0 || QFileInfo(photosPath).isDir() == false)
    {
        photosPath = QFileDialog::getExistingDirectory(this, "Where are the photos located?", photosPath);
        settings.setValue("photosPath", photosPath);
    }
    qDebug() << "photosPath " << photosPath;

    QString colorString = settings.value("padColor", "#000000").toString();
    qDebug() << "colorString " << colorString;
    m_padColor.setNamedColor(colorString);

    ui->menubar->hide();
    ui->statusbar->hide();

    QStringList extensionList = settings.value("imageExtensions", QStringList{".jpg", ".jpeg", ".png"}).toStringList();
    for (auto &&extension : extensionList) qDebug() << extension;
    m_imageFiles.reserve(1000);
    readFileTree(photosPath, extensionList, &m_imageFiles);
    m_permutation = generateRandomOrder(m_imageFiles.size());

    settings.sync();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::timedAction);
    std::chrono::milliseconds ms = static_cast<std::chrono::milliseconds>(settings.value("timeOutSeconds", 10).toInt() * uint64_t(1000));
    qDebug() << "ms " << ms.count();
    timedAction();
    m_timer->start(ms);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent (QPaintEvent *)
{
    QPainter painter(this);
    QRect source = m_image.rect();
    QRect target = geometry();
    painter.drawImage(target, m_image, source);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        close();
    }
}

void MainWindow::timedAction()
{
    QMainWindow::showFullScreen();
    if (m_currentIndex >= size_t(m_imageFiles.size())) m_currentIndex = 0;
    if (m_imageFiles.size() == 0) return;
    QImageReader imgReader(m_imageFiles[m_permutation[m_currentIndex]]);
    imgReader.setAutoTransform(true);
    QImage image = imgReader.read();
    if (image.isNull() == false)
    {
        m_image = QImage(width(), height(), image.format());
        m_image.fill(m_padColor);
        QPainter painter{&m_image};
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        double targetAspect = double(width()) / double(height());
        double sourceAspect =  double(image.width()) / double(image.height());
        QRect sourceRect = image.rect();
        if (targetAspect > sourceAspect) // source image tall and thin
        {
            int newWidth = (image.width() * height()) / image.height();
            QRect targetRect((width() - newWidth) / 2, 0, newWidth, height());
            painter.drawImage(targetRect, image, sourceRect);
        }
        else // source image short and wide
        {
            int newHeight = (image.height() * width()) / image.width();
            QRect targetRect(0, (height() - newHeight) / 2, width(), newHeight);
            painter.drawImage(targetRect, image, sourceRect);
        }
        update();
    }
    m_currentIndex++;
}

std::vector<size_t> MainWindow::generateRandomOrder(size_t count)
{
    std::vector<size_t> v;
    v.reserve(count);
    for (size_t i = 0; i < count; i++) v.push_back(i);
    std::random_device rd; // this can be slow but should be random so very useful as a seed
    std::mt19937 g(rd());
    std::shuffle(v.begin(), v.end(), g);
    return v;
}

// this version uses QDirIterator and should be quicker
void MainWindow::readFileTree(const QString &rootDir, const QStringList &extensionList, QStringList *filePaths)
{
    QStringList extensionListStandardised;
    for (auto &&extension : extensionList)
    {
        if (extension.startsWith(".")) extensionListStandardised.push_back(extension.toLower());
        else extensionListStandardised.push_back(QString(".") + extension.toLower());
    }
#ifdef Q_OS_ANDROID
    QDirIterator dirIterator(rootDir);
#else
    QDirIterator dirIterator(rootDir, QDirIterator::Subdirectories);
#endif
    while (dirIterator.hasNext())
    {
        QString path = dirIterator.next();
        // qDebug() << path;
        bool fileMatch = false;
        if (extensionListStandardised.size() == 0)
        {
            fileMatch = true;
        }
        else
        {
            QString lowerPath = path.toLower();
            for (auto &&extension : extensionListStandardised)
            {
                if (lowerPath.endsWith(extension))
                {
                    fileMatch = true;
                    break;
                }
            }
        }
        if (fileMatch && QFileInfo(path).isFile()) filePaths->push_back(path);
    }
}


// this works on Windows but not on Android 9
void MainWindow::readFileTree(const QFileInfo &rootInfo, const QStringList &extensionList, QStringList *filePaths)
{
    if (rootInfo.isDir() == false) return;
    QDir::Filters filter = QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot;
    QDir dir(rootInfo.filesystemAbsoluteFilePath());
    QFileInfoList infoList = dir.entryInfoList(filter, QDir::Name);
    for (auto &&info : infoList)
    {
        while (true)
        {
            // we recurse for folders but not for symbolic links to folders
            if (info.isSymLink() == false && info.isDir() == true)
            {
#ifdef Q_OS_ANDROID
                // readFileTree(info, extensionList, filePaths); // this does not seem to work on Android
#else
                readFileTree(info, extensionList, filePaths); // this does not seem to work on Android
#endif
                break;
            }
            if (info.isFile())
            {
                if (extensionList.size() == 0)
                {
                    filePaths->append(info.absoluteFilePath());
                    break;
                }
                for (auto &&extension : extensionList)
                {
                    if (info.fileName().toLower().endsWith(extension))
                    {
                        filePaths->append(info.absoluteFilePath());
                        break;
                    }
                }
                break;
            }
            break;
        }
    }
}
