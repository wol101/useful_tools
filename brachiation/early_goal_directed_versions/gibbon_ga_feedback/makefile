
include makevars


EXTRA = ../gibbon_sym_feedback/
DYN_DIR = ../dynamechs_3.0/
INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL -I/usr/X11R6/include \
-I/usr/X11R6/include/GL -I$(EXTRA) -I../galib245/ -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
LIBS = $(DYN_DIR)dm/libdm.a $(DYN_DIR)dmGL/libdmGL.a $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dm/libdm.a 
OPENGL_LIBS = -L/usr/X11R6/lib -L/usr/share/src/OpenGL/toolkits/lib32 -lglut -lGLU -lGL -lX11 -ldl -lXmu
GA_LIBS = ../galib245/ga/libga.a

all: GA objective
	cd ../gibbon_sym_feedback; make
	
GA: GA.o SubprocessObjective.o GAUtils.o
	$(CXX) -o $@ $^ $(LIBS) $(OPENGL_LIBS) $(GA_LIBS) -lm


objective: Objective.o GAUtils.o \
$(EXTRA)Bar.o $(EXTRA)ForceList.o  $(EXTRA)FBMuscleModel.o    $(EXTRA)Util.o \
$(EXTRA)Controller.o      $(EXTRA)FBGripModel.o  $(EXTRA)Simulation.o  \
$(EXTRA)ControllerList.o  $(EXTRA)LoadFile.o   $(EXTRA)SinController.o \
$(EXTRA)strtok.o $(EXTRA)LoggedForce.o $(EXTRA)Sensor.o $(EXTRA)SensorList.o \
$(EXTRA)IntegratingController.o
	$(CXX) -o $@ $^ $(LIBS) -lm $(OPENGL_LIBS) $(GA_LIBS)

clean:
	$(RM) GA objective
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) GA.cpp Objective.cpp SubprocessObjective.cpp GAUtils.cpp

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

