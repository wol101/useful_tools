# makefile for the subprocess objective option

# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -DUSE_OLD_STREAMS -ptused -DINLINE_OBJECTIVE
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


DYN_DIR = ../../dynamechs_3.0/
INC_DIRS= -I. -I../../galib245/ -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL \
	-I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
DYN_LIBS = $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dmGL/libdmGL.a \
	$(DYN_DIR)dm/libdm.a 
LIBS = -lm 
OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 \
	-lglut -lGLU -lGL -lX11 -lXmu
GA_LIBS = ../../galib245/ga/libga.a

all: direct_ga ga objective
	
direct_ga: \
Bar.o                  Simulation.o           Fitness.o\
Controller.o           GripModel.o            SinController.o \
ControllerList.o       LoadFile.o             SquareController.o\
LoggedForce.o          Util.o \
ForceList.o            MuscleModel.o          strtok.o\
GA.o                   Objective.o
	$(CXX) -o $@ $^ $(GA_LIBS) $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)
	$(CP) $@ ../bin 

objective: \
Bar.o                  Simulation.o           Fitness.o\
Controller.o           GripModel.o            SinController.o \
ControllerList.o       LoadFile.o             ObjectiveMain.o\
LoggedForce.o          Util.o                 SquareController.o\
ForceList.o            MuscleModel.o          strtok.o
	$(CXX) -o $@ $^ $(GA_LIBS) $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)
	$(CP) $@ ../bin 

ga: \
GANotInline.o        Util.o              ObjectiveNotInline.o
	$(CXX) -o $@ $^ $(GA_LIBS) $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)
	$(CP) $@ ../bin 

GANotInline.o: GA.cc
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -UINLINE_OBJECTIVE -c -o $@ $^
	
ObjectiveNotInline.o: Objective.cc
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -UINLINE_OBJECTIVE -c -o $@ $^
	
clean:
	$(RM) ga objective direct_ga
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) \
Bar.cc                  GA.cc                   ObjectiveMain.cc\
Controller.cc           GripModel.cc            Simulation.cc\
ControllerList.cc       LoadFile.cc             SinController.cc\
Fitness.cc              LoggedForce.cc          Objective.cc\
ForceList.cc            MuscleModel.cc          Util.cc\
SquareController.cc

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

