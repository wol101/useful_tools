# -------------------------------------------------
# Project created by QtCreator 2010-04-14T09:26:30
# -------------------------------------------------
QT -= core \
    gui
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
QMAKE_CXXFLAGS_RELEASE += -O3 \
    -ffast-math
INCLUDEPATH += gaul/src \
    gaul/util \
    ../GaitSymODE/src \
    ${HOME}/Unix/include \
    /usr/include/libxml2
LIBS += -lxml2 \
    ${HOME}/Unix/lib/libode.a
DEFINES += dDOUBLE \
    USE_GAUL \
    HAVE_MPI=0
TARGET = GaitSymGaul
OBJECTS_DIR = obj_qt

SOURCES += main.cpp \
    ../GaitSymODE/src/Util.cpp \
    ../GaitSymODE/src/UGMMuscle.cpp \
    ../GaitSymODE/src/UDP.cpp \
    ../GaitSymODE/src/TwoPointStrap.cpp \
    ../GaitSymODE/src/TIFFWrite.cpp \
    ../GaitSymODE/src/ThreePointStrap.cpp \
    ../GaitSymODE/src/TCP.cpp \
    ../GaitSymODE/src/StrokeFont.cpp \
    ../GaitSymODE/src/Strap.cpp \
    ../GaitSymODE/src/StepDriver.cpp \
    ../GaitSymODE/src/SphereGeom.cpp \
    ../GaitSymODE/src/Simulation.cpp \
    ../GaitSymODE/src/PlaneGeom.cpp \
    ../GaitSymODE/src/ObjectiveMain.cpp \
    ../GaitSymODE/src/NPointStrap.cpp \
    ../GaitSymODE/src/NamedObject.cpp \
    ../GaitSymODE/src/Muscle.cpp \
    ../GaitSymODE/src/MAMuscleExtended.cpp \
    ../GaitSymODE/src/MAMuscleComplete.cpp \
    ../GaitSymODE/src/MAMuscle.cpp \
    ../GaitSymODE/src/Joint.cpp \
    ../GaitSymODE/src/IrrlichtRoutines.cpp \
    ../GaitSymODE/src/HingeJoint.cpp \
    ../GaitSymODE/src/GLUtils.cpp \
    ../GaitSymODE/src/GLUIRoutines.cpp \
    ../GaitSymODE/src/Geom.cpp \
    ../GaitSymODE/src/FloatingHingeJoint.cpp \
    ../GaitSymODE/src/FixedJoint.cpp \
    ../GaitSymODE/src/fec.cpp \
    ../GaitSymODE/src/FacetedSphere.cpp \
    ../GaitSymODE/src/FacetedPolyline.cpp \
    ../GaitSymODE/src/FacetedObject.cpp \
    ../GaitSymODE/src/FacetedConicSegment.cpp \
    ../GaitSymODE/src/Face.cpp \
    ../GaitSymODE/src/ErrorHandler.cpp \
    ../GaitSymODE/src/Environment.cpp \
    ../GaitSymODE/src/DataTarget.cpp \
    ../GaitSymODE/src/DataFile.cpp \
    ../GaitSymODE/src/DampedSpringMuscle.cpp \
    ../GaitSymODE/src/CylinderWrapStrap.cpp \
    ../GaitSymODE/src/CyclicDriver.cpp \
    ../GaitSymODE/src/Contact.cpp \
    ../GaitSymODE/src/CappedCylinderGeom.cpp \
    ../GaitSymODE/src/Body.cpp \
    ../GaitSymODE/src/BallJoint.cpp \
    ../GaitSymODE/src/DataTargetScalar.cpp \
    ../GaitSymODE/src/DataTargetQuaternion.cpp \
    ../GaitSymODE/src/DataTargetVector.cpp \
    ../GaitSymODE/src/Driver.cpp \
    ../GaitSymODE/src/TrimeshGeom.cpp \
    ../GaitSymODE/src/RayGeom.cpp \
    ../GaitSymODE/src/Marker.cpp \
    ../GaitSymODE/src/Reporter.cpp \
    ../GaitSymODE/src/TorqueReporter.cpp \
    gaul/src/ga_utility.cpp \
    gaul/src/ga_tabu.cpp \
    gaul/src/ga_systematicsearch.cpp \
    gaul/src/ga_stats.cpp \
    gaul/src/ga_simplex.cpp \
    gaul/src/ga_similarity.cpp \
    gaul/src/ga_select.cpp \
    gaul/src/ga_seed.cpp \
    gaul/src/ga_sa.cpp \
    gaul/src/ga_replace.cpp \
    gaul/src/ga_rank.cpp \
    gaul/src/ga_randomsearch.cpp \
    gaul/src/ga_qsort.cpp \
    gaul/src/ga_optim.cpp \
    gaul/src/ga_mutate.cpp \
    gaul/src/ga_io.cpp \
    gaul/src/ga_intrinsics.cpp \
    gaul/src/ga_gradient.cpp \
    gaul/src/ga_deterministiccrowding.cpp \
    gaul/src/ga_de.cpp \
    gaul/src/ga_crossover.cpp \
    gaul/src/ga_core.cpp \
    gaul/src/ga_compare.cpp \
    gaul/src/ga_climbing.cpp \
    gaul/src/ga_chromo.cpp \
    gaul/src/ga_bitstring.cpp \
    gaul/util/timer_util.cpp \
    gaul/util/table_util.cpp \
    gaul/util/random_util.cpp \
    gaul/util/nn_util.cpp \
    gaul/util/memory_util.cpp \
    gaul/util/memory_chunks.cpp \
    gaul/util/log_util.cpp \
    gaul/util/linkedlist.cpp \
    gaul/util/compatibility.cpp \
    gaul/util/avltree.cpp \
    ../GaitSymODE/src/UniversalJoint.cpp \
    ../GaitSymODE/src/PositionReporter.cpp \
    ../GaitSymODE/src/XMLConverter.cpp \
    ../GaitSymODE/src/ExpressionVec.cpp \
    ../GaitSymODE/src/ExpressionVar.cpp \
    ../GaitSymODE/src/ExpressionRef.cpp \
    ../GaitSymODE/src/ExpressionParser.cpp \
    ../GaitSymODE/src/ExpressionMat.cpp \
    ../GaitSymODE/src/ExpressionFunTransform3D.cpp \
    ../GaitSymODE/src/ExpressionFolder.cpp \
    ../GaitSymODE/src/PIDMuscleLength.cpp \
    ../GaitSymODE/src/Drivable.cpp \
    ../GaitSymODE/src/Controller.cpp \
    ../GaitSymODE/src/AMotorJoint.cpp \
    ../GaitSymODE/src/FixedDriver.cpp \
    ../GaitSymODE/src/SliderJoint.cpp \
    ../GaitSymODE/src/BoxCarDriver.cpp \
    ../GaitSymODE/src/PIDTargetMatch.cpp \
    ../GaitSymODE/src/StackedBoxCarDriver.cpp \
    ../GaitSymODE/src/TwoCylinderWrapStrap.cpp \
    ../GaitSymODE/src/BoxGeom.cpp \
    ../GaitSymODE/src/SwingClearanceAbortReporter.cpp \
    do_genetic_algorithm.cpp \
    do_simplex_search.cpp \
    do_tabu_search.cpp \
    do_simulated_annealling.cpp \
    utilities.cpp \
    callbacks.cpp \
    do_next_ascent_hillclimbing.cpp \
    do_random_ascent_hillclimbing.cpp
