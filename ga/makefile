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
CXX         = CC -LANG:std 
CXXFLAGS    = -fullwarn -n32 $(OPT_FLAGS)
CXX_INC_DIR = /usr/include/CC
INC_DIRS    = -I/usr/local/include
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c
SOCKET_LIBS = -L/usr/local/lib -lptypes
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
SOCKET_LIBS = -L/usr/local/lib -lptypes
endif

GA_OBJECTS = GA.o Genome.o Objective.o Random.o Mating.o Statistics.o \
Population.o DataFile.o

PVM_LIBS ?= -L$(PVM_ROOT)/lib/$(PVM_ARCH)/ -lpvm3
PVM_INC ?= -I$(PVM_ROOT)/include/
PVM_INSTALL ?= $(PVM_ROOT)/bin/$(PVM_ARCH)

SOCKET_LIBS ?= -lptypes

vpath %.cc src 

all: drand48/drand48lib.a directories binaries 

directories: bin obj obj/obj obj/pvm obj/socket

binaries: bin/ga bin/ga_pvm bin/ga_socket \
bin/objective bin/objective_pvm bin/objective_socket \
bin/hillclimb bin/screen bin/utility bin/test_random

obj/obj: 
	mkdir obj/obj

obj/pvm: 
	mkdir obj/pvm

obj/socket:
	mkdir obj/socket

bin:
	mkdir bin

obj:
	mkdir obj
        
drand48/drand48lib.a:
	( cd drand48; make )
         
obj/pvm/%.o : %.cc
	$(CXX) -DUSE_PVM $(CXXFLAGS) $(PVM_INC) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/ga_pvm: $(addprefix obj/pvm/, $(GA_OBJECTS) )
	$(CXX) -o $@ $^ drand48/drand48lib.a $(PVM_LIBS) -lm

bin/objective_pvm: obj/pvm/TestObjective.o
	$(CXX) -o $@ $^ $(PVM_LIBS) -lm

obj/socket/%.o : %.cc
	$(CXX) -DUSE_SOCKETS $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/ga_socket: $(addprefix obj/socket/, $(GA_OBJECTS) )
	$(CXX) -o $@ $^ drand48/drand48lib.a $(SOCKET_LIBS) -lm

bin/objective_socket: obj/socket/TestObjective.o obj/socket/DataFile.o
	$(CXX) -o $@ $^ $(SOCKET_LIBS) -lm

obj/obj/%.o : %.cc
	$(CXX) -DUSE_FS $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/ga: $(addprefix obj/obj/, $(GA_OBJECTS) )
	$(CXX) -o $@ $^ drand48/drand48lib.a -lm

bin/objective: obj/obj/TestObjective.o
	$(CXX) -o $@ $^ -lm

bin/hillclimb: $(addprefix obj/obj/, HillClimb.o Genome.o Random.o)
	$(CXX) -o $@ $^ drand48/drand48lib.a -lm
        
bin/screen: $(addprefix obj/obj/, StartupScreen.o Genome.o Random.o Population.o Objective.o)
	$(CXX) -o $@ $^ drand48/drand48lib.a -lm
        
bin/utility: $(addprefix obj/obj/, Utility.o Genome.o Random.o Population.o Objective.o)
	$(CXX) -o $@ $^ drand48/drand48lib.a -lm
        
bin/test_random: $(addprefix obj/obj/, TestRandom.o Random.o )
	$(CXX) -o $@ $^ drand48/drand48lib.a -lm
        
clean:
	rm -rf bin obj
	rm -f *.o *.bak *.bck *~ core *.tmp *.exe
	rm -rf ii_files
	(cd src; rm -f *.o *.bak *.bck *~ core *.tmp *.exe)

install_pvm:
	cp -f bin/ga_pvm $(PVM_INSTALL)
	
