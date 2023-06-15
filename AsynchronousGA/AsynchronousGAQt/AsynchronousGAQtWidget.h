#ifndef ASYNCHRONOUSGAQTWIDGET_H
#define ASYNCHRONOUSGAQTWIDGET_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <QCloseEvent>
#include <QTimer>
#include <QDateTime>

class Simulation;

namespace AsynchronousGA {
class GAMain;
}

namespace Ui {
class AsynchronousGAQtWidget;
}

class AsynchronousGAQtWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit AsynchronousGAQtWidget(QWidget *parent = 0);
    ~AsynchronousGAQtWidget();

    void activateButtons();

public slots:
    void pushButtonParameterFileClicked();
    void pushButtonStartingPopulationFileClicked();
    void pushButtonXMLMasterFileClicked();
    void pushButtonOutputFolderClicked();
    void pushButtonModelConfigurationFileClicked();
    void pushButtonModelPopulationFileClicked();
    void pushButtonDriverFileClicked();
    void pushButtonWorkingFolderClicked();
    void pushButtonMergeXMLFileClicked();
    void pushButtonStartClicked();
    void pushButtonStopClicked();
    void checkBoxMergeXMLActivateClicked();
    void handleFinished();
    void lineEditOutputFolderTextChanged(const QString &text);
    void lineEditParameterFileTextChanged(const QString &text);
    void lineEditXMLMasterFileTextChanged(const QString &text);
    void lineEditStartingPopulationFileTextChanged(const QString &text);
    void lineEditModelConfigurationFileTextChanged(const QString &text);
    void lineEditModelPopulationFileTextChanged(const QString &text);
    void lineEditDriverFileTextChanged(const QString &text);
    void lineEditMergeXMLFileTextChanged(const QString &text);
    void lineEditWorkingFolderTextChanged(const QString &text);
    void checkBoxStateChanged(int state);
    void spinBoxValueChanged(const QString &text);
    void appendProgress(const QString &text);
    void setProgressValue(int value);
    void setResultNumber(int number);
    void setBestScore(double value);
    bool tryToStopGA();
    int logLevel();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void editSettings();
//    void print();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void font();
//    void bold();
//    void italic();
//    void leftAlign();
//    void rightAlign();
//    void justify();
//    void center();
//    void setLineSpacing();
//    void setParagraphSpacing();
    void about();
//    void aboutQt();

    void runMergeXML();
    void menuRequestPath(QPoint pos);

private:
    void closeEvent(QCloseEvent *event);

    Ui::AsynchronousGAQtWidget *ui;
    QFutureWatcher<int> m_watcher;
    AsynchronousGA::GAMain *m_ga;
    bool m_tryToStopGA;
    QTimer *m_mergeXMLTimer;
    int m_runMergeXML;
    double m_currentLoopValue;
    int m_currentLoopCount;
    Simulation *m_simulation;
    QDateTime m_lastResultsTime;
    int m_lastResultsNumber;

    bool m_asynchronousGAFileModified;
    QString m_asynchronousGAFileFileName;
    bool m_asynchronousGAFileFileNameValid;

#ifdef _WIN32
    std::wstring m_outputFolder;
    std::wstring m_parameterFile;
    std::wstring m_xmlMasterFile;
    std::wstring m_startingPopulationFile;
#else
    std::string m_outputFolder;
    std::string m_parameterFile;
    std::string m_xmlMasterFile;
    std::string m_startingPopulationFile;
#endif

    void createActions();
    void createMenus();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void runGA();
    void runGaitSym();
    void open(const QString &fileName);
    void setEditorFonts();

    QMenu *fileMenu;
    QMenu *editMenu;
//    QMenu *formatMenu;
    QMenu *helpMenu;
//    QActionGroup *alignmentGroup;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *editSettingsAct;
//    QAction *printAct;
    QAction *exitAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *fontAct;
//    QAction *boldAct;
//    QAction *italicAct;
//    QAction *leftAlignAct;
//    QAction *rightAlignAct;
//    QAction *justifyAct;
//    QAction *centerAct;
//    QAction *setLineSpacingAct;
//    QAction *setParagraphSpacingAct;
    QAction *aboutAct;
//    QAction *aboutQtAct;

    QFont m_editorFont;
};

#endif // ASYNCHRONOUSGAQTWIDGET_H
