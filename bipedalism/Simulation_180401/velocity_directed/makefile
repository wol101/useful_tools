# makefile for the subprocess objective option

# useful functions
MKDEPEND    = makedepend -I/usr/include
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
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


DYN_DIR = /users/ws/dynamechs/
INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL \
	-I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
DYN_LIBS = $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dmGL/libdmGL.a \
	$(DYN_DIR)dm/libdm.a 
LIBS = -lm 
OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 \
	-lglut -lGLU -lGL -lX11 -lXmu

all: objective
	
objective: \
Simulation.o           Fitness.o              ModifiedContactModel.o\
Controller.o           SinController.o        StepController.o\
ControllerList.o       ObjectiveMain.o\
LoggedForce.o          Util.o                 SquareController.o\
ForceList.o            MuscleModel.o          \
Segments.o             ParameterFile.o
	$(CXX) -o $@ $^ $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)

clean:
	$(RM) objective
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) \
ObjectiveMain.cc        ModifiedContactModel.cc\
Controller.cc           Simulation.cc           StepController.cc\
ControllerList.cc       SinController.cc\
Fitness.cc              LoggedForce.cc\
ForceList.cc            MuscleModel.cc          Util.cc\
SquareController.cc     Segments.cc             ParameterFile.cc

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

ObjectiveMain.o: /usr/include/stdio.h /usr/include/standards.h
ObjectiveMain.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
ObjectiveMain.o: /usr/include/memory.h /usr/include/stddef.h
ObjectiveMain.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
ObjectiveMain.o: /usr/include/CC/fstream.h /usr/include/assert.h
ObjectiveMain.o: /usr/include/unistd.h
ObjectiveMain.o: /usr/share/src/OpenGL/toolkits/include/GL/glut.h
ObjectiveMain.o: /usr/include/GL/gl.h /usr/include/GL/glu.h
ObjectiveMain.o: /users/ws/dynamechs/dmGL/dmGLTime.h
ObjectiveMain.o: /users/ws/dynamechs/dmGL/dmGL.h /usr/include/gl.h
ObjectiveMain.o: /usr/include/GL/glu.h /usr/include/math.h
ObjectiveMain.o: /usr/include/stdlib.h /usr/include/sys/time.h
ObjectiveMain.o: /users/ws/dynamechs/dmGL/dmGLMouse.h
ObjectiveMain.o: /users/ws/dynamechs/dmGL/dmGLPolarCamera_zup.h
ObjectiveMain.o: /users/ws/dynamechs/dmGL/dmGLPolarCamera.h
ObjectiveMain.o: /users/ws/dynamechs/dmGL/dmGLCamera.h
ObjectiveMain.o: /users/ws/dynamechs/dm/dmEnvironment.h
ObjectiveMain.o: /users/ws/dynamechs/dm/dm.h /usr/include/string.h
ObjectiveMain.o: /users/ws/dynamechs/dm/dmObject.h
ObjectiveMain.o: /users/ws/dynamechs/dm/dmArticulation.h
ObjectiveMain.o: /users/ws/dynamechs/dm/dmLink.h
ObjectiveMain.o: /users/ws/dynamechs/dm/dmSystem.h /usr/include/CC/vector
ObjectiveMain.o: /usr/include/CC/stl_range_errors.h
ObjectiveMain.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
ObjectiveMain.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
ObjectiveMain.o: /usr/include/CC/type_traits.h /usr/include/limits.h
ObjectiveMain.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
ObjectiveMain.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
ObjectiveMain.o: /usr/include/CC/stl_construct.h
ObjectiveMain.o: /usr/include/CC/stl_uninitialized.h
ObjectiveMain.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
ObjectiveMain.o: Fitness.h Util.h Simulation.h ForceList.h Controller.h
ObjectiveMain.o: ControllerList.h
ModifiedContactModel.o: /users/ws/dynamechs/dm/dm.h
ModifiedContactModel.o: /usr/include/CC/iostream.h /usr/include/memory.h
ModifiedContactModel.o: /usr/include/stddef.h /usr/include/sgidefs.h
ModifiedContactModel.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
ModifiedContactModel.o: /usr/include/CC/fstream.h /usr/include/math.h
ModifiedContactModel.o: /usr/include/standards.h /usr/include/stdlib.h
ModifiedContactModel.o: /usr/include/string.h
ModifiedContactModel.o: /users/ws/dynamechs/dm/dmForce.h
ModifiedContactModel.o: /users/ws/dynamechs/dm/dmObject.h
ModifiedContactModel.o: /users/ws/dynamechs/dm/dmEnvironment.h ForceList.h
ModifiedContactModel.o: /usr/include/CC/vector
ModifiedContactModel.o: /usr/include/CC/stl_range_errors.h
ModifiedContactModel.o: /usr/include/CC/stl_config.h
ModifiedContactModel.o: /usr/include/CC/stl_algobase.h
ModifiedContactModel.o: /usr/include/CC/stl_relops.h
ModifiedContactModel.o: /usr/include/CC/stl_pair.h
ModifiedContactModel.o: /usr/include/CC/type_traits.h /usr/include/limits.h
ModifiedContactModel.o: /usr/include/CC/new.h
ModifiedContactModel.o: /usr/include/CC/stl_iterator_base.h
ModifiedContactModel.o: /usr/include/CC/stl_iterator.h
ModifiedContactModel.o: /usr/include/CC/stl_alloc.h /usr/include/stdio.h
ModifiedContactModel.o: /usr/include/assert.h /usr/include/CC/stl_construct.h
ModifiedContactModel.o: /usr/include/CC/stl_uninitialized.h
ModifiedContactModel.o: /usr/include/CC/stl_vector.h
ModifiedContactModel.o: /usr/include/CC/stl_bvector.h Simulation.h
ModifiedContactModel.o: /usr/include/gl.h ModifiedContactModel.h
ModifiedContactModel.o: ./LoggedForce.h
Controller.o: /usr/include/CC/iostream.h /usr/include/memory.h
Controller.o: /usr/include/stddef.h /usr/include/sgidefs.h
Controller.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Controller.o: /usr/include/CC/fstream.h /usr/include/string.h
Controller.o: /usr/include/standards.h Controller.h
Controller.o: /users/ws/dynamechs/dm/dmObject.h /users/ws/dynamechs/dm/dm.h
Controller.o: /usr/include/math.h /usr/include/stdlib.h ControllerList.h
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
Controller.o: Simulation.h /usr/include/gl.h
Simulation.o: /users/ws/dynamechs/dm/dm.h /usr/include/CC/iostream.h
Simulation.o: /usr/include/memory.h /usr/include/stddef.h
Simulation.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
Simulation.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Simulation.o: /usr/include/math.h /usr/include/standards.h
Simulation.o: /usr/include/stdlib.h /usr/include/string.h
Simulation.o: /users/ws/dynamechs/dm/dmSystem.h
Simulation.o: /users/ws/dynamechs/dm/dmObject.h
Simulation.o: /users/ws/dynamechs/dm/dmEnvironment.h
Simulation.o: /users/ws/dynamechs/dm/dmArticulation.h
Simulation.o: /users/ws/dynamechs/dm/dmLink.h /usr/include/CC/vector
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
Simulation.o: /users/ws/dynamechs/dm/dmIntegrator.h
Simulation.o: /users/ws/dynamechs/dm/dmIntegEuler.h
Simulation.o: /users/ws/dynamechs/dmu/dmu.h ForceList.h ControllerList.h
Simulation.o: Segments.h /users/ws/dynamechs/dm/dmMobileBaseLink.h
Simulation.o: /users/ws/dynamechs/dm/dmRigidBody.h
Simulation.o: /users/ws/dynamechs/dm/dmForce.h
Simulation.o: /users/ws/dynamechs/dm/dmRevoluteLink.h
Simulation.o: /users/ws/dynamechs/dm/dmMDHLink.h
Simulation.o: /users/ws/dynamechs/dm/dmActuator.h Simulation.h
Simulation.o: /usr/include/gl.h
StepController.o: /usr/include/math.h /usr/include/sgidefs.h
StepController.o: /usr/include/standards.h /usr/include/assert.h
StepController.o: StepController.h Controller.h
StepController.o: /users/ws/dynamechs/dm/dmObject.h
StepController.o: /users/ws/dynamechs/dm/dm.h /usr/include/CC/iostream.h
StepController.o: /usr/include/memory.h /usr/include/stddef.h
StepController.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
StepController.o: /usr/include/CC/fstream.h /usr/include/stdlib.h
StepController.o: /usr/include/string.h
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
ControllerList.o: Controller.h /users/ws/dynamechs/dm/dmObject.h
ControllerList.o: /users/ws/dynamechs/dm/dm.h /usr/include/CC/iomanip.h
ControllerList.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
ControllerList.o: /usr/include/math.h ControllerList.h
SinController.o: /usr/include/math.h /usr/include/sgidefs.h
SinController.o: /usr/include/standards.h /usr/include/assert.h
SinController.o: SinController.h Controller.h
SinController.o: /users/ws/dynamechs/dm/dmObject.h
SinController.o: /users/ws/dynamechs/dm/dm.h /usr/include/CC/iostream.h
SinController.o: /usr/include/memory.h /usr/include/stddef.h
SinController.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
SinController.o: /usr/include/CC/fstream.h /usr/include/stdlib.h
SinController.o: /usr/include/string.h
Fitness.o: /usr/include/stdio.h /usr/include/standards.h
Fitness.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
Fitness.o: /usr/include/memory.h /usr/include/stddef.h
Fitness.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Fitness.o: /usr/include/CC/fstream.h /usr/include/assert.h
Fitness.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
Fitness.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
Fitness.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
Fitness.o: /usr/include/CC/type_traits.h /usr/include/string.h
Fitness.o: /usr/include/limits.h /usr/include/stdlib.h /usr/include/CC/new.h
Fitness.o: /usr/include/CC/stl_iterator_base.h /usr/include/CC/stl_iterator.h
Fitness.o: /usr/include/CC/stl_alloc.h /usr/include/CC/stl_construct.h
Fitness.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
Fitness.o: /usr/include/CC/stl_bvector.h /usr/include/math.h
Fitness.o: /usr/include/float.h
Fitness.o: /usr/share/src/OpenGL/toolkits/include/GL/glut.h
Fitness.o: /usr/include/GL/gl.h /usr/include/GL/glu.h
Fitness.o: /users/ws/dynamechs/dmGL/dmGLTime.h
Fitness.o: /users/ws/dynamechs/dmGL/dmGL.h /usr/include/gl.h
Fitness.o: /usr/include/GL/glu.h /usr/include/sys/time.h
Fitness.o: /users/ws/dynamechs/dmGL/dmGLMouse.h
Fitness.o: /users/ws/dynamechs/dmGL/dmGLPolarCamera_zup.h
Fitness.o: /users/ws/dynamechs/dmGL/dmGLPolarCamera.h
Fitness.o: /users/ws/dynamechs/dmGL/dmGLCamera.h
Fitness.o: /users/ws/dynamechs/dm/dmEnvironment.h /users/ws/dynamechs/dm/dm.h
Fitness.o: /users/ws/dynamechs/dm/dmObject.h
Fitness.o: /users/ws/dynamechs/dm/dmArticulation.h
Fitness.o: /users/ws/dynamechs/dm/dmLink.h /users/ws/dynamechs/dm/dmSystem.h
Fitness.o: /users/ws/dynamechs/dm/dmMobileBaseLink.h
Fitness.o: /users/ws/dynamechs/dm/dmRigidBody.h
Fitness.o: /users/ws/dynamechs/dm/dmForce.h /users/ws/dynamechs/dmu/dmu.h
Fitness.o: ForceList.h Simulation.h Controller.h ControllerList.h
Fitness.o: ./LoggedForce.h Util.h Fitness.h ModifiedContactModel.h
Fitness.o: SegmentParameters.h ParameterFile.h
LoggedForce.o: /usr/include/stdio.h /usr/include/standards.h
LoggedForce.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
LoggedForce.o: /usr/include/memory.h /usr/include/stddef.h
LoggedForce.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
LoggedForce.o: /usr/include/CC/fstream.h /usr/include/assert.h
LoggedForce.o: /usr/include/string.h /users/ws/dynamechs/dm/dm.h
LoggedForce.o: /usr/include/math.h /usr/include/stdlib.h
LoggedForce.o: /users/ws/dynamechs/dm/dmForce.h
LoggedForce.o: /users/ws/dynamechs/dm/dmObject.h ./LoggedForce.h
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
ForceList.o: /usr/include/CC/stl_bvector.h /users/ws/dynamechs/dm/dmForce.h
ForceList.o: /users/ws/dynamechs/dm/dm.h /usr/include/CC/iomanip.h
ForceList.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
ForceList.o: /usr/include/math.h /users/ws/dynamechs/dm/dmObject.h
ForceList.o: ForceList.h
MuscleModel.o: /users/ws/dynamechs/dm/dm.h /usr/include/CC/iostream.h
MuscleModel.o: /usr/include/memory.h /usr/include/stddef.h
MuscleModel.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
MuscleModel.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
MuscleModel.o: /usr/include/math.h /usr/include/standards.h
MuscleModel.o: /usr/include/stdlib.h /usr/include/string.h
MuscleModel.o: /users/ws/dynamechs/dmu/dmu.h /users/ws/dynamechs/dm/dmForce.h
MuscleModel.o: /users/ws/dynamechs/dm/dmObject.h
MuscleModel.o: /users/ws/dynamechs/dm/dmArticulation.h
MuscleModel.o: /users/ws/dynamechs/dm/dmLink.h
MuscleModel.o: /users/ws/dynamechs/dm/dmSystem.h /usr/include/CC/vector
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
MuscleModel.o: /usr/include/gl.h MuscleModel.h ./LoggedForce.h Controller.h
MuscleModel.o: Util.h ForceList.h Simulation.h
Util.o: /usr/include/math.h /usr/include/sgidefs.h /usr/include/standards.h
Util.o: /usr/include/stdio.h /usr/include/CC/iostream.h /usr/include/memory.h
Util.o: /usr/include/stddef.h /usr/include/CC/iomanip.h
Util.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Util.o: /usr/include/assert.h /usr/include/string.h /usr/include/errno.h
Util.o: /usr/include/sys/errno.h /usr/include/sys/types.h
Util.o: /usr/include/sys/wait.h /usr/include/unistd.h Util.h
SquareController.o: /usr/include/math.h /usr/include/sgidefs.h
SquareController.o: /usr/include/standards.h /usr/include/assert.h
SquareController.o: SquareController.h Controller.h
SquareController.o: /users/ws/dynamechs/dm/dmObject.h
SquareController.o: /users/ws/dynamechs/dm/dm.h /usr/include/CC/iostream.h
SquareController.o: /usr/include/memory.h /usr/include/stddef.h
SquareController.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
SquareController.o: /usr/include/CC/fstream.h /usr/include/stdlib.h
SquareController.o: /usr/include/string.h
Segments.o: /usr/include/string.h /usr/include/standards.h
Segments.o: /users/ws/dynamechs/dmu/glLoadModels.h
Segments.o: /users/ws/dynamechs/dmu/dmu.h /users/ws/dynamechs/dm/dm.h
Segments.o: /usr/include/CC/iostream.h /usr/include/memory.h
Segments.o: /usr/include/stddef.h /usr/include/sgidefs.h
Segments.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Segments.o: /usr/include/CC/fstream.h /usr/include/math.h
Segments.o: /usr/include/stdlib.h /usr/include/gl.h Segments.h
Segments.o: /users/ws/dynamechs/dm/dmMobileBaseLink.h
Segments.o: /users/ws/dynamechs/dm/dmRigidBody.h
Segments.o: /users/ws/dynamechs/dm/dmForce.h
Segments.o: /users/ws/dynamechs/dm/dmObject.h /users/ws/dynamechs/dm/dmLink.h
Segments.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
Segments.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
Segments.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
Segments.o: /usr/include/CC/type_traits.h /usr/include/limits.h
Segments.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
Segments.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
Segments.o: /usr/include/stdio.h /usr/include/assert.h
Segments.o: /usr/include/CC/stl_construct.h
Segments.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
Segments.o: /usr/include/CC/stl_bvector.h
Segments.o: /users/ws/dynamechs/dm/dmRevoluteLink.h
Segments.o: /users/ws/dynamechs/dm/dmMDHLink.h
Segments.o: /users/ws/dynamechs/dm/dmActuator.h Simulation.h
Segments.o: SegmentParameters.h StepController.h Controller.h
Segments.o: ModifiedContactModel.h /users/ws/dynamechs/dm/dmEnvironment.h
Segments.o: ./LoggedForce.h MuscleModel.h
ParameterFile.o: /usr/include/stdio.h /usr/include/standards.h
ParameterFile.o: /usr/include/sgidefs.h /usr/include/string.h
ParameterFile.o: /usr/include/sys/stat.h /usr/include/sys/types.h
ParameterFile.o: /usr/include/sys/timespec.h /usr/include/stdlib.h
ParameterFile.o: ParameterFile.h