Bar.o: /usr/include/math.h /usr/include/sgidefs.h /usr/include/standards.h
Bar.o: ../../dynamechs_3.0/dmu/glLoadModels.h ../../dynamechs_3.0/dmu/dmu.h
Bar.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
Bar.o: /usr/include/memory.h /usr/include/stddef.h /usr/include/CC/iomanip.h
Bar.o: /usr/include/CC/generic.h /usr/include/CC/fstream.h
Bar.o: /usr/include/stdlib.h /usr/include/string.h /usr/include/GL/gl.h Bar.h
GA.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sgidefs.h
GA.o: /usr/include/CC/iostream.h /usr/include/memory.h /usr/include/stddef.h
GA.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
GA.o: /usr/include/CC/fstream.h /usr/include/assert.h
GA.o: /usr/include/CC/exception ../../galib245/ga/ga.h
GA.o: ../../galib245/ga/gaconfig.h /usr/include/CC/iostream
GA.o: /usr/include/CC/iosfwd /usr/include/CC/stl_config.h
GA.o: /usr/include/wchar.h /usr/include/locale_attr.h /usr/include/ctype.h
GA.o: /usr/include/time.h /usr/include/CC/functional
GA.o: /usr/include/CC/stl_function.h /usr/include/CC/char_traits.h
GA.o: /usr/include/string.h /usr/include/CC/istream /usr/include/CC/ostream
GA.o: /usr/include/CC/ios /usr/include/CC/stl_string_fwd.h
GA.o: /usr/include/CC/stl_alloc.h /usr/include/stdlib.h
GA.o: /usr/include/CC/stl_exception.h /usr/include/CC/utility
GA.o: /usr/include/CC/stl_relops.h /usr/include/CC/stl_pair.h
GA.o: /usr/include/CC/stl_locale.h /usr/include/CC/typeinfo
GA.o: /usr/include/CC/stl_threads.h /usr/include/mutex.h
GA.o: /usr/include/CC/string /usr/include/CC/stdexcept
GA.o: /usr/include/CC/stl_iterator_base.h /usr/include/CC/memory
GA.o: /usr/include/CC/stl_algobase.h /usr/include/CC/type_traits.h
GA.o: /usr/include/limits.h /usr/include/CC/new.h
GA.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_construct.h
GA.o: /usr/include/CC/stl_tempbuf.h /usr/include/CC/stl_uninitialized.h
GA.o: /usr/include/CC/stl_raw_storage_iter.h /usr/include/CC/algorithm
GA.o: /usr/include/CC/stl_algo.h /usr/include/CC/stl_heap.h
GA.o: /usr/include/CC/stl_hash_fun.h /usr/include/CC/stl_ctype.h
GA.o: /usr/include/CC/c_locale.h /usr/include/CC/streambuf
GA.o: /usr/include/CC/stl_stdio_file.h /usr/include/CC/stl_numeric_facets.h
GA.o: /usr/include/CC/limits /usr/include/float.h /usr/include/CC/fstream
GA.o: /usr/include/CC/stl_codecvt.h ../../galib245/ga/GASimpleGA.h
GA.o: ../../galib245/ga/GABaseGA.h ../../galib245/ga/gaid.h
GA.o: ../../galib245/ga/GAParameter.h ../../galib245/ga/gatypes.h
GA.o: ../../galib245/ga/GAStatistics.h ../../galib245/ga/GAGenome.h
GA.o: ../../galib245/ga/gaerror.h ../../galib245/ga/GAEvalData.h
GA.o: ../../galib245/ga/GAPopulation.h ../../galib245/ga/GASelector.h
GA.o: ../../galib245/ga/GAScaling.h ../../galib245/ga/GASStateGA.h
GA.o: ../../galib245/ga/GAIncGA.h ../../galib245/ga/GADemeGA.h
GA.o: ../../galib245/ga/GADCrowdingGA.h ../../galib245/ga/GA1DBinStrGenome.h
GA.o: ../../galib245/ga/GABinStr.h ../../galib245/ga/garandom.h
GA.o: ../../galib245/ga/GA2DBinStrGenome.h
GA.o: ../../galib245/ga/GA3DBinStrGenome.h
GA.o: ../../galib245/ga/GABin2DecGenome.h ../../galib245/ga/gabincvt.h
GA.o: ../../galib245/ga/GA1DArrayGenome.h ../../galib245/ga/GAArray.h
GA.o: ../../galib245/ga/GAAllele.h ../../galib245/ga/GA2DArrayGenome.h
GA.o: ../../galib245/ga/GA3DArrayGenome.h ../../galib245/ga/GAListGenome.h
GA.o: ../../galib245/ga/GAList.h ../../galib245/ga/GAListBASE.h
GA.o: ../../galib245/ga/GANode.h ../../galib245/ga/GATreeGenome.h
GA.o: ../../galib245/ga/GATree.h ../../galib245/ga/GATreeBASE.h
GA.o: ../../galib245/ga/GARealGenome.h ../../galib245/ga/GARealGenome.C
GA.o: Util.h /usr/include/math.h
ObjectiveMain.o: /usr/include/stdio.h /usr/include/standards.h
ObjectiveMain.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
ObjectiveMain.o: /usr/include/memory.h /usr/include/stddef.h
ObjectiveMain.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
ObjectiveMain.o: /usr/include/CC/fstream.h /usr/include/assert.h
ObjectiveMain.o: ../../galib245/ga/ga.h ../../galib245/ga/gaconfig.h
ObjectiveMain.o: /usr/include/CC/iostream /usr/include/CC/iosfwd
ObjectiveMain.o: /usr/include/CC/stl_config.h /usr/include/wchar.h
ObjectiveMain.o: /usr/include/locale_attr.h /usr/include/ctype.h
ObjectiveMain.o: /usr/include/time.h /usr/include/CC/functional
ObjectiveMain.o: /usr/include/CC/stl_function.h /usr/include/CC/char_traits.h
ObjectiveMain.o: /usr/include/string.h /usr/include/CC/istream
ObjectiveMain.o: /usr/include/CC/ostream /usr/include/CC/ios
ObjectiveMain.o: /usr/include/CC/stl_string_fwd.h /usr/include/CC/stl_alloc.h
ObjectiveMain.o: /usr/include/stdlib.h /usr/include/CC/stl_exception.h
ObjectiveMain.o: /usr/include/CC/utility /usr/include/CC/stl_relops.h
ObjectiveMain.o: /usr/include/CC/stl_pair.h /usr/include/CC/stl_locale.h
ObjectiveMain.o: /usr/include/CC/typeinfo /usr/include/CC/exception
ObjectiveMain.o: /usr/include/CC/stl_threads.h /usr/include/mutex.h
ObjectiveMain.o: /usr/include/CC/string /usr/include/CC/stdexcept
ObjectiveMain.o: /usr/include/CC/stl_iterator_base.h /usr/include/CC/memory
ObjectiveMain.o: /usr/include/CC/stl_algobase.h /usr/include/CC/type_traits.h
ObjectiveMain.o: /usr/include/limits.h /usr/include/CC/new.h
ObjectiveMain.o: /usr/include/CC/stl_iterator.h
ObjectiveMain.o: /usr/include/CC/stl_construct.h
ObjectiveMain.o: /usr/include/CC/stl_tempbuf.h
ObjectiveMain.o: /usr/include/CC/stl_uninitialized.h
ObjectiveMain.o: /usr/include/CC/stl_raw_storage_iter.h
ObjectiveMain.o: /usr/include/CC/algorithm /usr/include/CC/stl_algo.h
ObjectiveMain.o: /usr/include/CC/stl_heap.h /usr/include/CC/stl_hash_fun.h
ObjectiveMain.o: /usr/include/CC/stl_ctype.h /usr/include/CC/c_locale.h
ObjectiveMain.o: /usr/include/CC/streambuf /usr/include/CC/stl_stdio_file.h
ObjectiveMain.o: /usr/include/CC/stl_numeric_facets.h /usr/include/CC/limits
ObjectiveMain.o: /usr/include/float.h /usr/include/CC/fstream
ObjectiveMain.o: /usr/include/CC/stl_codecvt.h ../../galib245/ga/GASimpleGA.h
ObjectiveMain.o: ../../galib245/ga/GABaseGA.h ../../galib245/ga/gaid.h
ObjectiveMain.o: ../../galib245/ga/GAParameter.h ../../galib245/ga/gatypes.h
ObjectiveMain.o: ../../galib245/ga/GAStatistics.h
ObjectiveMain.o: ../../galib245/ga/GAGenome.h ../../galib245/ga/gaerror.h
ObjectiveMain.o: ../../galib245/ga/GAEvalData.h
ObjectiveMain.o: ../../galib245/ga/GAPopulation.h
ObjectiveMain.o: ../../galib245/ga/GASelector.h ../../galib245/ga/GAScaling.h
ObjectiveMain.o: ../../galib245/ga/GASStateGA.h ../../galib245/ga/GAIncGA.h
ObjectiveMain.o: ../../galib245/ga/GADemeGA.h
ObjectiveMain.o: ../../galib245/ga/GADCrowdingGA.h
ObjectiveMain.o: ../../galib245/ga/GA1DBinStrGenome.h
ObjectiveMain.o: ../../galib245/ga/GABinStr.h ../../galib245/ga/garandom.h
ObjectiveMain.o: ../../galib245/ga/GA2DBinStrGenome.h
ObjectiveMain.o: ../../galib245/ga/GA3DBinStrGenome.h
ObjectiveMain.o: ../../galib245/ga/GABin2DecGenome.h
ObjectiveMain.o: ../../galib245/ga/gabincvt.h
ObjectiveMain.o: ../../galib245/ga/GA1DArrayGenome.h
ObjectiveMain.o: ../../galib245/ga/GAArray.h ../../galib245/ga/GAAllele.h
ObjectiveMain.o: ../../galib245/ga/GA2DArrayGenome.h
ObjectiveMain.o: ../../galib245/ga/GA3DArrayGenome.h
ObjectiveMain.o: ../../galib245/ga/GAListGenome.h ../../galib245/ga/GAList.h
ObjectiveMain.o: ../../galib245/ga/GAListBASE.h ../../galib245/ga/GANode.h
ObjectiveMain.o: ../../galib245/ga/GATreeGenome.h ../../galib245/ga/GATree.h
ObjectiveMain.o: ../../galib245/ga/GATreeBASE.h
ObjectiveMain.o: ../../galib245/ga/GARealGenome.h
ObjectiveMain.o: ../../galib245/ga/GARealGenome.C Fitness.h
ObjectiveMain.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
ObjectiveMain.o: /usr/include/CC/stl_vector.h /usr/include/CC/stl_bvector.h
ObjectiveMain.o: Util.h /usr/include/math.h
Controller.o: /usr/include/CC/iostream.h /usr/include/memory.h
Controller.o: /usr/include/stddef.h /usr/include/sgidefs.h
Controller.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Controller.o: /usr/include/CC/fstream.h Controller.h
Controller.o: ../../dynamechs_3.0/dm/dmObject.h ../../dynamechs_3.0/dm/dm.h
Controller.o: /usr/include/math.h /usr/include/standards.h
Controller.o: /usr/include/stdlib.h /usr/include/string.h ControllerList.h
Controller.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
Controller.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
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
GripModel.o: /usr/include/CC/type_traits.h /usr/include/limits.h
GripModel.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
GripModel.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
GripModel.o: /usr/include/stdio.h /usr/include/CC/stl_construct.h
GripModel.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
GripModel.o: /usr/include/CC/stl_bvector.h Simulation.h
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
Simulation.o: /usr/include/CC/stl_algobase.h /usr/include/CC/type_traits.h
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
ControllerList.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
ControllerList.o: /usr/include/CC/stl_config.h /usr/include/standards.h
ControllerList.o: /usr/include/CC/stl_algobase.h
ControllerList.o: /usr/include/CC/type_traits.h /usr/include/string.h
ControllerList.o: /usr/include/limits.h /usr/include/sgidefs.h
ControllerList.o: /usr/include/stdlib.h /usr/include/stddef.h
ControllerList.o: /usr/include/CC/new.h /usr/include/CC/iostream.h
ControllerList.o: /usr/include/memory.h /usr/include/CC/stl_iterator_base.h
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
LoadFile.o: /usr/include/CC/stl_algobase.h /usr/include/CC/type_traits.h
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
LoadFile.o: Controller.h SquareController.h GripModel.h LoggedForce.h
LoadFile.o: MuscleModel.h Simulation.h
SinController.o: /usr/include/math.h /usr/include/sgidefs.h
SinController.o: /usr/include/standards.h SinController.h Controller.h
SinController.o: ../../dynamechs_3.0/dm/dmObject.h
SinController.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
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
Fitness.o: /usr/include/CC/type_traits.h /usr/include/string.h
Fitness.o: /usr/include/limits.h /usr/include/stdlib.h /usr/include/CC/new.h
Fitness.o: /usr/include/CC/stl_iterator_base.h /usr/include/CC/stl_iterator.h
Fitness.o: /usr/include/CC/stl_alloc.h /usr/include/CC/stl_construct.h
Fitness.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
Fitness.o: /usr/include/CC/stl_bvector.h
Fitness.o: /usr/share/src/OpenGL/toolkits/include/GL/glut.h
Fitness.o: /usr/include/GL/gl.h /usr/include/GL/glu.h
Fitness.o: ../../dynamechs_3.0/dmGL/dmGLTime.h
Fitness.o: ../../dynamechs_3.0/dmGL/dmGL.h /usr/include/GL/gl.h
Fitness.o: /usr/include/GL/glu.h /usr/include/math.h /usr/include/sys/time.h
Fitness.o: ../../dynamechs_3.0/dmGL/dmGLMouse.h
Fitness.o: ../../dynamechs_3.0/dmGL/dmGLPolarCamera_zup.h
Fitness.o: ../../dynamechs_3.0/dmGL/dmGLPolarCamera.h
Fitness.o: ../../dynamechs_3.0/dmGL/dmGLCamera.h
Fitness.o: ../../dynamechs_3.0/dm/dmEnvironment.h ../../dynamechs_3.0/dm/dm.h
Fitness.o: ../../dynamechs_3.0/dm/dmObject.h
Fitness.o: ../../dynamechs_3.0/dm/dmArticulation.h
Fitness.o: ../../dynamechs_3.0/dm/dmLink.h ../../dynamechs_3.0/dm/dmSystem.h
Fitness.o: ../../dynamechs_3.0/dm/dmMobileBaseLink.h
Fitness.o: ../../dynamechs_3.0/dm/dmRigidBody.h
Fitness.o: ../../dynamechs_3.0/dm/dmForce.h ../../dynamechs_3.0/dmu/dmu.h
Fitness.o: ../../galib245/ga/ga.h ../../galib245/ga/gaconfig.h
Fitness.o: /usr/include/CC/iostream /usr/include/CC/iosfwd
Fitness.o: /usr/include/wchar.h /usr/include/locale_attr.h
Fitness.o: /usr/include/ctype.h /usr/include/time.h
Fitness.o: /usr/include/CC/functional /usr/include/CC/stl_function.h
Fitness.o: /usr/include/CC/char_traits.h /usr/include/CC/istream
Fitness.o: /usr/include/CC/ostream /usr/include/CC/ios
Fitness.o: /usr/include/CC/stl_string_fwd.h /usr/include/CC/stl_exception.h
Fitness.o: /usr/include/CC/utility /usr/include/CC/stl_relops.h
Fitness.o: /usr/include/CC/stl_pair.h /usr/include/CC/stl_locale.h
Fitness.o: /usr/include/CC/typeinfo /usr/include/CC/exception
Fitness.o: /usr/include/CC/stl_threads.h /usr/include/mutex.h
Fitness.o: /usr/include/CC/string /usr/include/CC/stdexcept
Fitness.o: /usr/include/CC/memory /usr/include/CC/stl_tempbuf.h
Fitness.o: /usr/include/CC/stl_raw_storage_iter.h /usr/include/CC/algorithm
Fitness.o: /usr/include/CC/stl_algo.h /usr/include/CC/stl_heap.h
Fitness.o: /usr/include/CC/stl_hash_fun.h /usr/include/CC/stl_ctype.h
Fitness.o: /usr/include/CC/c_locale.h /usr/include/CC/streambuf
Fitness.o: /usr/include/CC/stl_stdio_file.h
Fitness.o: /usr/include/CC/stl_numeric_facets.h /usr/include/CC/limits
Fitness.o: /usr/include/float.h /usr/include/CC/fstream
Fitness.o: /usr/include/CC/stl_codecvt.h ../../galib245/ga/GASimpleGA.h
Fitness.o: ../../galib245/ga/GABaseGA.h ../../galib245/ga/gaid.h
Fitness.o: ../../galib245/ga/GAParameter.h ../../galib245/ga/gatypes.h
Fitness.o: ../../galib245/ga/GAStatistics.h ../../galib245/ga/GAGenome.h
Fitness.o: ../../galib245/ga/gaerror.h ../../galib245/ga/GAEvalData.h
Fitness.o: ../../galib245/ga/GAPopulation.h ../../galib245/ga/GASelector.h
Fitness.o: ../../galib245/ga/GAScaling.h ../../galib245/ga/GASStateGA.h
Fitness.o: ../../galib245/ga/GAIncGA.h ../../galib245/ga/GADemeGA.h
Fitness.o: ../../galib245/ga/GADCrowdingGA.h
Fitness.o: ../../galib245/ga/GA1DBinStrGenome.h ../../galib245/ga/GABinStr.h
Fitness.o: ../../galib245/ga/garandom.h ../../galib245/ga/GA2DBinStrGenome.h
Fitness.o: ../../galib245/ga/GA3DBinStrGenome.h
Fitness.o: ../../galib245/ga/GABin2DecGenome.h ../../galib245/ga/gabincvt.h
Fitness.o: ../../galib245/ga/GA1DArrayGenome.h ../../galib245/ga/GAArray.h
Fitness.o: ../../galib245/ga/GAAllele.h ../../galib245/ga/GA2DArrayGenome.h
Fitness.o: ../../galib245/ga/GA3DArrayGenome.h
Fitness.o: ../../galib245/ga/GAListGenome.h ../../galib245/ga/GAList.h
Fitness.o: ../../galib245/ga/GAListBASE.h ../../galib245/ga/GANode.h
Fitness.o: ../../galib245/ga/GATreeGenome.h ../../galib245/ga/GATree.h
Fitness.o: ../../galib245/ga/GATreeBASE.h ../../galib245/ga/GARealGenome.h
Fitness.o: Bar.h ForceList.h Simulation.h Controller.h ControllerList.h
Fitness.o: GripModel.h LoggedForce.h Util.h Fitness.h
LoggedForce.o: /usr/include/stdio.h /usr/include/standards.h
LoggedForce.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
LoggedForce.o: /usr/include/memory.h /usr/include/stddef.h
LoggedForce.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
LoggedForce.o: /usr/include/CC/fstream.h /usr/include/assert.h
LoggedForce.o: /usr/include/string.h ../../dynamechs_3.0/dm/dm.h
LoggedForce.o: /usr/include/math.h /usr/include/stdlib.h
LoggedForce.o: ../../dynamechs_3.0/dm/dmForce.h
LoggedForce.o: ../../dynamechs_3.0/dm/dmObject.h LoggedForce.h
Objective.o: /usr/include/stdio.h /usr/include/standards.h
Objective.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
Objective.o: /usr/include/memory.h /usr/include/stddef.h
Objective.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Objective.o: /usr/include/CC/fstream.h /usr/include/assert.h
Objective.o: ../../galib245/ga/ga.h ../../galib245/ga/gaconfig.h
Objective.o: /usr/include/CC/iostream /usr/include/CC/iosfwd
Objective.o: /usr/include/CC/stl_config.h /usr/include/wchar.h
Objective.o: /usr/include/locale_attr.h /usr/include/ctype.h
Objective.o: /usr/include/time.h /usr/include/CC/functional
Objective.o: /usr/include/CC/stl_function.h /usr/include/CC/char_traits.h
Objective.o: /usr/include/string.h /usr/include/CC/istream
Objective.o: /usr/include/CC/ostream /usr/include/CC/ios
Objective.o: /usr/include/CC/stl_string_fwd.h /usr/include/CC/stl_alloc.h
Objective.o: /usr/include/stdlib.h /usr/include/CC/stl_exception.h
Objective.o: /usr/include/CC/utility /usr/include/CC/stl_relops.h
Objective.o: /usr/include/CC/stl_pair.h /usr/include/CC/stl_locale.h
Objective.o: /usr/include/CC/typeinfo /usr/include/CC/exception
Objective.o: /usr/include/CC/stl_threads.h /usr/include/mutex.h
Objective.o: /usr/include/CC/string /usr/include/CC/stdexcept
Objective.o: /usr/include/CC/stl_iterator_base.h /usr/include/CC/memory
Objective.o: /usr/include/CC/stl_algobase.h /usr/include/CC/type_traits.h
Objective.o: /usr/include/limits.h /usr/include/CC/new.h
Objective.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_construct.h
Objective.o: /usr/include/CC/stl_tempbuf.h
Objective.o: /usr/include/CC/stl_uninitialized.h
Objective.o: /usr/include/CC/stl_raw_storage_iter.h /usr/include/CC/algorithm
Objective.o: /usr/include/CC/stl_algo.h /usr/include/CC/stl_heap.h
Objective.o: /usr/include/CC/stl_hash_fun.h /usr/include/CC/stl_ctype.h
Objective.o: /usr/include/CC/c_locale.h /usr/include/CC/streambuf
Objective.o: /usr/include/CC/stl_stdio_file.h
Objective.o: /usr/include/CC/stl_numeric_facets.h /usr/include/CC/limits
Objective.o: /usr/include/float.h /usr/include/CC/fstream
Objective.o: /usr/include/CC/stl_codecvt.h ../../galib245/ga/GASimpleGA.h
Objective.o: ../../galib245/ga/GABaseGA.h ../../galib245/ga/gaid.h
Objective.o: ../../galib245/ga/GAParameter.h ../../galib245/ga/gatypes.h
Objective.o: ../../galib245/ga/GAStatistics.h ../../galib245/ga/GAGenome.h
Objective.o: ../../galib245/ga/gaerror.h ../../galib245/ga/GAEvalData.h
Objective.o: ../../galib245/ga/GAPopulation.h ../../galib245/ga/GASelector.h
Objective.o: ../../galib245/ga/GAScaling.h ../../galib245/ga/GASStateGA.h
Objective.o: ../../galib245/ga/GAIncGA.h ../../galib245/ga/GADemeGA.h
Objective.o: ../../galib245/ga/GADCrowdingGA.h
Objective.o: ../../galib245/ga/GA1DBinStrGenome.h
Objective.o: ../../galib245/ga/GABinStr.h ../../galib245/ga/garandom.h
Objective.o: ../../galib245/ga/GA2DBinStrGenome.h
Objective.o: ../../galib245/ga/GA3DBinStrGenome.h
Objective.o: ../../galib245/ga/GABin2DecGenome.h ../../galib245/ga/gabincvt.h
Objective.o: ../../galib245/ga/GA1DArrayGenome.h ../../galib245/ga/GAArray.h
Objective.o: ../../galib245/ga/GAAllele.h ../../galib245/ga/GA2DArrayGenome.h
Objective.o: ../../galib245/ga/GA3DArrayGenome.h
Objective.o: ../../galib245/ga/GAListGenome.h ../../galib245/ga/GAList.h
Objective.o: ../../galib245/ga/GAListBASE.h ../../galib245/ga/GANode.h
Objective.o: ../../galib245/ga/GATreeGenome.h ../../galib245/ga/GATree.h
Objective.o: ../../galib245/ga/GATreeBASE.h ../../galib245/ga/GARealGenome.h
Objective.o: Util.h /usr/include/math.h
ForceList.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
ForceList.o: /usr/include/CC/stl_config.h /usr/include/standards.h
ForceList.o: /usr/include/CC/stl_algobase.h /usr/include/CC/type_traits.h
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
SquareController.o: ../../dynamechs_3.0/dm/dmObject.h
SquareController.o: ../../dynamechs_3.0/dm/dm.h /usr/include/CC/iostream.h
SquareController.o: /usr/include/memory.h /usr/include/stddef.h
SquareController.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
SquareController.o: /usr/include/CC/fstream.h /usr/include/stdlib.h
SquareController.o: /usr/include/string.h
