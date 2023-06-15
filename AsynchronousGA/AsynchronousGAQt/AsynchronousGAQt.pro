#-------------------------------------------------
#
# Project created by QtCreator 2017-12-29T17:06:22
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsynchronousGAQt
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
        ../ptypes-2.1.1/include \
        ../windows_unistd \
        ../exprtk \
        ../rapidxml-1.13 \
        ../../GaitSym2017/ode-0.15/ode/src \
        ../../GaitSym2017/ode-0.15/libccd/src \
        ../../GaitSym2017/ode-0.15/OPCODE \
        ../../GaitSym2017/ode-0.15/include \
        ../../GaitSym2017/src

#    DEFINES += NO_CRIT_MSGBOX USE_TCP USE_QT_AGA NEED_BCOPY
    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA NEED_BCOPY USE_TCP_PTYPES \
    dIDEDOUBLE TRIMESH_ENABLED TRIMESH_OPCODE CCD_IDEDOUBLE dLIBCCD_ENABLED dTHREADING_INTF_DISABLED \
    RAPIDXML_NO_EXCEPTIONS BYTE_ORDER=LITTLE_ENDIAN HAVE_MALLOC_H USE_UNIX_ERRORS NEED_BCOPY _USE_MATH_DEFINES \
    _CRT_SECURE_NO_WARNINGS

    LIBS += -lWs2_32
    QMAKE_CXXFLAGS += -bigobj

    RC_FILE = Images/myapp.rc
}

else:unix {
    INCLUDEPATH += ../ \
        ../ptypes-2.1.1/include \
        ../exprtk \
        ../rapidxml-1.13 \
        ../../GaitSym2017/ode-0.15/ode/src \
        ../../GaitSym2017/ode-0.15/libccd/src \
        ../../GaitSym2017/ode-0.15/OPCODE \
        ../../GaitSym2017/ode-0.15/include \
        ../../GaitSym2017/src

    DEFINES += NO_CRIT_MSGBOX USE_QT_AGA NEED_BCOPY USE_TCP_PTYPES \
    dIDEDOUBLE TRIMESH_ENABLED TRIMESH_OPCODE CCD_IDEDOUBLE dLIBCCD_ENABLED dTHREADING_INTF_DISABLED \
    RAPIDXML_NO_EXCEPTIONS BYTE_ORDER=LITTLE_ENDIAN HAVE_MALLOC_H USE_UNIX_ERRORS NEED_BCOPY _USE_MATH_DEFINES \
    _CRT_SECURE_NO_WARNINGS \
    exprtk_disable_enhanced_features

    QMAKE_CXXFLAGS += -std=gnu++11
    QMAKE_CXXFLAGS_RELEASE += -O3 -ffast-math
}

OBJECTS_DIR = obj
CONFIG += no_batch # this gets around a bug in Visual Studio with the object_parallel_to_source option
CONFIG += object_parallel_to_source # this is important to stop obj files overwriting each other

