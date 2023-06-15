# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -DUSE_OLD_STREAMS -ptused 
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c

# set up a compilation rule for gnu make
.SUFFIXES: .cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


INC_DIRS= -I. -I../../galib245/ 
LIBS = -lm 
GA_LIBS = ../../galib245/ga/libga.a

all: ga
	
ga: GA.o SubprocessObjective.o GAUtils.o
	$(CXX) -o $@ $^ $(GA_LIBS) $(LIBS)
	$(CP) $@ ../bin 

clean:
	$(RM) ga
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) GA.cpp SubprocessObjective.cpp GAUtils.cpp

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

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
GA.o: GAUtils.h
SubprocessObjective.o: /usr/include/stdio.h /usr/include/standards.h
SubprocessObjective.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
SubprocessObjective.o: /usr/include/memory.h /usr/include/stddef.h
SubprocessObjective.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
SubprocessObjective.o: /usr/include/CC/fstream.h /usr/include/assert.h
SubprocessObjective.o: /usr/include/CC/exception ../../galib245/ga/ga.h
SubprocessObjective.o: ../../galib245/ga/gaconfig.h /usr/include/CC/iostream
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
SubprocessObjective.o: ../../galib245/ga/GASimpleGA.h
SubprocessObjective.o: ../../galib245/ga/GABaseGA.h ../../galib245/ga/gaid.h
SubprocessObjective.o: ../../galib245/ga/GAParameter.h
SubprocessObjective.o: ../../galib245/ga/gatypes.h
SubprocessObjective.o: ../../galib245/ga/GAStatistics.h
SubprocessObjective.o: ../../galib245/ga/GAGenome.h
SubprocessObjective.o: ../../galib245/ga/gaerror.h
SubprocessObjective.o: ../../galib245/ga/GAEvalData.h
SubprocessObjective.o: ../../galib245/ga/GAPopulation.h
SubprocessObjective.o: ../../galib245/ga/GASelector.h
SubprocessObjective.o: ../../galib245/ga/GAScaling.h
SubprocessObjective.o: ../../galib245/ga/GASStateGA.h
SubprocessObjective.o: ../../galib245/ga/GAIncGA.h
SubprocessObjective.o: ../../galib245/ga/GADemeGA.h
SubprocessObjective.o: ../../galib245/ga/GADCrowdingGA.h
SubprocessObjective.o: ../../galib245/ga/GA1DBinStrGenome.h
SubprocessObjective.o: ../../galib245/ga/GABinStr.h
SubprocessObjective.o: ../../galib245/ga/garandom.h
SubprocessObjective.o: ../../galib245/ga/GA2DBinStrGenome.h
SubprocessObjective.o: ../../galib245/ga/GA3DBinStrGenome.h
SubprocessObjective.o: ../../galib245/ga/GABin2DecGenome.h
SubprocessObjective.o: ../../galib245/ga/gabincvt.h
SubprocessObjective.o: ../../galib245/ga/GA1DArrayGenome.h
SubprocessObjective.o: ../../galib245/ga/GAArray.h
SubprocessObjective.o: ../../galib245/ga/GAAllele.h
SubprocessObjective.o: ../../galib245/ga/GA2DArrayGenome.h
SubprocessObjective.o: ../../galib245/ga/GA3DArrayGenome.h
SubprocessObjective.o: ../../galib245/ga/GAListGenome.h
SubprocessObjective.o: ../../galib245/ga/GAList.h
SubprocessObjective.o: ../../galib245/ga/GAListBASE.h
SubprocessObjective.o: ../../galib245/ga/GANode.h
SubprocessObjective.o: ../../galib245/ga/GATreeGenome.h
SubprocessObjective.o: ../../galib245/ga/GATree.h
SubprocessObjective.o: ../../galib245/ga/GATreeBASE.h
SubprocessObjective.o: ../../galib245/ga/GARealGenome.h GAUtils.h
GAUtils.o: /usr/include/stdio.h /usr/include/standards.h
GAUtils.o: /usr/include/sgidefs.h /usr/include/CC/iostream.h
GAUtils.o: /usr/include/memory.h /usr/include/stddef.h
GAUtils.o: /usr/include/CC/iomanip.h /usr/include/CC/generic.h
GAUtils.o: /usr/include/CC/fstream.h /usr/include/assert.h
GAUtils.o: /usr/include/string.h GAUtils.h
