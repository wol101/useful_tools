# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -ptused 
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c

# set up a compilation rule for gnu make
.SUFFIXES: .cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<

DYN_DIR = ../../dynamechs_3.0/
INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL \
	-I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
DYN_LIBS = $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dmGL/libdmGL.a \
	$(DYN_DIR)dm/libdm.a 
LIBS = -lm 
OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 \
	-lglut -lGLU -lGL -lX11 -lXmu

all: objective
	
objective: \
Bar.o             ForceList.o       LoggedForce.o     Simulation.o \
Controller.o      GripModel.o       MuscleModel.o     SinController.o \
ControllerList.o  LoadFile.o        Objective.o       Util.o \
strtok.o
	$(CXX) -o $@ $^ $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)
	$(CP) $@ ../bin 

clean:
	$(RM) objective
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) \
Bar.cpp             ForceList.cpp       LoggedForce.cpp     Simulation.cpp \
Controller.cpp      GripModel.cpp       MuscleModel.cpp     SinController.cpp \
ControllerList.cpp  LoadFile.cpp        Objective.cpp       Util.cpp

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

Bar.o: /usr/include/math.h /usr/include/sgidefs.h /usr/include/standards.h
Bar.o: ../../dynamechs_3.0/dmu/glLoadModels.h ../../dynamechs_3.0/dmu/dmu.h
Bar.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
Bar.o: /usr/include/memory.h /usr/include/stddef.h /usr/include/CC/iomanip.h
Bar.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Bar.o: /usr/include/stdlib.h /usr/include/string.h /usr/include/GL/gl.h Bar.h
ForceList.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
ForceList.o: /usr/include/CC/stl_config.h /usr/include/standards.h
ForceList.o: /usr/include/CC/stl_algobase.h /usr/include/CC/stl_relops.h
ForceList.o: /usr/include/CC/stl_pair.h /usr/include/CC/type_traits.h
ForceList.o: /usr/include/string.h /usr/include/limits.h
ForceList.o: /usr/include/sgidefs.h /usr/include/stdlib.h
ForceList.o: /usr/include/stddef.h /usr/include/CC/new.h
ForceList.o: /usr/include/CC/iostream.h /usr/include/memory.h
ForceList.o: /usr/include/CC/stl_iterator_base.h
ForceList.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
ForceList.o: /usr/include/stdio.h /usr/include/assert.h
ForceList.o: /usr/include/CC/stl_construct.h
ForceList.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
ForceList.o: /usr/include/CC/stl_bvector.h ../../dynamechs_3.0/dm/dmForce.h
ForceList.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iomanip.h
ForceList.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
ForceList.o: /usr/include/math.h ../../dynamechs_3.0/dm/dmObject.h
ForceList.o: ForceList.h
LoggedForce.o: /usr/include/stdio.h /usr/include/standards.h
LoggedForce.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
LoggedForce.o: /usr/include/memory.h /usr/include/stddef.h
LoggedForce.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
LoggedForce.o: /usr/include/CC/fstream.h /usr/include/assert.h
LoggedForce.o: /usr/include/string.h ../../dynamechs_3.0/dm/dm.h
LoggedForce.o: /usr/include/math.h /usr/include/stdlib.h
LoggedForce.o: ../../dynamechs_3.0/dm/dmForce.h
LoggedForce.o: ../../dynamechs_3.0/dm/dmObject.h LoggedForce.h
Simulation.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
Simulation.o: /usr/include/memory.h /usr/include/stddef.h
Simulation.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
Simulation.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Simulation.o: /usr/include/math.h /usr/include/standards.h
Simulation.o: /usr/include/stdlib.h /usr/include/string.h
Simulation.o: ../../dynamechs_3.0/dm/dmSystem.h
Simulation.o: ../../dynamechs_3.0/dm/dmObject.h
Simulation.o: ../../dynamechs_3.0/dm/dmEnvironment.h
Simulation.o: ../../dynamechs_3.0/dm/dmArticulation.h
Simulation.o: ../../dynamechs_3.0/dm/dmLink.h /usr/include/CC/vector
Simulation.o: /usr/include/CC/stl_range_errors.h /usr/include/CC/stl_config.h
Simulation.o: /usr/include/CC/stl_algobase.h /usr/include/CC/stl_relops.h
Simulation.o: /usr/include/CC/stl_pair.h /usr/include/CC/type_traits.h
Simulation.o: /usr/include/limits.h /usr/include/CC/new.h
Simulation.o: /usr/include/CC/stl_iterator_base.h
Simulation.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
Simulation.o: /usr/include/stdio.h /usr/include/assert.h
Simulation.o: /usr/include/CC/stl_construct.h
Simulation.o: /usr/include/CC/stl_uninitialized.h
Simulation.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
Simulation.o: ../../dynamechs_3.0/dm/dmIntegrator.h
Simulation.o: ../../dynamechs_3.0/dm/dmIntegEuler.h
Simulation.o: ../../dynamechs_3.0/dmu/dmu.h Bar.h /usr/include/GL/gl.h
Simulation.o: LoadFile.h ForceList.h ControllerList.h Simulation.h
Controller.o: /usr/include/CC/iostream.h /usr/include/memory.h
Controller.o: /usr/include/stddef.h /usr/include/sgidefs.h
Controller.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Controller.o: /usr/include/CC/fstream.h Controller.h
Controller.o: ../../dynamechs_3.0/dm/dmObject.h ../../dynamechs_3.0/dm/dm.h
Controller.o: /usr/include/math.h /usr/include/standards.h
Controller.o: /usr/include/stdlib.h /usr/include/string.h ControllerList.h
Controller.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
Controller.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
Controller.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
Controller.o: /usr/include/CC/type_traits.h /usr/include/limits.h
Controller.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
Controller.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
Controller.o: /usr/include/stdio.h /usr/include/assert.h
Controller.o: /usr/include/CC/stl_construct.h
Controller.o: /usr/include/CC/stl_uninitialized.h
Controller.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
Controller.o: Simulation.h /usr/include/GL/gl.h
GripModel.o: /usr/include/CC/iostream.h /usr/include/memory.h
GripModel.o: /usr/include/stddef.h /usr/include/sgidefs.h
GripModel.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
GripModel.o: /usr/include/CC/fstream.h /usr/include/assert.h
GripModel.o: ../../dynamechs_3.0/dm/dm.h /usr/include/math.h
GripModel.o: /usr/include/standards.h /usr/include/stdlib.h
GripModel.o: /usr/include/string.h Bar.h /usr/include/GL/gl.h Util.h
GripModel.o: GripModel.h LoggedForce.h ../../dynamechs_3.0/dm/dmForce.h
GripModel.o: ../../dynamechs_3.0/dm/dmObject.h Controller.h ForceList.h
GripModel.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
GripModel.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
GripModel.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
GripModel.o: /usr/include/CC/type_traits.h /usr/include/limits.h
GripModel.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
GripModel.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
GripModel.o: /usr/include/stdio.h /usr/include/CC/stl_construct.h
GripModel.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
GripModel.o: /usr/include/CC/stl_bvector.h Simulation.h
MuscleModel.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
MuscleModel.o: /usr/include/memory.h /usr/include/stddef.h
MuscleModel.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
MuscleModel.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
MuscleModel.o: /usr/include/math.h /usr/include/standards.h
MuscleModel.o: /usr/include/stdlib.h /usr/include/string.h
MuscleModel.o: ../../dynamechs_3.0/dmu/dmu.h ../../dynamechs_3.0/dm/dmForce.h
MuscleModel.o: ../../dynamechs_3.0/dm/dmObject.h
MuscleModel.o: ../../dynamechs_3.0/dm/dmArticulation.h
MuscleModel.o: ../../dynamechs_3.0/dm/dmLink.h
MuscleModel.o: ../../dynamechs_3.0/dm/dmSystem.h /usr/include/CC/vector
MuscleModel.o: /usr/include/CC/stl_range_errors.h
MuscleModel.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
MuscleModel.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
MuscleModel.o: /usr/include/CC/type_traits.h /usr/include/limits.h
MuscleModel.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
MuscleModel.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
MuscleModel.o: /usr/include/stdio.h /usr/include/assert.h
MuscleModel.o: /usr/include/CC/stl_construct.h
MuscleModel.o: /usr/include/CC/stl_uninitialized.h
MuscleModel.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
MuscleModel.o: /usr/include/GL/gl.h MuscleModel.h LoggedForce.h Controller.h
MuscleModel.o: Util.h ForceList.h Simulation.h
SinController.o: /usr/include/math.h /usr/include/sgidefs.h
SinController.o: /usr/include/standards.h SinController.h Controller.h
SinController.o: ../../dynamechs_3.0/dm/dmObject.h
SinController.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
SinController.o: /usr/include/memory.h /usr/include/stddef.h
SinController.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
SinController.o: /usr/include/CC/fstream.h /usr/include/stdlib.h
SinController.o: /usr/include/string.h
ControllerList.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
ControllerList.o: /usr/include/CC/stl_config.h /usr/include/standards.h
ControllerList.o: /usr/include/CC/stl_algobase.h /usr/include/CC/stl_relops.h
ControllerList.o: /usr/include/CC/stl_pair.h /usr/include/CC/type_traits.h
ControllerList.o: /usr/include/string.h /usr/include/limits.h
ControllerList.o: /usr/include/sgidefs.h /usr/include/stdlib.h
ControllerList.o: /usr/include/stddef.h /usr/include/CC/new.h
ControllerList.o: /usr/include/CC/iostream.h /usr/include/memory.h
ControllerList.o: /usr/include/CC/stl_iterator_base.h
ControllerList.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
ControllerList.o: /usr/include/stdio.h /usr/include/assert.h
ControllerList.o: /usr/include/CC/stl_construct.h
ControllerList.o: /usr/include/CC/stl_uninitialized.h
ControllerList.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
ControllerList.o: Controller.h ../../dynamechs_3.0/dm/dmObject.h
ControllerList.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iomanip.h
ControllerList.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
ControllerList.o: /usr/include/math.h ControllerList.h
LoadFile.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
LoadFile.o: /usr/include/memory.h /usr/include/stddef.h
LoadFile.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
LoadFile.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
LoadFile.o: /usr/include/math.h /usr/include/standards.h
LoadFile.o: /usr/include/stdlib.h /usr/include/string.h /usr/include/GL/gl.h
LoadFile.o: ../../dynamechs_3.0/dm/dmObject.h
LoadFile.o: ../../dynamechs_3.0/dm/dmSystem.h
LoadFile.o: ../../dynamechs_3.0/dm/dmArticulation.h
LoadFile.o: ../../dynamechs_3.0/dm/dmLink.h /usr/include/CC/vector
LoadFile.o: /usr/include/CC/stl_range_errors.h /usr/include/CC/stl_config.h
LoadFile.o: /usr/include/CC/stl_algobase.h /usr/include/CC/stl_relops.h
LoadFile.o: /usr/include/CC/stl_pair.h /usr/include/CC/type_traits.h
LoadFile.o: /usr/include/limits.h /usr/include/CC/new.h
LoadFile.o: /usr/include/CC/stl_iterator_base.h
LoadFile.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
LoadFile.o: /usr/include/stdio.h /usr/include/assert.h
LoadFile.o: /usr/include/CC/stl_construct.h
LoadFile.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
LoadFile.o: /usr/include/CC/stl_bvector.h
LoadFile.o: ../../dynamechs_3.0/dm/dmZScrewTxLink.h
LoadFile.o: ../../dynamechs_3.0/dm/dmMDHLink.h
LoadFile.o: ../../dynamechs_3.0/dm/dmRigidBody.h
LoadFile.o: ../../dynamechs_3.0/dm/dmForce.h
LoadFile.o: ../../dynamechs_3.0/dm/dmActuator.h
LoadFile.o: ../../dynamechs_3.0/dm/dmRevoluteLink.h
LoadFile.o: ../../dynamechs_3.0/dm/dmPrismaticLink.h
LoadFile.o: ../../dynamechs_3.0/dm/dmSphericalLink.h
LoadFile.o: ../../dynamechs_3.0/dm/dmMobileBaseLink.h
LoadFile.o: ../../dynamechs_3.0/dm/dmRevDCMotor.h
LoadFile.o: ../../dynamechs_3.0/dm/dmContactModel.h
LoadFile.o: ../../dynamechs_3.0/dm/dmEnvironment.h
LoadFile.o: ../../dynamechs_3.0/dmu/dmLoadFile.h
LoadFile.o: ../../dynamechs_3.0/dmu/dmu.h
LoadFile.o: ../../dynamechs_3.0/dmu/glLoadModels.h LoadFile.h SinController.h
LoadFile.o: Controller.h GripModel.h LoggedForce.h MuscleModel.h Simulation.h
Objective.o: /usr/include/stdio.h /usr/include/standards.h
Objective.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
Objective.o: /usr/include/memory.h /usr/include/stddef.h
Objective.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Objective.o: /usr/include/CC/fstream.h /usr/include/assert.h
Objective.o: /usr/share/src/OpenGL/toolkits/include/GL/glut.h
Objective.o: /usr/include/GL/gl.h /usr/include/GL/glu.h
Objective.o: ../../dynamechs_3.0/dmGL/dmGLTime.h
Objective.o: ../../dynamechs_3.0/dmGL/dmGL.h /usr/include/GL/gl.h
Objective.o: /usr/include/GL/glu.h /usr/include/math.h /usr/include/stdlib.h
Objective.o: /usr/include/sys/time.h ../../dynamechs_3.0/dmGL/dmGLMouse.h
Objective.o: ../../dynamechs_3.0/dmGL/dmGLPolarCamera_zup.h
Objective.o: ../../dynamechs_3.0/dmGL/dmGLPolarCamera.h
Objective.o: ../../dynamechs_3.0/dmGL/dmGLCamera.h
Objective.o: ../../dynamechs_3.0/dm/dmEnvironment.h
Objective.o: ../../dynamechs_3.0/dm/dm.h /usr/include/string.h
Objective.o: ../../dynamechs_3.0/dm/dmObject.h
Objective.o: ../../dynamechs_3.0/dm/dmArticulation.h
Objective.o: ../../dynamechs_3.0/dm/dmLink.h
Objective.o: ../../dynamechs_3.0/dm/dmSystem.h /usr/include/CC/vector
Objective.o: /usr/include/CC/stl_range_errors.h /usr/include/CC/stl_config.h
Objective.o: /usr/include/CC/stl_algobase.h /usr/include/CC/stl_relops.h
Objective.o: /usr/include/CC/stl_pair.h /usr/include/CC/type_traits.h
Objective.o: /usr/include/limits.h /usr/include/CC/new.h
Objective.o: /usr/include/CC/stl_iterator_base.h
Objective.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
Objective.o: /usr/include/CC/stl_construct.h
Objective.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
Objective.o: /usr/include/CC/stl_bvector.h
Objective.o: ../../dynamechs_3.0/dm/dmMobileBaseLink.h
Objective.o: ../../dynamechs_3.0/dm/dmRigidBody.h
Objective.o: ../../dynamechs_3.0/dm/dmForce.h ../../dynamechs_3.0/dmu/dmu.h
Objective.o: Bar.h ForceList.h Simulation.h Controller.h ControllerList.h
Objective.o: GripModel.h LoggedForce.h Util.h
Util.o: /usr/include/math.h /usr/include/sgidefs.h /usr/include/standards.h
Util.o: /usr/include/stdio.h /usr/include/CC/iostream.h /usr/include/memory.h
Util.o: /usr/include/stddef.h /usr/include/CC/iomanip.h
Util.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Util.o: /usr/include/assert.h /usr/include/string.h Util.h