SOURCES += \
     main.cpp \
     AsynchronousGAQtWidget.cpp \
    ../DataFile.cpp \
    ../FEC.cpp \
    ../GATCP.cpp \
    ../Genome.cpp \
    ../MD5.cpp \
    ../Mating.cpp \
    ../Population.cpp \
    ../Preferences.cpp \
    ../Random.cpp \
    ../Statistics.cpp \
    ../TCP.cpp \
    ../UDP.cpp \
    ../WindowsGlue.cpp \
    ../XMLConverter.cpp \
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
    ../ptypes-2.1.1/src/punit.cxx \
    ../ptypes-2.1.1/src/punknown.cxx \
    ../ptypes-2.1.1/src/pvariant.cxx \
    ../ptypes-2.1.1/src/pversion.cxx \
    ../GATCPptypes.cpp \
    ../MergeExpressionParser.cpp \
    ../MergeXML.cpp \
    ../XMLContainer.cpp \
    ../XMLContainerList.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceAABB.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceContainer.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceHPoint.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceIndexedTriangle.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceMatrix3x3.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceMatrix4x4.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceOBB.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IcePlane.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IcePoint.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceRandom.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceRay.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceRevisitedRadix.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceSegment.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceTriangle.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Ice/IceUtils.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_AABBCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_AABBTree.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_BaseModel.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_Collider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_Common.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_HybridModel.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_LSSCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_MeshInterface.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_Model.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_OBBCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_OptimizedTree.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_Picking.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_PlanesCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_RayCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_SphereCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_TreeBuilders.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_TreeCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/OPC_VolumeCollider.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/Opcode.cpp \
    ../../GaitSym2017/ode-0.15/OPCODE/StdAfx.cpp \
    ../../GaitSym2017/ode-0.15/libccd/src/alloc.c \
    ../../GaitSym2017/ode-0.15/libccd/src/ccd.c \
    ../../GaitSym2017/ode-0.15/libccd/src/mpr.c \
    ../../GaitSym2017/ode-0.15/libccd/src/polytope.c \
    ../../GaitSym2017/ode-0.15/libccd/src/support.c \
    ../../GaitSym2017/ode-0.15/libccd/src/vec3.c \
    ../../GaitSym2017/ode-0.15/ode/src/array.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/box.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/capsule.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_convex_trimesh.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_cylinder_box.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_cylinder_plane.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_cylinder_sphere.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_cylinder_trimesh.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_kernel.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_libccd.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_quadtreespace.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_sapspace.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_space.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_transform.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_box.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_ccylinder.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_disabled.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_distance.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_gimpact.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_opcode.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_plane.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_ray.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_sphere.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_trimesh.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_trimesh_trimesh_new.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/collision_util.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/convex.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/cylinder.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/error.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/export-dif.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/fastdot.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/fastldlt.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/fastlsolve.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/fastltsolve.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/heightfield.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/amotor.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/ball.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/contact.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/dball.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/dhinge.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/fixed.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/floatinghinge.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/hinge.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/hinge2.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/joint.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/lmotor.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/null.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/piston.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/plane2d.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/pr.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/pu.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/slider.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/transmission.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/joints/universal.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/lcp.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/mass.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/mat.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/matrix.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/memory.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/misc.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/nextafterf.c \
    ../../GaitSym2017/ode-0.15/ode/src/objects.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/obstack.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/ode.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/odeinit.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/odemath.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/odeou.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/odetls.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/plane.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/quickstep.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/ray.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/rotation.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/sphere.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/step.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/threading_base.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/threading_impl.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/threading_pool_posix.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/threading_pool_win.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/timer.cpp \
    ../../GaitSym2017/ode-0.15/ode/src/util.cpp \
    ../../GaitSym2017/src/AMotorJoint.cpp \
    ../../GaitSym2017/src/BallJoint.cpp \
    ../../GaitSym2017/src/Body.cpp \
    ../../GaitSym2017/src/BoxCarDriver.cpp \
    ../../GaitSym2017/src/BoxGeom.cpp \
    ../../GaitSym2017/src/ButterworthFilter.cpp \
    ../../GaitSym2017/src/CappedCylinderGeom.cpp \
    ../../GaitSym2017/src/Contact.cpp \
    ../../GaitSym2017/src/Controller.cpp \
    ../../GaitSym2017/src/CyclicDriver.cpp \
    ../../GaitSym2017/src/CylinderWrapStrap.cpp \
    ../../GaitSym2017/src/DampedSpringMuscle.cpp \
    ../../GaitSym2017/src/DataFile.cpp \
    ../../GaitSym2017/src/DataTarget.cpp \
    ../../GaitSym2017/src/DataTargetQuaternion.cpp \
    ../../GaitSym2017/src/DataTargetScalar.cpp \
    ../../GaitSym2017/src/DataTargetVector.cpp \
    ../../GaitSym2017/src/Drivable.cpp \
    ../../GaitSym2017/src/Driver.cpp \
    ../../GaitSym2017/src/Environment.cpp \
    ../../GaitSym2017/src/ErrorHandler.cpp \
    ../../GaitSym2017/src/FEC.cpp \
    ../../GaitSym2017/src/Face.cpp \
    ../../GaitSym2017/src/FacetedBox.cpp \
    ../../GaitSym2017/src/FacetedCappedCylinder.cpp \
    ../../GaitSym2017/src/FacetedConicSegment.cpp \
    ../../GaitSym2017/src/FacetedObject.cpp \
    ../../GaitSym2017/src/FacetedPolyline.cpp \
    ../../GaitSym2017/src/FacetedRect.cpp \
    ../../GaitSym2017/src/FacetedSphere.cpp \
    ../../GaitSym2017/src/Filter.cpp \
    ../../GaitSym2017/src/FixedDriver.cpp \
    ../../GaitSym2017/src/FixedJoint.cpp \
    ../../GaitSym2017/src/FloatingHingeJoint.cpp \
    ../../GaitSym2017/src/GLUtils.cpp \
    ../../GaitSym2017/src/Geom.cpp \
    ../../GaitSym2017/src/HingeJoint.cpp \
    ../../GaitSym2017/src/Joint.cpp \
    ../../GaitSym2017/src/MAMuscle.cpp \
    ../../GaitSym2017/src/MAMuscleComplete.cpp \
    ../../GaitSym2017/src/MAMuscleExtended.cpp \
    ../../GaitSym2017/src/Marker.cpp \
    ../../GaitSym2017/src/MovingAverage.cpp \
    ../../GaitSym2017/src/Muscle.cpp \
    ../../GaitSym2017/src/NPointStrap.cpp \
    ../../GaitSym2017/src/NamedObject.cpp \
    ../../GaitSym2017/src/ObjectiveMain.cpp \
    ../../GaitSym2017/src/PCA.cpp \
    ../../GaitSym2017/src/PIDErrorInController.cpp \
    ../../GaitSym2017/src/PIDMuscleLength.cpp \
    ../../GaitSym2017/src/PIDTargetMatch.cpp \
    ../../GaitSym2017/src/PlaneGeom.cpp \
    ../../GaitSym2017/src/PositionReporter.cpp \
    ../../GaitSym2017/src/RayGeom.cpp \
    ../../GaitSym2017/src/Reporter.cpp \
    ../../GaitSym2017/src/Simulation.cpp \
    ../../GaitSym2017/src/SliderJoint.cpp \
    ../../GaitSym2017/src/SphereGeom.cpp \
    ../../GaitSym2017/src/StackedBoxCarDriver.cpp \
    ../../GaitSym2017/src/StepDriver.cpp \
    ../../GaitSym2017/src/Strap.cpp \
    ../../GaitSym2017/src/StrokeFont.cpp \
    ../../GaitSym2017/src/SwingClearanceAbortReporter.cpp \
    ../../GaitSym2017/src/TCP.cpp \
    ../../GaitSym2017/src/TIFFWrite.cpp \
    ../../GaitSym2017/src/TegotaeDriver.cpp \
    ../../GaitSym2017/src/ThreePointStrap.cpp \
    ../../GaitSym2017/src/TorqueReporter.cpp \
    ../../GaitSym2017/src/TrimeshGeom.cpp \
    ../../GaitSym2017/src/TwoCylinderWrapStrap.cpp \
    ../../GaitSym2017/src/TwoPointStrap.cpp \
    ../../GaitSym2017/src/UDP.cpp \
    ../../GaitSym2017/src/UGMMuscle.cpp \
    ../../GaitSym2017/src/UniversalJoint.cpp \
    ../../GaitSym2017/src/Util.cpp \
    ../../GaitSym2017/src/Warehouse.cpp \
    ../../GaitSym2017/src/XMLConverter.cpp \
    TextEditDialog.cpp \
    Utilities.cpp \
    BasicXMLSyntaxHighlighter.cpp

