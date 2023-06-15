#ifndef ERRORREPORTERDIALOG_H
#define ERRORREPORTERDIALOG_H

#include <QDialog>

namespace Ui {
class ErrorReporterDialog;
}

class ErrorReporterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorReporterDialog(QWidget *parent = 0);
    ~ErrorReporterDialog();

    bool eventFilter(QObject *obj, QEvent *event);
    void AppendText(QString text);

private:
    Ui::ErrorReporterDialog *ui;
};

#endif // ERRORREPORTERDIALOG_H
