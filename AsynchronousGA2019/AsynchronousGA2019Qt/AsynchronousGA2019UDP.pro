#-------------------------------------------------
#
# Project created by QtCreator 2019-12-29T17:06:22
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsynchronousGA2019UDP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

OBJECTS_DIR = obj
CONFIG += no_batch # this gets around a bug in Visual Studio with the object_parallel_to_source option
CONFIG += object_parallel_to_source # this is important to stop obj files overwriting each other
CONFIG += c++17

win32 {
    INCLUDEPATH += ../ \
        ../enet-1.3.14/include \
        ../exprtk \
        ../pystring \
        ../rapidxml-1.13 \
        ../windows_unistd
    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA USE_UDP \
        USE_UNIX_ERRORS _USE_MATH_DEFINES \
        _CRT_SECURE_NO_WARNINGS NEED_BCOPY

    LIBS += -lWs2_32 -lWinmm
    QMAKE_CXXFLAGS += -bigobj

    RC_FILE = Images/myapp.rc
}

else:unix {
    INCLUDEPATH += ../ \
        ../enet-1.3.14/include \
        ../exprtk \
        ../pystring \
        ../rapidxml-1.13
    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA USE_UDP \
        HAS_SOCKLEN_T

    QMAKE_CXXFLAGS +=
}


SOURCES += \
    ../DataFile.cpp \
    ../FEC.cpp \
    ../GAENET.cpp \
    ../GATCP.cpp \
    ../GATCPptypes.cpp \
    ../GAUDP.cpp \
    ../Genome.cpp \
    ../Mating.cpp \
    ../MD5.cpp \
    ../MergeExpressionParser.cpp \
    ../MergeXML.cpp \
    ../Population.cpp \
    ../Preferences.cpp \
    ../Random.cpp \
    ../Statistics.cpp \
    ../TCP.cpp \
    ../ThreadedUDP.cpp \
    ../UDP.cpp \
    ../WindowsGlue.cpp \
    ../XMLContainer.cpp \
    ../XMLContainerList.cpp \
    ../XMLConverter.cpp \
    ../enet-1.3.14/callbacks.c \
    ../enet-1.3.14/compress.c \
    ../enet-1.3.14/host.c \
    ../enet-1.3.14/list.c \
    ../enet-1.3.14/packet.c \
    ../enet-1.3.14/peer.c \
    ../enet-1.3.14/protocol.c \
    ../enet-1.3.14/unix.c \
    ../enet-1.3.14/win32.c \
    ../pystring/pystring.cpp \
    AsynchronousGAQtWidget.cpp \
    BasicXMLSyntaxHighlighter.cpp \
    TextEditDialog.cpp \
    Utilities.cpp \
    main.cpp

HEADERS += \
    ../DataFile.h \
    ../FEC.h \
    ../GAENET.h \
    ../GATCP.h \
    ../GATCPptypes.h \
    ../GAUDP.h \
    ../Genome.h \
    ../MPIStuff.h \
    ../Mating.h \
    ../MD5.h \
    ../MergeExpressionParser.h \
    ../MergeUtil.h \
    ../MergeXML.h \
    ../Population.h \
    ../Preferences.h \
    ../Random.h \
    ../Statistics.h \
    ../TCP.h \
    ../TCPIPMessage.h \
    ../ThreadedUDP.h \
    ../UDP.h \
    ../WindowsGlue.h \
    ../XMLContainer.h \
    ../XMLContainerList.h \
    ../XMLConverter.h \
    ../avl_array/avl_array.hpp \
    ../enet-1.3.14/include/enet/callbacks.h \
    ../enet-1.3.14/include/enet/enet.h \
    ../enet-1.3.14/include/enet/list.h \
    ../enet-1.3.14/include/enet/protocol.h \
    ../enet-1.3.14/include/enet/time.h \
    ../enet-1.3.14/include/enet/types.h \
    ../enet-1.3.14/include/enet/unix.h \
    ../enet-1.3.14/include/enet/utility.h \
    ../enet-1.3.14/include/enet/win32.h \
    ../exprtk/exprtk.hpp \
    ../pystring/pystring.h \
    ../rapidxml-1.13/rapidxml.hpp \
    ../windows_unistd/unistd.h \
    AsynchronousGAQtWidget.h \
    BasicXMLSyntaxHighlighter.h \
    TextEditDialog.h \
    Utilities.h

FORMS += \
    AsynchronousGAQtWidget.ui \
    TextEditDialog.ui

RESOURCES += \
    Resources.qrc