HEADERS += \
     AsynchronousGAQtWidget.h \
    ../DataFile.h \
    ../DebugControl.h \
    ../FEC.h \
    ../GATCP.h \
    ../Genome.h \
    ../MD5.h \
    ../MPIStuff.h \
    ../Mating.h \
    ../Population.h \
    ../Preferences.h \
    ../Random.h \
    ../Statistics.h \
    ../TCP.h \
    ../UDP.h \
    ../WindowsGlue.h \
    ../XMLConverter.h \
    ../avl_array/avl_array.hpp \
    ../exprtk/exprtk.hpp \
    ../ptypes-2.1.1/include/pasync.h \
    ../ptypes-2.1.1/include/pinet.h \
    ../ptypes-2.1.1/include/pport.h \
    ../ptypes-2.1.1/include/pstreams.h \
    ../ptypes-2.1.1/include/ptime.h \
    ../ptypes-2.1.1/include/ptypes.h \
    ../windows_unistd/unistd.h \
    ../GATCPptypes.h \
    ../MergeExpressionParser.h \
    ../MergeUtil.h \
    ../XMLContainer.h \
    ../XMLContainerList.h \
    ../rapidxml-1.13/rapidxml.hpp \
    ../MergeXML.h \
    ../TCPIPMessage.h \
    TextEditDialog.h \
    Utilities.h \
    BasicXMLSyntaxHighlighter.h

FORMS += \
        AsynchronousGAQtWidget.ui \
    TextEditDialog.ui

RESOURCES += \
    Resources.qrc
