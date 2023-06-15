# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX	  = CC -LANG:std
CXXFLAGS    = -fullwarn -n32 -ptused $(OPT_FLAGS)
INC_DIRS    = -I/usr/local/include

OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 -L$(GLUI_DIR) \
	-lglui -lglut -lGLU -lGL -lX11 -lXmu
OPENGL_INC = -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL \
	-I/usr/share/src/OpenGL/toolkits/include -I$(GLUI_DIR)
SOCKET_LIBS = -L/usr/local/lib -lptypes
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX	  = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 
# extras for Athlon-MP system
ifeq ($(HOST),linux-huwis.lboro.ac.uk)
CXXFLAGS += -funroll-loops -fomit-frame-pointer -ffast-math -march=athlon-mp -mfpmath=sse
endif
# extras for Beowulf compilation
ifeq ($(HOSTNAME),ulgbc1.liv.ac.uk)
SOCKET_LIBS = -L$(HOME)/src/ptypes-1.8.3/lib -lptypes
INC_DIRS = -I$(HOME)/src/ptypes-1.8.3/include 
endif
OPENGL_LIBS = -L/usr/X11R6/lib -L$(GLUI_DIR) \
	-lglui -lglut -lGLU -lGL -lXmu -lXi -lX11
OPENGL_INC = -I/usr/X11R6/include -I/usr/X11R6/include/GL -I/usr/include/GL -I$(GLUI_DIR)
# set the memory debugging library
ifeq ($(OPT_FLAGS),-g)
LIBS = -lefence
endif
endif

ifeq ($(SYSTEM),SunOS)
# GNU compiler flags (works with g++ and egcs)
CXX	  = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)
INC_DIRS    = -I$(HOME)/include
SOCKET_LIBS = -L$(HOME)/lib -lptypes -lsocket -lnsl -lrt
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX	  = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 
# suggested by linker
LDFLAGS = -Xlinker -bind_at_load

OPENGL_LIBS = -L$(GLUI_DIR) -L$(GLUT_DIR)/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I$(GLUI_DIR) \
	-I$(GLUT_DIR)/include -I$(GLUT_DIR)/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUT_DIR = $(HOME)/Unix/src/glut3.7/

# set the memory debugging library
ifeq ($(OPT_FLAGS),-g)
LIBS = -lMallocDebug
endif
endif

ifeq ($(SYSTEM),CYGWIN_NT-4.0)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX	  = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 

OPENGL_LIBS = -L$(GLUI_DIR) -L$(GLUT_DIR)/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I$(GLUI_DIR) \
	-I$(GLUT_DIR)/include -I$(GLUT_DIR)/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUT_DIR = /usr/local/
PVM_LIBS = -L$(PVM_ROOT_U)/lib/$(PVM_ARCH) -lpvm3
PVM_INC = -I$(PVM_ROOT_U)/include/
PVM_INSTALL = $(PVM_ROOT_U)/bin/$(PVM_ARCH)
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.0)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX	  = c++-2
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 

OPENGL_LIBS = -L$(GLUI_DIR) -L$(GLUT_DIR)/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I$(GLUI_DIR) \
	-I$(GLUT_DIR)/include -I$(GLUT_DIR)/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUT_DIR = /usr/local/
PVM_LIBS = -L$(PVM_ROOT_U)/lib/$(PVM_ARCH) -lpvm3
PVM_INC = -I$(PVM_ROOT_U)/include/
PVM_INSTALL = $(PVM_ROOT_U)/bin/$(PVM_ARCH)
SOCKET_LIBS = -L/usr/local/lib -lptypes
endif

DYN_DIR = dynamechs_4.0pre1
GLUI_DIR = glui_v2_1_beta

PVM_LIBS ?= -L$(PVM_ROOT)/lib/$(PVM_ARCH) -lpvm3
PVM_INC ?= -I$(PVM_ROOT)/include
PVM_INSTALL ?= $(PVM_ROOT)/bin/$(PVM_ARCH)

DYN_LIBS = $(DYN_DIR)/dm/libdm.a 
DYN_INC = -I$(DYN_DIR)/dm
DYN_LIBS_OPENGL = $(DYN_DIR)/dm/libdm_opengl.a 

SOCKET_LIBS ?= -lptypes

