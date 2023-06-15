TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source # this is important to stop obj files overwriting each other
OBJECTS_DIR = obj

# mpicxx --showme
# clang++ -I/usr/local/Cellar/open-mpi/1.8.4/include -L/usr/local/opt/libevent/lib -L/usr/local/Cellar/open-mpi/1.8.4/lib -lmpi_cxx -lmpi

INCLUDEPATH += /usr/local/Cellar/open-mpi/1.8.4/include\
    ../GaitSym2016/rapidxml-1.13 \
    ../GaitSym2016/exprtk \
    ../GaitSym2016/src \
    ${HOME}/Unix/include
LIBS += -L/usr/local/opt/libevent/lib -L/usr/local/Cellar/open-mpi/1.8.4/lib -lmpi_cxx -lmpi \
    -framework Accelerate \
    -L"$(HOME)/Unix/lib" -lode -lm -lANN
DEFINES += dDOUBLE EXPERIMENTAL USE_PCA USE_CBLAS USE_MPI_ABORT USE_OLD_ODE USE_MPI
QMAKE_CXXFLAGS += -std=c++11
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
QMAKE_MAC_SDK = macosx10.11

SOURCES += \
    Bootstrap.cpp \
    UnifiedMPI.cpp \
    ../GaitSym2016/src/AMotorJoint.cpp \
    ../GaitSym2016/src/BallJoint.cpp \
    ../GaitSym2016/src/Body.cpp \
    ../GaitSym2016/src/BoxCarDriver.cpp \
    ../GaitSym2016/src/BoxGeom.cpp \
    ../GaitSym2016/src/ButterworthFilter.cpp \
    ../GaitSym2016/src/CappedCylinderGeom.cpp \
    ../GaitSym2016/src/Contact.cpp \
    ../GaitSym2016/src/Controller.cpp \
    ../GaitSym2016/src/CyclicDriver.cpp \
    ../GaitSym2016/src/CylinderWrapStrap.cpp \
    ../GaitSym2016/src/DampedSpringMuscle.cpp \
    ../GaitSym2016/src/DataFile.cpp \
    ../GaitSym2016/src/DataTarget.cpp \
    ../GaitSym2016/src/DataTargetQuaternion.cpp \
    ../GaitSym2016/src/DataTargetScalar.cpp \
    ../GaitSym2016/src/DataTargetVector.cpp \
    ../GaitSym2016/src/Drivable.cpp \
    ../GaitSym2016/src/Driver.cpp \
    ../GaitSym2016/src/Environment.cpp \
    ../GaitSym2016/src/ErrorHandler.cpp \
    ../GaitSym2016/src/Face.cpp \
    ../GaitSym2016/src/FacetedConicSegment.cpp \
    ../GaitSym2016/src/FacetedObject.cpp \
    ../GaitSym2016/src/FacetedPolyline.cpp \
    ../GaitSym2016/src/FacetedSphere.cpp \
    ../GaitSym2016/src/Filter.cpp \
    ../GaitSym2016/src/FixedDriver.cpp \
    ../GaitSym2016/src/FixedJoint.cpp \
    ../GaitSym2016/src/FloatingHingeJoint.cpp \
    ../GaitSym2016/src/Geom.cpp \
    ../GaitSym2016/src/GLUtils.cpp \
    ../GaitSym2016/src/HingeJoint.cpp \
    ../GaitSym2016/src/Joint.cpp \
    ../GaitSym2016/src/MAMuscle.cpp \
    ../GaitSym2016/src/MAMuscleComplete.cpp \
    ../GaitSym2016/src/MAMuscleExtended.cpp \
    ../GaitSym2016/src/Marker.cpp \
    ../GaitSym2016/src/MovingAverage.cpp \
    ../GaitSym2016/src/Muscle.cpp \
    ../GaitSym2016/src/NamedObject.cpp \
    ../GaitSym2016/src/NPointStrap.cpp \
    ../GaitSym2016/src/ObjectiveMain.cpp \
    ../GaitSym2016/src/PCA.cpp \
    ../GaitSym2016/src/PIDMuscleLength.cpp \
    ../GaitSym2016/src/PIDTargetMatch.cpp \
    ../GaitSym2016/src/PlaneGeom.cpp \
    ../GaitSym2016/src/PositionReporter.cpp \
    ../GaitSym2016/src/RayGeom.cpp \
    ../GaitSym2016/src/Reporter.cpp \
    ../GaitSym2016/src/Simulation.cpp \
    ../GaitSym2016/src/SliderJoint.cpp \
    ../GaitSym2016/src/SphereGeom.cpp \
    ../GaitSym2016/src/StackedBoxCarDriver.cpp \
    ../GaitSym2016/src/StepDriver.cpp \
    ../GaitSym2016/src/Strap.cpp \
    ../GaitSym2016/src/StrokeFont.cpp \
    ../GaitSym2016/src/SwingClearanceAbortReporter.cpp \
    ../GaitSym2016/src/ThreePointStrap.cpp \
    ../GaitSym2016/src/TIFFWrite.cpp \
    ../GaitSym2016/src/TorqueReporter.cpp \
    ../GaitSym2016/src/TrimeshGeom.cpp \
    ../GaitSym2016/src/TwoCylinderWrapStrap.cpp \
    ../GaitSym2016/src/TwoPointStrap.cpp \
    ../GaitSym2016/src/UGMMuscle.cpp \
    ../GaitSym2016/src/UniversalJoint.cpp \
    ../GaitSym2016/src/Util.cpp \
    ../GaitSym2016/src/Warehouse.cpp \
    ../GaitSym2016/src/XMLConverter.cpp \
    ../MergeXML/src/MergeExpressionParser.cpp \
    ../MergeXML/src/MergeXML.cpp \
    ../MergeXML/src/XMLContainer.cpp \
    ../MergeXML/src/XMLContainerList.cpp \
    ../AsynchronousGA/Genome.cpp \
    ../AsynchronousGA/main.cpp \
    ../AsynchronousGA/Mating.cpp \
    ../AsynchronousGA/Population.cpp \
    ../AsynchronousGA/Preferences.cpp \
    ../AsynchronousGA/Random.cpp \
    ../AsynchronousGA/Statistics.cpp

DISTFILES +=  \
    makefile
