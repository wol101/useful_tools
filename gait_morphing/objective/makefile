# makefile for the subprocess objective option

# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3

# change this in shell 

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC -LANG:std
CXXFLAGS    = -fullwarn -n32 -ptused $(OPT_FLAGS)
INC_DIRS    = -I/usr/local/include

OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 -L$(GLUI_DIR)/lib \
	-lglui -lglut -lGLU -lGL -lX11 -lXmu
OPENGL_INC = -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL \
	-I/usr/share/src/OpenGL/toolkits/include -I$(GLUI_DIR)
GLUI_DIR = $(HOME)/src/glui_v2_1_beta/
SOCKET_LIBS = -L/usr/local/lib -lptypes
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 
# extras for Athlon-MP system
ifeq ($(HOST),linux-huwis.lboro.ac.uk)
CXXFLAGS += -funroll-loops -fomit-frame-pointer -ffast-math -march=athlon-mp -mfpmath=sse
endif

OPENGL_LIBS = -L/usr/X11R6/lib -L$(GLUI_DIR)/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXi -lX11
OPENGL_INC = -I/usr/X11R6/include -I/usr/X11R6/include/GL -I/usr/X11R6/include/X11/mesa/GL/ -I$(GLUI_DIR)
GLUI_DIR = $(HOME)/src/glui_v2_1_beta/
endif

ifeq ($(SYSTEM),SunOS)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)
INC_DIRS    = -I$(HOME)/include
SOCKET_LIBS = -L$(HOME)/lib -lptypes -lsocket -lnsl -lrt
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 
# suggested by linker
LDFLAGS = -Xlinker -bind_at_load

OPENGL_LIBS = -L${GLUI_DIR}/lib -L${GLUT_DIR}/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I${GLUI_DIR} \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUI_DIR = $(HOME)/Unix/src/glui_v2_1_beta/
GLUT_DIR = $(HOME)/Unix/src/glut3.7/
endif

ifeq ($(SYSTEM),CYGWIN_NT-4.0)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 

OPENGL_LIBS = -L${GLUI_DIR}/lib -L${GLUT_DIR}/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I${GLUI_DIR} \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUI_DIR = $(HOME)/src/glui_v2_1_beta/
GLUT_DIR = /usr/local/
PVM_LIBS = -L$(PVM_ROOT_U)/lib/$(PVM_ARCH)/ -lpvm3
PVM_INC = -I$(PVM_ROOT_U)/include/
PVM_INSTALL = $(PVM_ROOT_U)/bin/$(PVM_ARCH)
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.0)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++-2
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 

OPENGL_LIBS = -L${GLUI_DIR}/lib -L${GLUT_DIR}/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I${GLUI_DIR} \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUI_DIR = $(HOME)/src/glui_v2_1_beta/
GLUT_DIR = /usr/local/
PVM_LIBS = -L$(PVM_ROOT_U)/lib/$(PVM_ARCH)/ -lpvm3
PVM_INC = -I$(PVM_ROOT_U)/include/
PVM_INSTALL = $(PVM_ROOT_U)/bin/$(PVM_ARCH)
SOCKET_LIBS = -L/usr/local/lib -lptypes
endif

DYN_DIR = dynamechs_4.0pre1/

INC_DIRS += -I. -I$(DYN_DIR)dm \
	$(OPENGL_INC) $(PVM_INC)
LIBS = -lm 

PVM_LIBS ?= -L$(PVM_ROOT)/lib/$(PVM_ARCH)/ -lpvm3
PVM_INC ?= -I$(PVM_ROOT)/include/
PVM_INSTALL ?= $(PVM_ROOT)/bin/$(PVM_ARCH)

DYN_LIBS = $(DYN_DIR)/dm/libdm.a 
DYN_LIBS_OPENGL = $(DYN_DIR)/dm/libdm_opengl.a 

SOCKET_LIBS ?= -lptypes

CCFILES = \
CPG.cc                    MAMuscle.cc               StrapForce.cc \
ModifiedContactModel.cc   StrapForceAnchor.cc \
ForceList.cc              ObjectiveMain.cc          \
GLUIRoutines.cc           Segments.cc               \
LoadObj.cc                Simulation.cc             SideStabilizer.cc \
DataFile.cc

OBJECTS = $(addsuffix .o, $(basename $(CCFILES) ) )

vpath %.cc src 

all: dynamechs directories binaries

dynamechs:
	( cd dynamechs_4.0pre1 ; make all )

directories: bin obj obj/obj obj/pvm obj/opengl obj/socket obj/socketgl obj/merge_config_files

binaries: \
bin/objective bin/objective_pvm bin/objective_opengl bin/objective_socket bin/objective_socketgl bin/merge_config_files

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

obj/merge_config_files:
	mkdir obj/merge_config_files

bin:
	mkdir bin

obj:
	mkdir obj
        
obj/obj/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective: $(addprefix obj/obj/, $(OBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS) $(LIBS)

obj/pvm/%.o : %.cc
	$(CXX) -DUSE_PVM $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_pvm: $(addprefix obj/pvm/, $(OBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS) $(PVM_LIBS) $(LIBS)

obj/opengl/%.o : %.cc
	$(CXX) -DUSE_OPENGL $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_opengl: $(addprefix obj/opengl/, $(OBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS_OPENGL) $(SOCKET_LIBS) $(OPENGL_LIBS) $(LIBS)

obj/socket/%.o : %.cc
	$(CXX) -DUSE_SOCKETS $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_socket: $(addprefix obj/socket/, $(OBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS) $(SOCKET_LIBS) $(LIBS)

obj/socketgl/%.o : %.cc
	$(CXX) -DUSE_SOCKETS -DUSE_OPENGL $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/objective_socketgl: $(addprefix obj/socketgl/, $(OBJECTS) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(DYN_LIBS_OPENGL) $(SOCKET_LIBS) $(OPENGL_LIBS) $(LIBS)

obj/merge_config_files/%.o : %.cc
	$(CXX) $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

bin/merge_config_files: $(addprefix obj/merge_config_files/, MergeConfigFiles.o DataFile.o )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	( cd src; rm -f *~ *.bak *.bck *.tmp *.o )
	( cd dynamechs_4.0pre1 ; make clean )

install_pvm:
	cp -f bin/objective_pvm $(PVM_INSTALL)
