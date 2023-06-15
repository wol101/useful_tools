#include "AsynchronousGAQtWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AsynchronousGAQtWidget w;
    w.show();

    return a.exec();
}
