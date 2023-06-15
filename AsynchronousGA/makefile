# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3 -ffast-math
EXTRA_FLAGS ?= -std=c++11 -DUSE_TCP -DCOMMAND_LINE_VERSION

ifeq ($(SYSTEM),Linux)
# GNU compiler flags 
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs)
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
# suggested by linker
LDFLAGS = -Xlinker -bind_at_load
ifeq ($(OPT_FLAGS), -g)
LIBS = -lMallocDebug
endif
endif

ifeq ($(SYSTEM),CYGWIN_NT-10.0)
# GNU compiler flags 
OPT_FLAGS = -Os
CXXFLAGS = -Wall -fexceptions -Wa,-mbig-obj -Dexprtk_disable_enhanced_features $(OPT_FLAGS) $(EXTRA_FLAGS)
endif

LIBS += -lm
INC_DIRS += -Iexprtk

SRC = DataFile.cpp  Genome.cpp  MD5.cpp     Preferences.cpp  Statistics.cpp  UDP.cpp         XMLConverter.cpp\
      FEC.cpp       GATCP.cpp   Mating.cpp  Population.cpp   Random.cpp      TCP.cpp         WindowsGlue.cpp

OBJ = $(addsuffix .o, $(basename $(SRC) ) )

BINARIES = bin/AsynchronousGA_TCP

all: directories binaries 

directories: bin obj 

binaries: $(BINARIES)

obj: 
	-mkdir obj
	-mkdir obj/obj
	-mkdir obj/gaitsym

bin:
	-mkdir bin
	 
obj/obj/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

bin/AsynchronousGA_TCP: $(addprefix obj/obj/, $(OBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o nohup.out

superclean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	rm -rf project_builder/build project_builder/MergeXML.pbproj/*.pbxuser
	rm -rf build-AsynchronousGAQt*
	find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;

