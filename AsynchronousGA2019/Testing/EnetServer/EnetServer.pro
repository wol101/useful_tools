QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

INCLUDEPATH += ../../enet-1.3.14/include
LIBS += -lWs2_32 -lWinmm
QMAKE_CXXFLAGS += -std:c++14 -bigobj
DEFINES += _CRT_SECURE_NO_WARNINGS

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../enet-1.3.14/callbacks.c \
        ../../enet-1.3.14/compress.c \
        ../../enet-1.3.14/host.c \
        ../../enet-1.3.14/list.c \
        ../../enet-1.3.14/packet.c \
        ../../enet-1.3.14/peer.c \
        ../../enet-1.3.14/protocol.c \
        ../../enet-1.3.14/unix.c \
        ../../enet-1.3.14/win32.c \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../enet-1.3.14/include/enet/callbacks.h \
    ../../enet-1.3.14/include/enet/enet.h \
    ../../enet-1.3.14/include/enet/list.h \
    ../../enet-1.3.14/include/enet/protocol.h \
    ../../enet-1.3.14/include/enet/time.h \
    ../../enet-1.3.14/include/enet/types.h \
    ../../enet-1.3.14/include/enet/unix.h \
    ../../enet-1.3.14/include/enet/utility.h \
    ../../enet-1.3.14/include/enet/win32.h
