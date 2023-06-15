# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3 -ffast-math
EXTRA_FLAGS ?= -std=c++17 -DCOMMAND_LINE_VERSION

ifeq ($(SYSTEM),Linux)
# GNU compiler flags 
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
LIBS += -lpthread
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
INC_DIRS += \
-Iasio-1.18.2/include \
-Ienet-1.3.14/include \
-Iexprtk \
-Ipystring \
-Irapidxml-1.13

SRC = \
DataFile.cpp \
GAASIO.cpp \
Genome.cpp \
MD5.cpp \
Mating.cpp \
MergeExpressionParser.cpp \
MergeXML.cpp \
Population.cpp \
Preferences.cpp \
Random.cpp \
ServerASIO.cpp \
Statistics.cpp \
XMLContainer.cpp \
XMLContainerList.cpp \
XMLConverter.cpp 

PYSTRINGSRC = \
pystring.cpp


OBJ = $(addsuffix .o, $(basename $(SRC) ) )
PYSTRINGOBJ = $(addsuffix .o, $(basename $(PYSTRINGSRC) ) )
ENETOBJ = $(addsuffix .o, $(basename $(ENETSRC) ) )

BINARIES = bin/AsynchronousGA_ASIO

all: directories binaries 

directories: bin obj 

binaries: $(BINARIES)

obj: 
	-mkdir obj
	-mkdir obj/tcp
	-mkdir obj/asio
	-mkdir obj/pystring
	-mkdir obj/enet

bin:
	-mkdir bin

obj/pystring/%.o : pystring/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@


obj/asio/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -DUSE_ASIO -c $< -o $@

bin/AsynchronousGA_ASIO: $(addprefix obj/asio/, $(OBJ) ) $(addprefix obj/pystring/, $(PYSTRINGOBJ) ) $(addprefix obj/enet/, $(ENETOBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o nohup.out
	rm -rf build*

superclean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	rm -rf project_builder/build project_builder/MergeXML.pbproj/*.pbxuser
	rm -rf build*
	rm -rf AsynchronousGA2019Qt/AsynchronousGA2019Qt*.pro.user*
	find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;
	find . -name '*~' -exec rm -f {} \;
	find . -name '#*' -exec rm -f {} \;
	find . -name '*.bak' -exec rm -f {} \;
	find . -name '*.bck' -exec rm -f {} \;
	find . -name '*.tmp' -exec rm -f {} \;
	find . -name '*.o' -exec rm -f {} \;

