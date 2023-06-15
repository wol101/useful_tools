#include "AsynchronousGAQtWidget.h"

#include "Preferences.h"
#include "Random.h"
#include "TCP.h"
#include "UDP.h"
#include "UDTWrapper.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
#ifdef USE_TCP
    TCPUpDown tcpUpDown;
    if (tcpUpDown.status())
    {
        QString errorMessage = QString("This application failed to initialise TCPIP\nApplication will abort.");
        QMessageBox::critical(nullptr, "GaitSym2019", errorMessage);
        exit(EXIT_FAILURE);
    }
#endif
#ifdef USE_UDP
    UDPUpDown udpUpDown;
    if (udpUpDown.status())
    {
        QString errorMessage = QString("This application failed to initialise TCPIP\nApplication will abort.");
        QMessageBox::critical(nullptr, "GaitSym2019", errorMessage);
        exit(EXIT_FAILURE);
    }
#endif
#ifdef USE_UDT
    UDTUpDown udtUpDown;
    if (udtUpDown.status())
    {
        QString errorMessage = QString("This application failed to initialise UDT\nApplication will abort.");
        QMessageBox::critical(nullptr, "GaitSym2019", errorMessage);
        exit(EXIT_FAILURE);
    }
#endif

    std::unique_ptr<Preferences> preferences = std::make_unique<Preferences>();
    g_prefs = preferences.get();
    std::unique_ptr<Random> random = std::make_unique<Random>();
    g_random = random.get();

    QApplication a(argc, argv);
    AsynchronousGAQtWidget w;
    w.show();

    return a.exec();
}
