# -------------------------------------------------
# Project created by QtCreator 2010-03-31T12:38:29
# -------------------------------------------------
QT -= gui
TARGET = BatchIsosurface
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
QMAKE_MAC_SDK = macosx10.11
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
OBJECTS_DIR = obj
DEFINES = VTK_EXCLUDE_STRSTREAM_HEADERS

macx {
QMAKE_CXXFLAGS_RELEASE += -O3 \
    -ffast-math
INCLUDEPATH += ${HOME}/Unix/include/vtk-5.10
LIBS += -L${HOME}/Unix/lib/vtk-5.10 \
    -lvtkRendering \
    -lvtkIO \
    -lvtkGraphics \
    -lvtkverdict \
    -lvtkImaging \
    -lvtkftgl \
    -lvtkfreetype \
    -lvtkFiltering \
    -lvtkCommon \
    -lvtkDICOMParser \
    -lvtkNetCDF \
    -lvtkmetaio \
    -lvtksqlite \
    -lvtkpng \
    -lvtkzlib \
    -lvtkjpeg \
    -lvtkexpat \
    -lvtksys \
#    -lvtktiff \ # VTK5.10.1/Utilities/vtktiff/tif_config.h.in define inline to nothing for compilation
    -framework \
    ApplicationServices \
    -framework \
    CoreServices \
    -framework \
    OpenGL \
    -framework \
    Cocoa \
    -framework \
    IOKit \
    -lpthread \
    -lm \
    -L/usr/local/lib \
    -ltiff \
    -lz
}
win32 {
INCLUDEPATH += ${HOME}/Unix/include/vtk-5.10
LIBS += -L${HOME}/Unix/lib/vtk-5.10 \
    -lvtkRendering \
    -lvtkIO \
    -lvtkGraphics \
    -lvtkverdict \
    -lvtkImaging \
    -lvtkftgl \
    -lvtkfreetype \
    -lvtkFiltering \
    -lvtkCommon \
    -lvtkDICOMParser \
    -lvtkNetCDF \
    -lvtkmetaio \
    -lvtksqlite \
    -lvtkpng \
    -lvtkzlib \
    -lvtkjpeg \
    -lvtkexpat \
    -lvtksys \
#    -lvtktiff \ # VTK5.10.1/Utilities/vtktiff/tif_config.h.in define inline to nothing for compilation
    -lm
}

SOURCES += main.cpp \
    PipelineObject.cpp \
    DataFile.cpp \
    InputFileParser.cpp \
    vtkASCIIPointCloudReader.cpp \
    vtkNrrdReader.cpp

HEADERS += PipelineObject.h \
    DataFile.h \
    InputFileParser.h \
    vtkASCIIPointCloudReader.h \
    vtkNrrdReader.h


