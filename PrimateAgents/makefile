# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3 -ffast-math
EXTRA_FLAGS ?= 

ifeq ($(SYSTEM),Linux)
# GNU compiler flags 
CXX	  = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
INC_DIRS = -I/usr/local/include/libxml2
# extras for Beowulf compilation
ifeq ($(HOSTNAME),ulgbc1.liv.ac.uk)
OPT_FLAGS += -march=k6
LIBS = -L$(HOME)/src/libxml2-2.6.2/.libs
INC_DIRS = -I$(HOME)/src/libxml2-2.6.2/include
endif
ifeq ($(HOSTNAME),microlith)
#OPT_FLAGS += -march=opteron
LIBS = -L$(HOME)/src/libxml2-2.6.2/.libs -L$(HOME)/src/cvs-1.11.5/zlib
INC_DIRS = -I$(HOME)/src/libxml2-2.6.2/include
EXTRA_FLAGS += -DUSING_64_BIT_POINTERS
endif
OPENGL_LIBS = -L/usr/X11R6/lib -L$(GLUI_DIR) \
	-lglui -lglut -lGLU -lGL -lXmu -lXi -lX11
OPENGL_INC = -I/usr/X11R6/include -I/usr/X11R6/include/GL -I/usr/include/GL -I$(GLUI_DIR)
# set the memory debugging library
# LIBS += -lefence
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags 
CXX	  = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS) -DUPDATE_CONTROLS_ON_IDLE -DUSE_LIBTIFF -DUSE_TIFF_LZW
# suggested by linker
LDFLAGS = -Xlinker -bind_at_load
INC_DIRS = -I/usr/local/include/libxml2
OPENGL_LIBS = -L$(GLUI_DIR) -L$(GLUT_DIR)/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I$(GLUI_DIR) \
	-I$(GLUT_DIR)/include -I$(GLUT_DIR)/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUT_DIR = $(HOME)/Unix/src/glut3.7/
LIBS = -ltiff
# set the memory debugging library
#LIBS = -lMallocDebug
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.1)
# GNU compiler flags 
CXX	  = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS) -DUSE_LOCALTIME
INC_DIRS = -I/usr/include/libxml2
OPENGL_LIBS = -L$(GLUI_DIR) -L$(GLUT_DIR)/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I$(GLUI_DIR) \
	-I$(GLUT_DIR)/include -I$(GLUT_DIR)/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUT_DIR = /usr/local/
SOCKET_LIBS = -L/usr/local/lib -lptypes
endif


# generic library and include definitions
GLUI_DIR = glui_v2_1_beta

LIBS += -L/usr/local/lib -lxml2 -lpthread -lm -lz

SRCFILES = \
DataFile.cpp     DayLength.cpp    GridSquare.cpp   HabitatType.cpp  main.cpp \
Environment.cpp  Grid.cpp         GLUIRoutines.cpp GLShapes.cpp     Agent.cpp \
Simulation.cpp   Random.cpp       TIFFWrite.cpp    Key.cpp

OBJFILES = $(addsuffix .o, $(basename $(SRCFILES) ) )

vpath %.cpp src

BINARIES = bin/primate_agent bin/primate_agent_opengl bin/montecarlo \
bin/analysis

BINARIES_NO_OPENGL = bin/primate_agent bin/montecarlo bin/analysis

all: directories libraries binaries 

no_opengl: directories binaries_no_opengl

directories: bin obj obj/primate_agent obj/primate_agent_opengl

binaries: $(BINARIES)

binaries_no_opengl: $(BINARIES_NO_OPENGL)

libraries: glui

obj/primate_agent: obj
	-mkdir $@

obj/primate_agent_opengl: obj
	-mkdir $@

bin:
	-mkdir $@

obj:
	-mkdir $@
        	 
obj/primate_agent/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

bin/primate_agent: $(addprefix obj/primate_agent/, $(OBJFILES) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

obj/primate_agent_opengl/%.o : %.cpp
	$(CXX) -DUSE_OPENGL $(CXXFLAGS) $(INC_DIRS) $(OPENGL_INC)  -c $< -o $@

bin/primate_agent_opengl: $(addprefix obj/primate_agent_opengl/, $(OBJFILES) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(OPENGL_LIBS) $(LIBS)

clean:
	-rm -rf obj bin
	-rm -f *~ *.bak *.bck *.tmp *.o 
	( cd src; rm -f *~ *.bak *.bck *.tmp *.o )
	( cd montecarlo ; make clean )
	( cd analysis ; make clean )

superclean:
	-rm -rf obj bin
	-rm -f *~ *.bak *.bck *.tmp *.o 
	( cd src; rm -f *~ *.bak *.bck *.tmp *.o )
	( cd scripts; rm -f *~ *.bak *.bck *.tmp *.o )
	( cd analysis; rm -f *~ *.bak *.bck *.tmp *.o )
	( cd data; rm -f *~ *.bak *.bck *.tmp *.o )
	( cd glui_v2_1_beta ; make clean )
	-rm -rf build PrimateAgent.pbproj/*.pbxuser
	-find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;
	( cd montecarlo ; make superclean )
	( cd analysis ; make superclean )

glui: 
	( cd glui_v2_1_beta ; make all )

.PHONY: bin/montecarlo
bin/montecarlo:
	( cd montecarlo ; make all )
        
.PHONY: bin/analysis
bin/analysis:
	( cd analysis ; make all )
