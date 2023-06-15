# makefile for the subprocess objective option

# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


DYN_DIR = /users/ws/Gibbon_Simulation/dynamechs_3.0/
INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL \
	-I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
DYN_LIBS = $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dmGL/libdmGL.a \
	$(DYN_DIR)dm/libdm.a 
LIBS = -lm 
OPENGL_LIBS =  -L/usr/share/src/OpenGL/toolkits/lib32 \
	-lglut -lGLU -lGL -lX11 -lXmu

all: muscle_test
	
muscle_test: \
Controller.o      MuscleModel.o     Util.o           SquareController.o\
ControllerList.o  LoadFile.o        Simulation.o      main.o\
ForceList.o       LoggedForce.o     SinController.o
	$(CXX) -o $@ $^ $(GA_LIBS) $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)
	
clean:
	$(RM) muscle_test
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) \
Controller.cc      MuscleModel.cc     Util.cc            SquareController.cc\
ControllerList.cc  LoadFile.cc        Simulation.cc      main.cc\
ForceList.cc       LoggedForce.cc     SinController.cc

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

Controller.o: /usr/include/CC/iostream.h /usr/include/memory.h
Controller.o: /usr/include/stddef.h /usr/include/sgidefs.h
Controller.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Controller.o: /usr/include/CC/fstream.h Controller.h
Controller.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
Controller.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
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
MuscleModel.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
MuscleModel.o: /usr/include/CC/iostream.h /usr/include/memory.h
MuscleModel.o: /usr/include/stddef.h /usr/include/sgidefs.h
MuscleModel.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
MuscleModel.o: /usr/include/CC/fstream.h /usr/include/math.h
MuscleModel.o: /usr/include/standards.h /usr/include/stdlib.h
MuscleModel.o: /usr/include/string.h
MuscleModel.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmu/dmu.h
MuscleModel.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmForce.h
MuscleModel.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
MuscleModel.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmArticulation.h
MuscleModel.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmLink.h
MuscleModel.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmSystem.h
MuscleModel.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
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
Util.o: /usr/include/math.h /usr/include/sgidefs.h /usr/include/standards.h
Util.o: /usr/include/stdio.h /usr/include/CC/iostream.h /usr/include/memory.h
Util.o: /usr/include/stddef.h /usr/include/CC/iomanip.h
Util.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Util.o: /usr/include/assert.h /usr/include/string.h /usr/include/errno.h
Util.o: /usr/include/sys/errno.h /usr/include/sys/types.h
Util.o: /usr/include/sys/wait.h /usr/include/unistd.h Util.h
SquareController.o: /usr/include/math.h /usr/include/sgidefs.h
SquareController.o: /usr/include/standards.h SquareController.h Controller.h
SquareController.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
SquareController.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
SquareController.o: /usr/include/CC/iostream.h /usr/include/memory.h
SquareController.o: /usr/include/stddef.h /usr/include/CC/iomanip.h
SquareController.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
SquareController.o: /usr/include/stdlib.h /usr/include/string.h
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
ControllerList.o: Controller.h
ControllerList.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
ControllerList.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
ControllerList.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
ControllerList.o: /usr/include/CC/fstream.h /usr/include/math.h
ControllerList.o: ControllerList.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
LoadFile.o: /usr/include/CC/iostream.h /usr/include/memory.h
LoadFile.o: /usr/include/stddef.h /usr/include/sgidefs.h
LoadFile.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
LoadFile.o: /usr/include/CC/fstream.h /usr/include/math.h
LoadFile.o: /usr/include/standards.h /usr/include/stdlib.h
LoadFile.o: /usr/include/string.h /usr/include/GL/gl.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmSystem.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmArticulation.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmLink.h
LoadFile.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
LoadFile.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
LoadFile.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
LoadFile.o: /usr/include/CC/type_traits.h /usr/include/limits.h
LoadFile.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
LoadFile.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
LoadFile.o: /usr/include/stdio.h /usr/include/assert.h
LoadFile.o: /usr/include/CC/stl_construct.h
LoadFile.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
LoadFile.o: /usr/include/CC/stl_bvector.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmZScrewTxLink.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmMDHLink.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmRigidBody.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmForce.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmActuator.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmRevoluteLink.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmPrismaticLink.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmSphericalLink.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmMobileBaseLink.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmRevDCMotor.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmContactModel.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmEnvironment.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmu/dmLoadFile.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmu/dmu.h
LoadFile.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmu/glLoadModels.h
LoadFile.o: LoadFile.h SinController.h Controller.h SquareController.h
LoadFile.o: MuscleModel.h LoggedForce.h Simulation.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
Simulation.o: /usr/include/CC/iostream.h /usr/include/memory.h
Simulation.o: /usr/include/stddef.h /usr/include/sgidefs.h
Simulation.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Simulation.o: /usr/include/CC/fstream.h /usr/include/math.h
Simulation.o: /usr/include/standards.h /usr/include/stdlib.h
Simulation.o: /usr/include/string.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmSystem.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmEnvironment.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmArticulation.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmLink.h
Simulation.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
Simulation.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
Simulation.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
Simulation.o: /usr/include/CC/type_traits.h /usr/include/limits.h
Simulation.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
Simulation.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
Simulation.o: /usr/include/stdio.h /usr/include/assert.h
Simulation.o: /usr/include/CC/stl_construct.h
Simulation.o: /usr/include/CC/stl_uninitialized.h
Simulation.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmIntegrator.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmIntegEuler.h
Simulation.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmu/dmu.h LoadFile.h
Simulation.o: ForceList.h ControllerList.h Simulation.h /usr/include/GL/gl.h
main.o: /usr/include/assert.h
main.o: /usr/share/src/OpenGL/toolkits/include/GL/glut.h /usr/include/GL/gl.h
main.o: /usr/include/GL/glu.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmGL/dmGLTime.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmGL/dmGL.h
main.o: /usr/include/CC/iostream.h /usr/include/memory.h
main.o: /usr/include/stddef.h /usr/include/sgidefs.h
main.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
main.o: /usr/include/GL/gl.h /usr/include/GL/glu.h /usr/include/math.h
main.o: /usr/include/standards.h /usr/include/stdlib.h
main.o: /usr/include/sys/time.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmGL/dmGLMouse.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmGL/dmGLPolarCamera_zup.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmGL/dmGLPolarCamera.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dmGL/dmGLCamera.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmEnvironment.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
main.o: /usr/include/CC/fstream.h /usr/include/string.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmArticulation.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmLink.h
main.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmSystem.h
main.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
main.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
main.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
main.o: /usr/include/CC/type_traits.h /usr/include/limits.h
main.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
main.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
main.o: /usr/include/stdio.h /usr/include/CC/stl_construct.h
main.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
main.o: /usr/include/CC/stl_bvector.h ForceList.h Simulation.h Controller.h
main.o: ControllerList.h
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
ForceList.o: /usr/include/CC/stl_bvector.h
ForceList.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmForce.h
ForceList.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
ForceList.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
ForceList.o: /usr/include/CC/fstream.h /usr/include/math.h
ForceList.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
ForceList.o: ForceList.h
LoggedForce.o: /usr/include/stdio.h /usr/include/standards.h
LoggedForce.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
LoggedForce.o: /usr/include/memory.h /usr/include/stddef.h
LoggedForce.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
LoggedForce.o: /usr/include/CC/fstream.h /usr/include/assert.h
LoggedForce.o: /usr/include/string.h
LoggedForce.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
LoggedForce.o: /usr/include/math.h /usr/include/stdlib.h
LoggedForce.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmForce.h
LoggedForce.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
LoggedForce.o: LoggedForce.h
SinController.o: /usr/include/math.h /usr/include/sgidefs.h
SinController.o: /usr/include/standards.h SinController.h Controller.h
SinController.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dmObject.h
SinController.o: /users/ws/Gibbon_Simulation/dynamechs_3.0/dm/dm.h
SinController.o: /usr/include/CC/iostream.h /usr/include/memory.h
SinController.o: /usr/include/stddef.h /usr/include/CC/iomanip.h
SinController.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
SinController.o: /usr/include/stdlib.h /usr/include/string.h
