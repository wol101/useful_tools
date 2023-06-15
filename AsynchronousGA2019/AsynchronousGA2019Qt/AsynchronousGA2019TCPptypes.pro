#-------------------------------------------------
#
# Project created by QtCreator 2019-12-29T17:06:22
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsynchronousGA2019TCPptypes
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    INCLUDEPATH += ../ \
        ../enet-1.3.14/include \
        ../exprtk \
        ../pystring \
        ../rapidxml-1.13 \
        ../ptypes-2.1.1/include \
        ../windows_unistd
    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA USE_TCP_PTYPES \
    USE_UNIX_ERRORS _USE_MATH_DEFINES \
    _CRT_SECURE_NO_WARNINGS

    LIBS += -lWs2_32 -lWinmm
    QMAKE_CXXFLAGS += -bigobj

    RC_FILE = Images/myapp.rc
}

else:unix {
    INCLUDEPATH += ../ \
        ../enet-1.3.14/include \
        ../exprtk \
        ../pystring \
        ../rapidxml-1.13 \
        ../ptypes-2.1.1/include
    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA USE_TCP_PTYPES \
    HAS_SOCKLEN_T \
    exprtk_disable_enhanced_features

    QMAKE_CXXFLAGS +=
    QMAKE_CXXFLAGS_RELEASE += -O3 -ffast-math
}

OBJECTS_DIR = obj
CONFIG += no_batch # this gets around a bug in Visual Studio with the object_parallel_to_source option
CONFIG += object_parallel_to_source # this is important to stop obj files overwriting each other
CONFIG += c++14 # there are a few c++14 specifics - mostly string_literals

SOURCES += \
    ../DataFile.cpp \
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
    ../ptypes-2.1.1/src/pasync.cxx \
    ../ptypes-2.1.1/src/patomic.cxx \
    ../ptypes-2.1.1/src/pcomponent.cxx \
    ../ptypes-2.1.1/src/pcset.cxx \
    ../ptypes-2.1.1/src/pcsetdbg.cxx \
    ../ptypes-2.1.1/src/pexcept.cxx \
    ../ptypes-2.1.1/src/pfatal.cxx \
    ../ptypes-2.1.1/src/pfdxstm.cxx \
    ../ptypes-2.1.1/src/pinfile.cxx \
    ../ptypes-2.1.1/src/pinfilter.cxx \
    ../ptypes-2.1.1/src/pinmem.cxx \
    ../ptypes-2.1.1/src/pinstm.cxx \
    ../ptypes-2.1.1/src/pintee.cxx \
    ../ptypes-2.1.1/src/piobase.cxx \
    ../ptypes-2.1.1/src/pipbase.cxx \
    ../ptypes-2.1.1/src/pipmsg.cxx \
    ../ptypes-2.1.1/src/pipmsgsv.cxx \
    ../ptypes-2.1.1/src/pipstm.cxx \
    ../ptypes-2.1.1/src/pipstmsv.cxx \
    ../ptypes-2.1.1/src/pipsvbase.cxx \
    ../ptypes-2.1.1/src/pmd5.cxx \
    ../ptypes-2.1.1/src/pmem.cxx \
    ../ptypes-2.1.1/src/pmsgq.cxx \
    ../ptypes-2.1.1/src/pmtxtable.cxx \
    ../ptypes-2.1.1/src/pnpipe.cxx \
    ../ptypes-2.1.1/src/pnpserver.cxx \
    ../ptypes-2.1.1/src/pobjlist.cxx \
    ../ptypes-2.1.1/src/poutfile.cxx \
    ../ptypes-2.1.1/src/poutfilter.cxx \
    ../ptypes-2.1.1/src/poutmem.cxx \
    ../ptypes-2.1.1/src/poutstm.cxx \
    ../ptypes-2.1.1/src/ppipe.cxx \
    ../ptypes-2.1.1/src/ppodlist.cxx \
    ../ptypes-2.1.1/src/pputf.cxx \
    ../ptypes-2.1.1/src/prwlock.cxx \
    ../ptypes-2.1.1/src/psemaphore.cxx \
    ../ptypes-2.1.1/src/pstdio.cxx \
    ../ptypes-2.1.1/src/pstrcase.cxx \
    ../ptypes-2.1.1/src/pstrconv.cxx \
    ../ptypes-2.1.1/src/pstring.cxx \
    ../ptypes-2.1.1/src/pstrlist.cxx \
    ../ptypes-2.1.1/src/pstrmanip.cxx \
    ../ptypes-2.1.1/src/pstrtoi.cxx \
    ../ptypes-2.1.1/src/pstrutils.cxx \
    ../ptypes-2.1.1/src/ptextmap.cxx \
    ../ptypes-2.1.1/src/pthread.cxx \
    ../ptypes-2.1.1/src/ptime.cxx \
    ../ptypes-2.1.1/src/ptimedsem.cxx \
    ../ptypes-2.1.1/src/ptrigger.cxx \
    ../ptypes-2.1.1/src/ptypes_test.cxx \
    ../ptypes-2.1.1/src/punit.cxx \
    ../ptypes-2.1.1/src/punknown.cxx \
    ../ptypes-2.1.1/src/pvariant.cxx \
    ../ptypes-2.1.1/src/pversion.cxx \
    ../pystring/pystring.cpp \
    AsynchronousGAQtWidget.cpp \
    BasicXMLSyntaxHighlighter.cpp \
    TextEditDialog.cpp \
    Utilities.cpp \
    main.cpp

HEADERS += \
    ../DataFile.h \
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
    ../TCPIPMessage.h \
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
    ../ptypes-2.1.1/include/pasync.h \
    ../ptypes-2.1.1/include/pinet.h \
    ../ptypes-2.1.1/include/pport.h \
    ../ptypes-2.1.1/include/pstreams.h \
    ../ptypes-2.1.1/include/ptime.h \
    ../ptypes-2.1.1/include/ptypes.h \
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
