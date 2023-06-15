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
CXXFLAGS    = -fullwarn -n32 $(OPT_FLAGS)
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c
endif

ifeq ($(SYSTEM),SunOS)
# SunOS using SUNWspro compiler
CXX         = CC
CXXFLAGS    = $(OPT_FLAGS)
CXX_INC_DIR = 
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c
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
PVM_LIBS = -L$(PVM_ROOT_U)/lib/$(PVM_ARCH)/ -lpvm3
PVM_INC = -I$(PVM_ROOT_U)/include/
PVM_INSTALL = $(PVM_ROOT_U)/bin/$(PVM_ARCH)
endif

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<

OBJECTS = GA.o Genome.o Objective.o Random.o Mating.o Statistics.o \
Population.o 

PVM_LIBS ?= -L$(PVM_ROOT)/lib/$(PVM_ARCH)/ -lpvm3
PVM_INC ?= -I$(PVM_ROOT)/include/
PVM_INSTALL ?= $(PVM_ROOT)/bin/$(PVM_ARCH)

INC_DIRS = $(PVM_INC)

all: drand48lib.a DataFile.a ga_pvm objective_pvm   

ga_pvm: $(OBJECTS) ../DataFile/DataFile.a drand48/drand48lib.a
	$(CXX) -o $@ $^ $(PVM_LIBS) -lm

objective_pvm: TestObjective.o
	$(CXX) -o $@ $^ $(PVM_LIBS)
                
screen: StartupScreen.o Genome.o Random.o Population.o Objective.o \
../DataFile/DataFile.a drand48/drand48lib.a
	$(CXX) -o $@ $^ $(PVM_LIBS) -lm
        
utility: Utility.o Genome.o Random.o Population.o Objective.o \
../DataFile/DataFile.a drand48/drand48lib.a
	$(CXX) -o $@ $^ $(PVM_LIBS) -lm
        
test_random: TestRandom.o Random.o drand48/drand48lib.a
	$(CXX) -o $@ $^ -lm

drand48lib.a:         
	( cd drand48; make )

DataFile.a:
	( cd ../DataFile; make )

clean:
	rm -f ga_pvm objective_pvm screen utility test_random
	rm -f *.o *.bak *.bck *~ core *.tmp *.exe
	rm -rf ii_files
	( cd drand48; make clean )
	( cd ../DataFile; make clean )

install:
	cp -f ga_pvm $(PVM_INSTALL)
