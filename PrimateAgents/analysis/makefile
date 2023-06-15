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
LIBS = -L$(HOME)/src/gsl-1.6/.libs -L$(HOME)/src/gsl-1.6/cblas/.libs
INC_DIRS = -I$(HOME)/src/gsl-1.6/
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

LIBS += -L/usr/local/lib -lgsl -lgslcblas -lm
INC_DIRS += -I../src

SRCFILES = \
main.cpp     MonthlySummary.cpp    DataFile.cpp    

OBJFILES = $(addsuffix .o, $(basename $(SRCFILES) ) )

vpath %.cpp ../src

BINARIES = ../bin/analysis 

all: directories binaries 

directories: ../bin ../obj ../obj/analysis 

binaries: $(BINARIES)

../obj/analysis: ../obj
	-mkdir $@

../bin:
	-mkdir $@

../obj:
	-mkdir $@
        	 
../obj/analysis/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

../bin/analysis: $(addprefix ../obj/analysis/, $(OBJFILES) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm -rf ../obj/analysis ../bin/analysis
	-rm -f *~ *.bak *.bck *.tmp *.o 

superclean:
	-rm -rf ../obj/analysis ../bin/analysis
	-rm -f *~ *.bak *.bck *.tmp *.o 
	-rm -rf build Anlysis.pbproj/*.pbxuser
	-find . -name '.DS_Store' -exec rm -f {} \;
	-find . -name '.gdb_history' -exec rm -f {} \;
	-find . -name '.#*' -exec rm -f {} \;

    
