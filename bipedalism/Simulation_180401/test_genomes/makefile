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
CXXFLAGS    = -Wall -fexceptions -O3
CXX_INC_DIR = /usr/include/g++-3/
LD          = g++ -w
AR          = ar rv
INSTALL     = install -c                       
endif

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<

all: left_foot_up scaled_cycle gait_gen1

left_foot_up: left_foot_up.o
	$(CXX) -o $@ $^
	./left_foot_up

scaled_cycle: scaled_cycle.o
	$(CXX) -o $@ $^
	./scaled_cycle

gait_gen1: gait_gen1.o    
	$(CXX) -o $@ $^
	./gait_gen1

clean:
	rm -f *.o *.bak *.bck *~ core *.tmp
	rm -f left_foot_up scaled_cycle gait_gen1 objective