HEADERS += \
    gaul/src/gaul.h \
    gaul/src/gaul/ga_tabu.h \
    gaul/src/gaul/ga_systematicsearch.h \
    gaul/src/gaul/ga_simplex.h \
    gaul/src/gaul/ga_similarity.h \
    gaul/src/gaul/ga_sa.h \
    gaul/src/gaul/ga_randomsearch.h \
    gaul/src/gaul/ga_qsort.h \
    gaul/src/gaul/ga_optim.h \
    gaul/src/gaul/ga_intrinsics.h \
    gaul/src/gaul/ga_gradient.h \
    gaul/src/gaul/ga_deterministiccrowding.h \
    gaul/src/gaul/ga_de.h \
    gaul/src/gaul/ga_core.h \
    gaul/src/gaul/ga_climbing.h \
    gaul/src/gaul/ga_chromo.h \
    gaul/src/gaul/ga_bitstring.h \
    gaul/util/gaul/timer_util.h \
    gaul/util/gaul/table_util.h \
    gaul/util/gaul/random_util.h \
    gaul/util/gaul/nn_util.h \
    gaul/util/gaul/memory_util.h \
    gaul/util/gaul/memory_chunks.h \
    gaul/util/gaul/log_util.h \
    gaul/util/gaul/linkedlist.h \
    gaul/util/gaul/gaul_util.h \
    gaul/util/gaul/gaul_config.h \
    gaul/util/gaul/gaul_config_win.h \
    gaul/util/gaul/compatibility.h \
    gaul/util/gaul/avltree.h \
    ../GaitSymODE/src/AMotorJoint.h \
    ../GaitSymODE/src/FixedDriver.h \
    ../GaitSymODE/src/SliderJoint.h \
    ../GaitSymODE/src/BoxCarDriver.h \
    ../GaitSymODE/src/PIDTargetMatch.h \
    ../GaitSymODE/src/StackedBoxCarDriver.h \
    ../GaitSymODE/src/TwoCylinderWrapStrap.h \
    ../GaitSymODE/src/BoxGeom.h \
    ../GaitSymODE/src/SwingClearanceAbortReporter.h \
    callbacks.h \
    do_next_ascent_hillclimbing.h \
    do_random_ascent_hillclimbing.h \
    do_genetic_algorithm.h \
    do_simplex_search.h \
    do_simulated_annealling.h \
    do_tabu_search.h \
    utilities.h
OTHER_FILES +=
