# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3 -ffast-math
EXTRA_FLAGS ?= 

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
INC_DIRS += -I"../../GaitSym2016/src" -I"../../GaitSym2016/exprtk" -I"../../GaitSym2016/rapidxml-1.13"

SRC = MergeExpressionParser.cpp MergeXML.cpp  XMLContainer.cpp XMLContainerList.cpp

GAITSYMSRC = DataFile.cpp

OBJ = $(addsuffix .o, $(basename $(SRC) ) )
GAITSYMOBJ = $(addsuffix .o, $(basename $(GAITSYMSRC) ) )

BINARIES = ../bin/mergexml

all: directories binaries 

directories: ../bin ../obj 

binaries: $(BINARIES)

../obj: 
	-mkdir ../obj
	-mkdir ../obj/obj
	-mkdir ../obj/gaitsym

../bin:
	-mkdir ../bin
	 
../obj/obj/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

../obj/gaitsym/%.o : ../../GaitSym2016/src/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

../bin/mergexml: $(addprefix ../obj/obj/, $(OBJ) ) $(addprefix ../obj/gaitsym/, $(GAITSYMOBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf ../obj ../bin
	rm -f *~ *.bak *.bck *.tmp *.o 

superclean:
	rm -rf ../obj ../bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	rm -rf project_builder/build project_builder/MergeXML.pbproj/*.pbxuser
	find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;

