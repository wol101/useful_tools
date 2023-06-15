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
.SUFFIXES: .cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<

INC_DIRS= -I. 
LIBS = -lm

all: kinetics_to_vrml
	
kinetics_to_vrml: \
kinetics_to_vrml.o EulerAngles.o
	$(CXX) -o $@ $^ $(LIBS)

clean:
	$(RM) kinetics_to_vrml
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) \
kinetics_to_vrml.cpp EulerAngles.cpp

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

kinetics_to_vrml.o: /usr/include/CC/iostream /usr/include/CC/iosfwd
kinetics_to_vrml.o: /usr/include/CC/stl_config.h /usr/include/standards.h
kinetics_to_vrml.o: /usr/include/wchar.h /usr/include/sgidefs.h
kinetics_to_vrml.o: /usr/include/locale_attr.h /usr/include/stdio.h
kinetics_to_vrml.o: /usr/include/ctype.h /usr/include/time.h
kinetics_to_vrml.o: /usr/include/stddef.h /usr/include/CC/functional
kinetics_to_vrml.o: /usr/include/CC/stl_function.h
kinetics_to_vrml.o: /usr/include/CC/char_traits.h /usr/include/string.h
kinetics_to_vrml.o: /usr/include/CC/istream /usr/include/CC/ostream
kinetics_to_vrml.o: /usr/include/CC/ios /usr/include/CC/stl_string_fwd.h
kinetics_to_vrml.o: /usr/include/CC/stl_alloc.h /usr/include/stdlib.h
kinetics_to_vrml.o: /usr/include/assert.h /usr/include/CC/stl_exception.h
kinetics_to_vrml.o: /usr/include/CC/utility /usr/include/CC/stl_relops.h
kinetics_to_vrml.o: /usr/include/CC/stl_pair.h /usr/include/CC/stl_locale.h
kinetics_to_vrml.o: /usr/include/CC/typeinfo /usr/include/CC/exception
kinetics_to_vrml.o: /usr/include/CC/stl_threads.h /usr/include/mutex.h
kinetics_to_vrml.o: /usr/include/CC/string /usr/include/CC/stdexcept
kinetics_to_vrml.o: /usr/include/CC/stl_iterator_base.h
kinetics_to_vrml.o: /usr/include/CC/memory /usr/include/CC/stl_algobase.h
kinetics_to_vrml.o: /usr/include/CC/type_traits.h /usr/include/limits.h
kinetics_to_vrml.o: /usr/include/CC/new.h /usr/include/CC/iostream.h
kinetics_to_vrml.o: /usr/include/memory.h /usr/include/CC/stl_iterator.h
kinetics_to_vrml.o: /usr/include/CC/stl_construct.h
kinetics_to_vrml.o: /usr/include/CC/stl_tempbuf.h
kinetics_to_vrml.o: /usr/include/CC/stl_uninitialized.h
kinetics_to_vrml.o: /usr/include/CC/stl_raw_storage_iter.h
kinetics_to_vrml.o: /usr/include/CC/algorithm /usr/include/CC/stl_algo.h
kinetics_to_vrml.o: /usr/include/CC/stl_heap.h /usr/include/CC/stl_hash_fun.h
kinetics_to_vrml.o: /usr/include/CC/stl_ctype.h /usr/include/CC/c_locale.h
kinetics_to_vrml.o: /usr/include/CC/streambuf
kinetics_to_vrml.o: /usr/include/CC/stl_stdio_file.h
kinetics_to_vrml.o: /usr/include/CC/stl_numeric_facets.h
kinetics_to_vrml.o: /usr/include/CC/limits /usr/include/float.h
kinetics_to_vrml.o: /usr/include/CC/fstream /usr/include/CC/stl_codecvt.h
kinetics_to_vrml.o: /usr/include/CC/vector /usr/include/CC/stl_range_errors.h
kinetics_to_vrml.o: /usr/include/CC/stl_vector.h
kinetics_to_vrml.o: /usr/include/CC/stl_bvector.h
