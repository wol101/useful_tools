#-------------------------------------------------
#
# Project created by QtCreator 2013-01-12T11:55:29
#
#-------------------------------------------------

cache()

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FuzzySyncQT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    copylistobject.cpp \
    filesyncpairobject.cpp \
    ErrorReporterDialog.cpp

HEADERS  += mainwindow.h \
    copylistobject.h \
    filesyncpairobject.h \
    ErrorReporterDialog.h

FORMS    += mainwindow.ui \
    ErrorReporterDialog.ui

ICON = Icon.icns
