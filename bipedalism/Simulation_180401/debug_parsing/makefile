# makefile for the subprocess objective option

# get the system name
SYSTEM = $(shell uname)

# useful functions
MKDEPEND    = makedepend -I/usr/include
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -ptused 
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
PRELINK     =
CXXFLAGS    = -Wall -fexceptions -g
CXX_INC_DIR = /usr/include/g++-3/
LD          = g++ -w
AR          = ar rv
INSTALL     = install -c                       
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++
PRELINK     =
CXXFLAGS    = -Wall -fexceptions -O3
CXX_INC_DIR = 
LD          = c++ -w
AR          = ar rv
INSTALL     = install -c                       
endif

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


LIBS =  

all: DebugParsing

DebugParsing: \
DebugParsing.o ParameterFile.o
	$(CXX) -o $@ $^ $(LIBS)

clean:
	$(RM) DebugParsing
	$(RM) *.o *~ *.bak core *.bck
	$(RM) ii_files

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) \
DebugParsing.cc ParameterFile.cc

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

DebugParsing.o: /usr/include/stdio.h /usr/include/sys/types.h
DebugParsing.o: /usr/include/sys/cdefs.h /usr/include/machine/types.h
DebugParsing.o: /usr/include/ppc/types.h /usr/include/machine/ansi.h
DebugParsing.o: /usr/include/ppc/ansi.h /usr/include/machine/endian.h
DebugParsing.o: /usr/include/ppc/endian.h
DebugParsing.o: /usr/include/gcc/darwin/2.95.2/g++/iostream.h
DebugParsing.o: /usr/include/gcc/darwin/2.95.2/g++/streambuf.h
DebugParsing.o: /usr/include/gcc/darwin/2.95.2/g++/libio.h
DebugParsing.o: /usr/include/gcc/darwin/2.95.2/g++/_G_config.h
DebugParsing.o: /usr/include/stddef.h /usr/include/stdarg.h
DebugParsing.o: /usr/include/gcc/darwin/2.95.2/g++/fstream.h ParameterFile.h
ParameterFile.o: /usr/include/stdio.h /usr/include/sys/types.h
ParameterFile.o: /usr/include/sys/cdefs.h /usr/include/machine/types.h
ParameterFile.o: /usr/include/ppc/types.h /usr/include/machine/ansi.h
ParameterFile.o: /usr/include/ppc/ansi.h /usr/include/machine/endian.h
ParameterFile.o: /usr/include/ppc/endian.h /usr/include/string.h
ParameterFile.o: /usr/include/sys/stat.h /usr/include/sys/time.h
ParameterFile.o: /usr/include/time.h /usr/include/machine/limits.h
ParameterFile.o: /usr/include/ppc/limits.h /usr/include/stdlib.h
ParameterFile.o: /usr/include/gcc/darwin/2.95.2/g++/iostream.h
ParameterFile.o: /usr/include/gcc/darwin/2.95.2/g++/streambuf.h
ParameterFile.o: /usr/include/gcc/darwin/2.95.2/g++/libio.h
ParameterFile.o: /usr/include/gcc/darwin/2.95.2/g++/_G_config.h
ParameterFile.o: /usr/include/stddef.h /usr/include/stdarg.h ParameterFile.h
