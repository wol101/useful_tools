#-------------------------------------------------
#
# Project created by QtCreator 2019-12-29T17:06:22
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsynchronousGA2019ASIO
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

win32 {
    INCLUDEPATH += ../ \
        ../asio-1.18.2/include \
        ../exprtk \
        ../pystring \
        ../rapidxml-1.13 \
        ../windows_unistd
    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA USE_ASIO \
        USE_UNIX_ERRORS _USE_MATH_DEFINES \
        _CRT_SECURE_NO_WARNINGS \
        WINVER=0x0A00 _WIN32_WINNT=0x0A00
    LIBS += -lWs2_32 -lWinmm
    QMAKE_CXXFLAGS += -bigobj

    RC_FILE = Images/myapp.rc
}

else:unix {
    INCLUDEPATH += ../ \
        ../asio-1.18.2/include \
        ../exprtk \
        ../pystring \
        ../rapidxml-1.13
    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA USE_ASIO \
        HAS_SOCKLEN_T

    QMAKE_CXXFLAGS +=
}

OBJECTS_DIR = obj
CONFIG += no_batch # this gets around a bug in Visual Studio with the object_parallel_to_source option
CONFIG += object_parallel_to_source # this is important to stop obj files overwriting each other
CONFIG += c++17 # we are using a lot of new C++ in this

SOURCES += \
    ../DataFile.cpp \
    ../GAASIO.cpp \
    ../Genome.cpp \
    ../Mating.cpp \
    ../MD5.cpp \
    ../MergeExpressionParser.cpp \
    ../MergeXML.cpp \
    ../Population.cpp \
    ../Preferences.cpp \
    ../Random.cpp \
    ../ServerASIO.cpp \
    ../Statistics.cpp \
    ../XMLContainer.cpp \
    ../XMLContainerList.cpp \
    ../XMLConverter.cpp \
    ../pystring/pystring.cpp \
    AsynchronousGAQtWidget.cpp \
    BasicXMLSyntaxHighlighter.cpp \
    TextEditDialog.cpp \
    Utilities.cpp \
    main.cpp

HEADERS += \
    ../DataFile.h \
    ../GAASIO.h \
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
    ../ServerASIO.h \
    ../Statistics.h \
    ../XMLContainer.h \
    ../XMLContainerList.h \
    ../XMLConverter.h \
    ../avl_array/avl_array.hpp \
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
