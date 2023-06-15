# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3 -ffast-math
EXTRA_FLAGS ?= 

ifeq ($(SYSTEM),Linux)
# GNU compiler flags 
CXX	  = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
# extras for Beowulf compilation
ifeq ($(HOSTNAME),ulgbc1.liv.ac.uk)
OPT_FLAGS += -march=k6
endif
ifeq ($(HOSTNAME),microlith)
#OPT_FLAGS += -march=opteron
endif
# set the memory debugging library
# LIBS += -lefence
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags 
CXX	  = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
# suggested by linker
LDFLAGS = -Xlinker -bind_at_load
# set the memory debugging library
#LIBS = -lMallocDebug
endif

# generic library and include definitions

LIBS += -L/usr/local/lib -lm
INC_DIRS += -I../src

SRCFILES = \
main.cpp     RangedVariable.cpp    DataFile.cpp   Random.cpp  

OBJFILES = $(addsuffix .o, $(basename $(SRCFILES) ) )

vpath %.cpp ../src

BINARIES = ../bin/montecarlo 

all: directories binaries 

directories: ../bin ../obj ../obj/montecarlo 

binaries: $(BINARIES)

../obj/montecarlo: ../obj
	-mkdir $@

../bin:
	-mkdir $@

../obj:
	-mkdir $@
        	 
../obj/montecarlo/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

../bin/montecarlo: $(addprefix ../obj/montecarlo/, $(OBJFILES) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm -rf ../obj/montecarlo ../bin/montecarlo
	-rm -f *~ *.bak *.bck *.tmp *.o 

superclean:
	-rm -rf ../obj/montecarlo ../bin/montecarlo
	-rm -f *~ *.bak *.bck *.tmp *.o 
	-rm -rf build MonteCarlo.pbproj/*.pbxuser
	-find . -name '.DS_Store' -exec rm -f {} \;
	-find . -name '.gdb_history' -exec rm -f {} \;
	-find . -name '.#*' -exec rm -f {} \;

    
