#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QVector>
#include <QImage>
#include <QColor>

#include <vector>

class QTimer;
class QFileInfo;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent (QPaintEvent *) Q_DECL_OVERRIDE;

private slots:
    void timedAction();

private:
    Ui::MainWindow *ui;

    QTimer *m_timer = nullptr;
    QStringList m_imageFiles;
    std::vector<size_t> m_permutation;
    size_t m_currentIndex = 0;
    QImage m_image;
    QColor m_padColor;

    static void readFileTree(const QString &rootDir, const QStringList &extensionList, QStringList *filePaths);
    static void readFileTree(const QFileInfo &rootInfo, const QStringList &extensionList, QStringList *filePaths);
    static std::vector<size_t> generateRandomOrder(size_t count);

};
#endif // MAINWINDOW_H
