# makefile for the subprocess objective option

# get the system name
SYSTEM = $(shell uname)

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -ptused 

OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 -L$(GLUI_DIR)/lib \
	-lglui -lglut -lGLU -lGL -lX11 -lXmu
OPENGL_INC = -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL \
	-I/usr/share/src/OpenGL/toolkits/include -I$(GLUI_DIR)
DYN_DIR = $(HOME)/src/dynamechs_4.0pre1/
DYN_LIB = $(DYN_DIR)/lib/platform.irix65.r5/
GLUI_DIR = $(HOME)/src/glui_v2_1_beta/
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
CXXFLAGS    = -Wall -fexceptions -g

OPENGL_LIBS = -L/usr/X11R6/lib \
	-lglut -lMesaGLU -lMesaGL -lXmu -lXi -lX11
OPENGL_INC = -I/usr/X11R6/include -I/usr/X11R6/include/GL -I/usr/X11R6/include/X11/mesa/GL/
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++
# enable almost all warnings except -Waggregate-return
CXXFLAGS    = -Wall -g 

# enable (almost) all the optional warnings that -Wall misses
# -Wall -Wmost-W -Wtraditional -Wshadow -Wpointer-arith \
# -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings \
# -Wconversion -Waggregate-return -Wstrict-prototypes \
# -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls \
# -Wnested-externs -Winline -Woverloaded-virtual -Wsynth

OPENGL_LIBS = -L${GLUI_DIR}/lib -L${GLUT_DIR}/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I${GLUI_DIR} \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
DYN_DIR = $(HOME)/Unix/src/dynamechs_4.0pre1/
DYN_LIB = $(DYN_DIR)/lib/platform.darwin.ppc/
GLUI_DIR = $(HOME)/Unix/src/glui_v2_1_beta/
GLUT_DIR = $(HOME)/Unix/src/glut3.7/

endif

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL \
	$(OPENGL_INC)
DYN_LIBS = $(DYN_LIB)libdmu.a $(DYN_LIB)libdmGL.a \
	$(DYN_LIB)libdm.a 
LIBS = -lm 

all: objective

objective: \
CPG.o                    ObjectiveMain.o          StrapForce.o \
ExtendedForce.o          ParameterFile.o          StrapForceAnchor.o \
ForceList.o              Segments.o               Util.o \
LadderContactModel.o     Simulation.o             MAMuscle.o \
LoadObj.o                Ladder.o                 GLUIRoutines.o
	$(CXX) -o $@ $^ $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)

clean:
	rm -f objective
	rm -f *.o *~ *.bak core *.bck *.tmp
	rm -rf ii_files

