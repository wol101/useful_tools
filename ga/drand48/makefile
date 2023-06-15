SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3

# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -fullwarn -n32 -LANG:std $(OPT_FLAGS)
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c
# dummy value for ranlib since it is not needed on Irix
RANLIB      = echo
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
PRELINK     =
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)
CXX_INC_DIR = /usr/include/g++-3/
LD          = g++ -w
AR          = ar rv
INSTALL     = install -c                       
RANLIB      = ranlib
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs)
CXX         = c++
PRELINK     =
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)
CXX_INC_DIR = 
LD          = c++ -w
AR          = ar rv
INSTALL     = install -c                       
RANLIB      = ranlib
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.0)
# GNU compiler flags (works with g++ and egcs)
CXX         = c++
PRELINK     =
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)
CXX_INC_DIR = 
LD          = c++ -w
AR          = ar rv
INSTALL     = install -c                       
RANLIB      = ranlib
endif


# set up a compilation rule for gnu make
.SUFFIXES: .c
.c.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


drand48lib.a: _rand48.o drand48.o erand48.o nrand48.o seed48.o srand48.o
	$(AR) $@ $^
	$(RANLIB) $@

clean:
	rm -f ga objective hillclimb screen utility
	rm -f *.o *.bak *.bck *~ core *.tmp *.exe
	rm -rf ii_files
