#include <QPlainTextEdit>

#include "ErrorReporterDialog.h"
#include "ui_ErrorReporterDialog.h"

ErrorReporterDialog::ErrorReporterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorReporterDialog)
{
    ui->setupUi(this);
}

ErrorReporterDialog::~ErrorReporterDialog()
{
    delete ui;
}

bool ErrorReporterDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Close) // we want to hide the dialog not close it
    {
        event->ignore();
        hide();
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void ErrorReporterDialog::AppendText(QString text)
{
    ui->plainTextEdit->appendPlainText(text);
}