SYMCCFILES = \
CPG.cc		      MAMuscle.cc		 StrapForce.cc \
ModifiedContactModel.cc   StrapForceAnchor.cc \
ForceList.cc		ObjectiveMain.cc	   \
GLUIRoutines.cc	    Segments.cc		 \
LoadObj.cc		  Simulation.cc	      SideStabilizer.cc \
DataFile.cc Genome.cc Random.cc

SYMOBJECTS = $(addsuffix .o, $(basename $(SYMCCFILES) ) )

vpath %.cc src 

all: directories libraries binaries 

directories: bin obj obj/obj obj/pvm obj/opengl obj/socket \
obj/socketgl \
obj/ga_obj obj/ga_pvm obj/ga_socket

binaries: \
bin/objective bin/objective_pvm bin/objective_opengl \
bin/objective_socket bin/objective_socketgl \
bin/ga bin/ga_pvm bin/ga_socket \
bin/objective bin/objective_pvm bin/objective_socket 

libraries: dynamechs glui

obj/obj: 
	mkdir obj/obj

obj/pvm: 
	mkdir obj/pvm

obj/opengl: 
	mkdir obj/opengl

obj/socket:
	mkdir obj/socket

obj/socketgl:
	mkdir obj/socketgl

obj/ga_obj: 
	mkdir obj/ga_obj

obj/ga_pvm: 
	mkdir obj/ga_pvm

obj/ga_socket:
	mkdir obj/ga_socket

bin:
	mkdir bin

obj:
	mkdir obj
	 
obj/obj/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(DYN_INC) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective: $(addprefix obj/obj/, $(SYMOBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS) $(LIBS)

obj/pvm/%.o : %.cc
	$(CXX) -DUSE_PVM $(CXXFLAGS) $(DYN_INC) $(PVM_INC) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_pvm: $(addprefix obj/pvm/, $(SYMOBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS) $(PVM_LIBS) $(LIBS)

obj/opengl/%.o : %.cc
	$(CXX) -DUSE_OPENGL $(CXXFLAGS) $(DYN_INC) $(INC_DIRS) $(OPENGL_INC) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_opengl: $(addprefix obj/opengl/, $(SYMOBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS_OPENGL) $(OPENGL_LIBS) $(LIBS)

obj/socket/%.o : %.cc
	$(CXX) -DUSE_SOCKETS $(CXXFLAGS) $(DYN_INC) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_socket: $(addprefix obj/socket/, $(SYMOBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS) $(SOCKET_LIBS) $(LIBS)

obj/socketgl/%.o : %.cc
	$(CXX) -DUSE_SOCKETS -DUSE_OPENGL $(CXXFLAGS) $(DYN_INC) $(OPENGL_INC) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_socketgl: $(addprefix obj/socketgl/, $(SYMOBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS_OPENGL) $(SOCKET_LIBS) $(OPENGL_LIBS) $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	( cd src; rm -f *~ *.bak *.bck *.tmp *.o )

superclean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	( cd src; rm -f *~ *.bak *.bck *.tmp *.o )
	( cd dynamechs_4.0pre1 ; make clean )
	( cd drand48; make clean )
	( cd glui_v2_1_beta ; make clean )
	rm -rf project_builder/build project_builder/GaitGen.pbproj/*.pbxuser

installpvm:
	cp -f bin/objective_pvm $(PVM_INSTALL)
	cp -f bin/ga_pvm $(PVM_INSTALL)

dynamechs: 
	( cd dynamechs_4.0pre1 ; make all )

glui: 
	( cd glui_v2_1_beta ; make all )

GA_OBJECTS = GA.o Genome.o Objective.o Random.o Mating.o Statistics.o \
Population.o DataFile.o

obj/ga_pvm/%.o : %.cc
	$(CXX) -DUSE_PVM $(CXXFLAGS) $(PVM_INC) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/ga_pvm: $(addprefix obj/ga_pvm/, $(GA_OBJECTS) )
	$(CXX) -o $@ $^ $(PVM_LIBS) $(LIBS)

obj/ga_socket/%.o : %.cc
	$(CXX) -DUSE_SOCKETS $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/ga_socket: $(addprefix obj/ga_socket/, $(GA_OBJECTS) )
	$(CXX) -o $@ $^ $(SOCKET_LIBS) $(LIBS)

obj/ga_obj/%.o : %.cc
	$(CXX) -DUSE_FS $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/ga: $(addprefix obj/ga_obj/, $(GA_OBJECTS) )
	$(CXX) -o $@ $^ $(LIBS)


