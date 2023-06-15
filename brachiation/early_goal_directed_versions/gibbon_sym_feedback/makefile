
include makevars
include makefile.sources

DYN_DIR = ../dynamechs_3.0/
INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL -I/usr/X11R6/include -I/usr/X11R6/include/GL -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
LIBS = $(DYN_DIR)dm/libdm.a $(DYN_DIR)dmGL/libdmGL.a $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dm/libdm.a 
OPENGL_LIBS = -L/usr/X11R6/lib -L/usr/share/src/OpenGL/toolkits/lib32 -lglut -lGLU -lGL -lX11 -ldl -lXmu

gibbon: $(OBJS)
	$(CXX) -o $@ $^ $(LIBS) -lm $(OPENGL_LIBS)

clean:
	$(RM) gibbon
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) $(SRCS)

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

Bar.o: /usr/include/math.h /usr/include/sgidefs.h /usr/include/standards.h
Bar.o: ../dynamechs_3.0/dmu/glLoadModels.h ../dynamechs_3.0/dmu/dmu.h
Bar.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
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
ForceList.o: /usr/include/CC/stl_bvector.h ../dynamechs_3.0/dm/dmForce.h
ForceList.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iomanip.h
ForceList.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
ForceList.o: /usr/include/math.h ../dynamechs_3.0/dm/dmObject.h ForceList.h
LoggedForce.o: /usr/include/stdio.h /usr/include/standards.h
LoggedForce.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
LoggedForce.o: /usr/include/memory.h /usr/include/stddef.h
LoggedForce.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
LoggedForce.o: /usr/include/CC/fstream.h /usr/include/assert.h
LoggedForce.o: /usr/include/string.h ../dynamechs_3.0/dm/dm.h
LoggedForce.o: /usr/include/math.h /usr/include/stdlib.h
LoggedForce.o: ../dynamechs_3.0/dm/dmForce.h ../dynamechs_3.0/dm/dmObject.h
LoggedForce.o: LoggedForce.h
SinController.o: /usr/include/math.h /usr/include/sgidefs.h
SinController.o: /usr/include/standards.h SinController.h Controller.h
SinController.o: ../dynamechs_3.0/dm/dmObject.h ../dynamechs_3.0/dm/dm.h
SinController.o: /usr/include/CC/iostream.h /usr/include/memory.h
SinController.o: /usr/include/stddef.h /usr/include/CC/iomanip.h
SinController.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
SinController.o: /usr/include/stdlib.h /usr/include/string.h
Controller.o: /usr/include/CC/iostream.h /usr/include/memory.h
Controller.o: /usr/include/stddef.h /usr/include/sgidefs.h
Controller.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Controller.o: /usr/include/CC/fstream.h Controller.h
Controller.o: ../dynamechs_3.0/dm/dmObject.h ../dynamechs_3.0/dm/dm.h
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
FBGripModel.o: /usr/include/CC/iostream.h /usr/include/memory.h
FBGripModel.o: /usr/include/stddef.h /usr/include/sgidefs.h
FBGripModel.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
FBGripModel.o: /usr/include/CC/fstream.h /usr/include/assert.h
FBGripModel.o: ../dynamechs_3.0/dm/dm.h /usr/include/math.h
FBGripModel.o: /usr/include/standards.h /usr/include/stdlib.h
FBGripModel.o: /usr/include/string.h Bar.h /usr/include/GL/gl.h Util.h
FBGripModel.o: FBGripModel.h LoggedForce.h ../dynamechs_3.0/dm/dmForce.h
FBGripModel.o: ../dynamechs_3.0/dm/dmObject.h Sensor.h Controller.h
FBGripModel.o: ForceList.h /usr/include/CC/vector
FBGripModel.o: /usr/include/CC/stl_range_errors.h
FBGripModel.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
FBGripModel.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
FBGripModel.o: /usr/include/CC/type_traits.h /usr/include/limits.h
FBGripModel.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
FBGripModel.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
FBGripModel.o: /usr/include/stdio.h /usr/include/CC/stl_construct.h
FBGripModel.o: /usr/include/CC/stl_uninitialized.h
FBGripModel.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
FBGripModel.o: Simulation.h
FBMuscleModel.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
FBMuscleModel.o: /usr/include/memory.h /usr/include/stddef.h
FBMuscleModel.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
FBMuscleModel.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
FBMuscleModel.o: /usr/include/math.h /usr/include/standards.h
FBMuscleModel.o: /usr/include/stdlib.h /usr/include/string.h
FBMuscleModel.o: ../dynamechs_3.0/dmu/dmu.h ../dynamechs_3.0/dm/dmForce.h
FBMuscleModel.o: ../dynamechs_3.0/dm/dmObject.h
FBMuscleModel.o: ../dynamechs_3.0/dm/dmArticulation.h
FBMuscleModel.o: ../dynamechs_3.0/dm/dmLink.h ../dynamechs_3.0/dm/dmSystem.h
FBMuscleModel.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
FBMuscleModel.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
FBMuscleModel.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
FBMuscleModel.o: /usr/include/CC/type_traits.h /usr/include/limits.h
FBMuscleModel.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
FBMuscleModel.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
FBMuscleModel.o: /usr/include/stdio.h /usr/include/assert.h
FBMuscleModel.o: /usr/include/CC/stl_construct.h
FBMuscleModel.o: /usr/include/CC/stl_uninitialized.h
FBMuscleModel.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
FBMuscleModel.o: /usr/include/GL/gl.h FBMuscleModel.h LoggedForce.h Sensor.h
FBMuscleModel.o: Controller.h Util.h ForceList.h Simulation.h
Util.o: /usr/include/math.h /usr/include/sgidefs.h /usr/include/standards.h
Util.o: Util.h
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
ControllerList.o: Controller.h ../dynamechs_3.0/dm/dmObject.h
ControllerList.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iomanip.h
ControllerList.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
ControllerList.o: /usr/include/math.h ControllerList.h
LoadFile.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
LoadFile.o: /usr/include/memory.h /usr/include/stddef.h
LoadFile.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
LoadFile.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
LoadFile.o: /usr/include/math.h /usr/include/standards.h
LoadFile.o: /usr/include/stdlib.h /usr/include/string.h /usr/include/GL/gl.h
LoadFile.o: ../dynamechs_3.0/dm/dmObject.h ../dynamechs_3.0/dm/dmSystem.h
LoadFile.o: ../dynamechs_3.0/dm/dmArticulation.h ../dynamechs_3.0/dm/dmLink.h
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
LoadFile.o: ../dynamechs_3.0/dm/dmZScrewTxLink.h
LoadFile.o: ../dynamechs_3.0/dm/dmMDHLink.h ../dynamechs_3.0/dm/dmRigidBody.h
LoadFile.o: ../dynamechs_3.0/dm/dmForce.h ../dynamechs_3.0/dm/dmActuator.h
LoadFile.o: ../dynamechs_3.0/dm/dmRevoluteLink.h
LoadFile.o: ../dynamechs_3.0/dm/dmPrismaticLink.h
LoadFile.o: ../dynamechs_3.0/dm/dmSphericalLink.h
LoadFile.o: ../dynamechs_3.0/dm/dmMobileBaseLink.h
LoadFile.o: ../dynamechs_3.0/dm/dmRevDCMotor.h
LoadFile.o: ../dynamechs_3.0/dm/dmContactModel.h
LoadFile.o: ../dynamechs_3.0/dm/dmEnvironment.h
LoadFile.o: ../dynamechs_3.0/dmu/dmLoadFile.h ../dynamechs_3.0/dmu/dmu.h
LoadFile.o: ../dynamechs_3.0/dmu/glLoadModels.h LoadFile.h Controller.h
LoadFile.o: SinController.h FBGripModel.h LoggedForce.h Sensor.h
LoadFile.o: FBMuscleModel.h Simulation.h IntegratingController.h
Simulation.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
Simulation.o: /usr/include/memory.h /usr/include/stddef.h
Simulation.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
Simulation.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Simulation.o: /usr/include/math.h /usr/include/standards.h
Simulation.o: /usr/include/stdlib.h /usr/include/string.h
Simulation.o: ../dynamechs_3.0/dm/dmSystem.h ../dynamechs_3.0/dm/dmObject.h
Simulation.o: ../dynamechs_3.0/dm/dmEnvironment.h
Simulation.o: ../dynamechs_3.0/dm/dmArticulation.h
Simulation.o: ../dynamechs_3.0/dm/dmLink.h /usr/include/CC/vector
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
Simulation.o: ../dynamechs_3.0/dm/dmIntegrator.h
Simulation.o: ../dynamechs_3.0/dm/dmIntegEuler.h ../dynamechs_3.0/dmu/dmu.h
Simulation.o: Bar.h /usr/include/GL/gl.h LoadFile.h ForceList.h
Simulation.o: ControllerList.h Sensor.h SensorList.h Controller.h
Simulation.o: IntegratingController.h Simulation.h
main.o: /usr/include/assert.h
main.o: /usr/share/src/OpenGL/toolkits/include/GL/glut.h /usr/include/GL/gl.h
main.o: /usr/include/GL/glu.h ../dynamechs_3.0/dmGL/dmGLTime.h
main.o: ../dynamechs_3.0/dmGL/dmGL.h /usr/include/CC/iostream.h
main.o: /usr/include/memory.h /usr/include/stddef.h /usr/include/sgidefs.h
main.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
main.o: /usr/include/GL/gl.h /usr/include/GL/glu.h /usr/include/math.h
main.o: /usr/include/standards.h /usr/include/stdlib.h
main.o: /usr/include/sys/time.h ../dynamechs_3.0/dmGL/dmGLMouse.h
main.o: ../dynamechs_3.0/dmGL/dmGLPolarCamera_zup.h
main.o: ../dynamechs_3.0/dmGL/dmGLPolarCamera.h
main.o: ../dynamechs_3.0/dmGL/dmGLCamera.h
main.o: ../dynamechs_3.0/dm/dmEnvironment.h ../dynamechs_3.0/dm/dm.h
main.o: /usr/include/CC/fstream.h /usr/include/string.h
main.o: ../dynamechs_3.0/dm/dmObject.h ../dynamechs_3.0/dm/dmArticulation.h
main.o: ../dynamechs_3.0/dm/dmLink.h ../dynamechs_3.0/dm/dmSystem.h
main.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
main.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
main.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
main.o: /usr/include/CC/type_traits.h /usr/include/limits.h
main.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
main.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
main.o: /usr/include/stdio.h /usr/include/CC/stl_construct.h
main.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
main.o: /usr/include/CC/stl_bvector.h Bar.h ForceList.h Simulation.h
main.o: Controller.h ControllerList.h FBGripModel.h LoggedForce.h
main.o: ../dynamechs_3.0/dm/dmForce.h Sensor.h
Sensor.o: Sensor.h ../dynamechs_3.0/dm/dmObject.h ../dynamechs_3.0/dm/dm.h
Sensor.o: /usr/include/CC/iostream.h /usr/include/memory.h
Sensor.o: /usr/include/stddef.h /usr/include/sgidefs.h
Sensor.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Sensor.o: /usr/include/CC/fstream.h /usr/include/math.h
Sensor.o: /usr/include/standards.h /usr/include/stdlib.h
Sensor.o: /usr/include/string.h SensorList.h /usr/include/CC/vector
Sensor.o: /usr/include/CC/stl_range_errors.h /usr/include/CC/stl_config.h
Sensor.o: /usr/include/CC/stl_algobase.h /usr/include/CC/stl_relops.h
Sensor.o: /usr/include/CC/stl_pair.h /usr/include/CC/type_traits.h
Sensor.o: /usr/include/limits.h /usr/include/CC/new.h
Sensor.o: /usr/include/CC/stl_iterator_base.h /usr/include/CC/stl_iterator.h
Sensor.o: /usr/include/CC/stl_alloc.h /usr/include/stdio.h
Sensor.o: /usr/include/assert.h /usr/include/CC/stl_construct.h
Sensor.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
Sensor.o: /usr/include/CC/stl_bvector.h Simulation.h /usr/include/GL/gl.h
SensorList.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
SensorList.o: /usr/include/CC/stl_config.h /usr/include/standards.h
SensorList.o: /usr/include/CC/stl_algobase.h /usr/include/CC/stl_relops.h
SensorList.o: /usr/include/CC/stl_pair.h /usr/include/CC/type_traits.h
SensorList.o: /usr/include/string.h /usr/include/limits.h
SensorList.o: /usr/include/sgidefs.h /usr/include/stdlib.h
SensorList.o: /usr/include/stddef.h /usr/include/CC/new.h
SensorList.o: /usr/include/CC/iostream.h /usr/include/memory.h
SensorList.o: /usr/include/CC/stl_iterator_base.h
SensorList.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
SensorList.o: /usr/include/stdio.h /usr/include/assert.h
SensorList.o: /usr/include/CC/stl_construct.h
SensorList.o: /usr/include/CC/stl_uninitialized.h
SensorList.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
SensorList.o: Sensor.h ../dynamechs_3.0/dm/dmObject.h
SensorList.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iomanip.h
SensorList.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
SensorList.o: /usr/include/math.h SensorList.h
IntegratingController.o: /usr/include/assert.h IntegratingController.h
IntegratingController.o: Controller.h ../dynamechs_3.0/dm/dmObject.h
IntegratingController.o: ../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
IntegratingController.o: /usr/include/memory.h /usr/include/stddef.h
IntegratingController.o: /usr/include/sgidefs.h /usr/include/CC/iomanip.h
IntegratingController.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
IntegratingController.o: /usr/include/math.h /usr/include/standards.h
IntegratingController.o: /usr/include/stdlib.h /usr/include/string.h
IntegratingController.o: /usr/include/CC/vector
IntegratingController.o: /usr/include/CC/stl_range_errors.h
IntegratingController.o: /usr/include/CC/stl_config.h
IntegratingController.o: /usr/include/CC/stl_algobase.h
IntegratingController.o: /usr/include/CC/stl_relops.h
IntegratingController.o: /usr/include/CC/stl_pair.h
IntegratingController.o: /usr/include/CC/type_traits.h /usr/include/limits.h
IntegratingController.o: /usr/include/CC/new.h
IntegratingController.o: /usr/include/CC/stl_iterator_base.h
IntegratingController.o: /usr/include/CC/stl_iterator.h
IntegratingController.o: /usr/include/CC/stl_alloc.h /usr/include/stdio.h
IntegratingController.o: /usr/include/CC/stl_construct.h
IntegratingController.o: /usr/include/CC/stl_uninitialized.h
IntegratingController.o: /usr/include/CC/stl_vector.h
IntegratingController.o: /usr/include/CC/stl_bvector.h Sensor.h