GA.o: /usr/include/stdio.h /usr/include/standards.h /usr/include/sgidefs.h
GA.o: /usr/include/CC/iostream.h /usr/include/memory.h /usr/include/stddef.h
GA.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
GA.o: /usr/include/CC/fstream.h /usr/include/assert.h
GA.o: /usr/include/CC/exception ../galib245/ga/ga.h ../galib245/ga/gaconfig.h
GA.o: /usr/include/CC/iostream /usr/include/CC/iosfwd
GA.o: /usr/include/CC/stl_config.h /usr/include/wchar.h
GA.o: /usr/include/locale_attr.h /usr/include/ctype.h /usr/include/time.h
GA.o: /usr/include/CC/functional /usr/include/CC/stl_function.h
GA.o: /usr/include/CC/char_traits.h /usr/include/string.h
GA.o: /usr/include/CC/istream /usr/include/CC/ostream /usr/include/CC/ios
GA.o: /usr/include/CC/stl_string_fwd.h /usr/include/CC/stl_alloc.h
GA.o: /usr/include/stdlib.h /usr/include/CC/stl_exception.h
GA.o: /usr/include/CC/utility /usr/include/CC/stl_relops.h
GA.o: /usr/include/CC/stl_pair.h /usr/include/CC/stl_locale.h
GA.o: /usr/include/CC/typeinfo /usr/include/CC/stl_threads.h
GA.o: /usr/include/mutex.h /usr/include/CC/string /usr/include/CC/stdexcept
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
GA.o: /usr/include/CC/stl_codecvt.h ../galib245/ga/GASimpleGA.h
GA.o: ../galib245/ga/GABaseGA.h ../galib245/ga/gaid.h
GA.o: ../galib245/ga/GAParameter.h ../galib245/ga/gatypes.h
GA.o: ../galib245/ga/GAStatistics.h ../galib245/ga/GAGenome.h
GA.o: ../galib245/ga/gaerror.h ../galib245/ga/GAEvalData.h
GA.o: ../galib245/ga/GAPopulation.h ../galib245/ga/GASelector.h
GA.o: ../galib245/ga/GAScaling.h ../galib245/ga/GASStateGA.h
GA.o: ../galib245/ga/GAIncGA.h ../galib245/ga/GADemeGA.h
GA.o: ../galib245/ga/GADCrowdingGA.h ../galib245/ga/GA1DBinStrGenome.h
GA.o: ../galib245/ga/GABinStr.h ../galib245/ga/garandom.h
GA.o: ../galib245/ga/GA2DBinStrGenome.h ../galib245/ga/GA3DBinStrGenome.h
GA.o: ../galib245/ga/GABin2DecGenome.h ../galib245/ga/gabincvt.h
GA.o: ../galib245/ga/GA1DArrayGenome.h ../galib245/ga/GAArray.h
GA.o: ../galib245/ga/GAAllele.h ../galib245/ga/GA2DArrayGenome.h
GA.o: ../galib245/ga/GA3DArrayGenome.h ../galib245/ga/GAListGenome.h
GA.o: ../galib245/ga/GAList.h ../galib245/ga/GAListBASE.h
GA.o: ../galib245/ga/GANode.h ../galib245/ga/GATreeGenome.h
GA.o: ../galib245/ga/GATree.h ../galib245/ga/GATreeBASE.h
GA.o: ../galib245/ga/GARealGenome.h ../galib245/ga/GARealGenome.C GAUtils.h
Objective.o: /usr/include/stdio.h /usr/include/standards.h
Objective.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
Objective.o: /usr/include/memory.h /usr/include/stddef.h
Objective.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
Objective.o: /usr/include/CC/fstream.h /usr/include/assert.h
Objective.o: /usr/share/src/OpenGL/toolkits/include/GL/glut.h
Objective.o: /usr/include/GL/gl.h /usr/include/GL/glu.h
Objective.o: ../dynamechs_3.0/dmGL/dmGLTime.h ../dynamechs_3.0/dmGL/dmGL.h
Objective.o: /usr/include/GL/gl.h /usr/include/GL/glu.h /usr/include/math.h
Objective.o: /usr/include/stdlib.h /usr/include/sys/time.h
Objective.o: ../dynamechs_3.0/dmGL/dmGLMouse.h
Objective.o: ../dynamechs_3.0/dmGL/dmGLPolarCamera_zup.h
Objective.o: ../dynamechs_3.0/dmGL/dmGLPolarCamera.h
Objective.o: ../dynamechs_3.0/dmGL/dmGLCamera.h
Objective.o: ../dynamechs_3.0/dm/dmEnvironment.h ../dynamechs_3.0/dm/dm.h
Objective.o: /usr/include/string.h ../dynamechs_3.0/dm/dmObject.h
Objective.o: ../dynamechs_3.0/dm/dmArticulation.h
Objective.o: ../dynamechs_3.0/dm/dmLink.h ../dynamechs_3.0/dm/dmSystem.h
Objective.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
Objective.o: /usr/include/CC/stl_config.h /usr/include/CC/stl_algobase.h
Objective.o: /usr/include/CC/type_traits.h /usr/include/limits.h
Objective.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator_base.h
Objective.o: /usr/include/CC/stl_iterator.h /usr/include/CC/stl_alloc.h
Objective.o: /usr/include/CC/stl_construct.h
Objective.o: /usr/include/CC/stl_uninitialized.h /usr/include/CC/stl_vector.h
Objective.o: /usr/include/CC/stl_bvector.h
Objective.o: ../dynamechs_3.0/dm/dmMobileBaseLink.h
Objective.o: ../dynamechs_3.0/dm/dmRigidBody.h ../dynamechs_3.0/dm/dmForce.h
Objective.o: ../dynamechs_3.0/dmu/dmu.h ../gibbon_sym_feedback/Bar.h
Objective.o: ../gibbon_sym_feedback/ForceList.h
Objective.o: ../gibbon_sym_feedback/Simulation.h
Objective.o: ../gibbon_sym_feedback/Controller.h
Objective.o: ../gibbon_sym_feedback/ControllerList.h
Objective.o: ../gibbon_sym_feedback/FBGripModel.h
Objective.o: ../gibbon_sym_feedback/LoggedForce.h
Objective.o: ../gibbon_sym_feedback/Sensor.h GAUtils.h
SubprocessObjective.o: /usr/include/stdio.h /usr/include/standards.h
SubprocessObjective.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
SubprocessObjective.o: /usr/include/memory.h /usr/include/stddef.h
SubprocessObjective.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
SubprocessObjective.o: /usr/include/CC/fstream.h /usr/include/assert.h
SubprocessObjective.o: /usr/include/CC/exception ../galib245/ga/ga.h
SubprocessObjective.o: ../galib245/ga/gaconfig.h /usr/include/CC/iostream
SubprocessObjective.o: /usr/include/CC/iosfwd /usr/include/CC/stl_config.h
SubprocessObjective.o: /usr/include/wchar.h /usr/include/locale_attr.h
SubprocessObjective.o: /usr/include/ctype.h /usr/include/time.h
SubprocessObjective.o: /usr/include/CC/functional
SubprocessObjective.o: /usr/include/CC/stl_function.h
SubprocessObjective.o: /usr/include/CC/char_traits.h /usr/include/string.h
SubprocessObjective.o: /usr/include/CC/istream /usr/include/CC/ostream
SubprocessObjective.o: /usr/include/CC/ios /usr/include/CC/stl_string_fwd.h
SubprocessObjective.o: /usr/include/CC/stl_alloc.h /usr/include/stdlib.h
SubprocessObjective.o: /usr/include/CC/stl_exception.h
SubprocessObjective.o: /usr/include/CC/utility /usr/include/CC/stl_relops.h
SubprocessObjective.o: /usr/include/CC/stl_pair.h
SubprocessObjective.o: /usr/include/CC/stl_locale.h /usr/include/CC/typeinfo
SubprocessObjective.o: /usr/include/CC/stl_threads.h /usr/include/mutex.h
SubprocessObjective.o: /usr/include/CC/string /usr/include/CC/stdexcept
SubprocessObjective.o: /usr/include/CC/stl_iterator_base.h
SubprocessObjective.o: /usr/include/CC/memory /usr/include/CC/stl_algobase.h
SubprocessObjective.o: /usr/include/CC/type_traits.h /usr/include/limits.h
SubprocessObjective.o: /usr/include/CC/new.h /usr/include/CC/stl_iterator.h
SubprocessObjective.o: /usr/include/CC/stl_construct.h
SubprocessObjective.o: /usr/include/CC/stl_tempbuf.h
SubprocessObjective.o: /usr/include/CC/stl_uninitialized.h
SubprocessObjective.o: /usr/include/CC/stl_raw_storage_iter.h
SubprocessObjective.o: /usr/include/CC/algorithm /usr/include/CC/stl_algo.h
SubprocessObjective.o: /usr/include/CC/stl_heap.h
SubprocessObjective.o: /usr/include/CC/stl_hash_fun.h
SubprocessObjective.o: /usr/include/CC/stl_ctype.h /usr/include/CC/c_locale.h
SubprocessObjective.o: /usr/include/CC/streambuf
SubprocessObjective.o: /usr/include/CC/stl_stdio_file.h
SubprocessObjective.o: /usr/include/CC/stl_numeric_facets.h
SubprocessObjective.o: /usr/include/CC/limits /usr/include/float.h
SubprocessObjective.o: /usr/include/CC/fstream /usr/include/CC/stl_codecvt.h
SubprocessObjective.o: ../galib245/ga/GASimpleGA.h ../galib245/ga/GABaseGA.h
SubprocessObjective.o: ../galib245/ga/gaid.h ../galib245/ga/GAParameter.h
SubprocessObjective.o: ../galib245/ga/gatypes.h ../galib245/ga/GAStatistics.h
SubprocessObjective.o: ../galib245/ga/GAGenome.h ../galib245/ga/gaerror.h
SubprocessObjective.o: ../galib245/ga/GAEvalData.h
SubprocessObjective.o: ../galib245/ga/GAPopulation.h
SubprocessObjective.o: ../galib245/ga/GASelector.h ../galib245/ga/GAScaling.h
SubprocessObjective.o: ../galib245/ga/GASStateGA.h ../galib245/ga/GAIncGA.h
SubprocessObjective.o: ../galib245/ga/GADemeGA.h
SubprocessObjective.o: ../galib245/ga/GADCrowdingGA.h
SubprocessObjective.o: ../galib245/ga/GA1DBinStrGenome.h
SubprocessObjective.o: ../galib245/ga/GABinStr.h ../galib245/ga/garandom.h
SubprocessObjective.o: ../galib245/ga/GA2DBinStrGenome.h
SubprocessObjective.o: ../galib245/ga/GA3DBinStrGenome.h
SubprocessObjective.o: ../galib245/ga/GABin2DecGenome.h
SubprocessObjective.o: ../galib245/ga/gabincvt.h
SubprocessObjective.o: ../galib245/ga/GA1DArrayGenome.h
SubprocessObjective.o: ../galib245/ga/GAArray.h ../galib245/ga/GAAllele.h
SubprocessObjective.o: ../galib245/ga/GA2DArrayGenome.h
SubprocessObjective.o: ../galib245/ga/GA3DArrayGenome.h
SubprocessObjective.o: ../galib245/ga/GAListGenome.h ../galib245/ga/GAList.h
SubprocessObjective.o: ../galib245/ga/GAListBASE.h ../galib245/ga/GANode.h
SubprocessObjective.o: ../galib245/ga/GATreeGenome.h ../galib245/ga/GATree.h
SubprocessObjective.o: ../galib245/ga/GATreeBASE.h
SubprocessObjective.o: ../galib245/ga/GARealGenome.h GAUtils.h
GAUtils.o: /usr/include/stdio.h /usr/include/standards.h
GAUtils.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
GAUtils.o: /usr/include/memory.h /usr/include/stddef.h
GAUtils.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
GAUtils.o: /usr/include/CC/fstream.h /usr/include/assert.h
GAUtils.o: /usr/include/string.h GAUtils.h
