# makefile for the subprocess objective option

# get the system name
SYSTEM = $(shell uname)

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -ptused 

OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 \
	-lglut -lGLU -lGL -lX11 -lXmu
OPENGL_INC = -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
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
CXX         = g++
CXXFLAGS    = -Wall -fexceptions -g

OPENGL_LIBS = -L/usr/local/lib -L/usr/X11R6/lib \
	-lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I/usr/local/include/ -I/usr/local/include/GL/ \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
DYN_DIR = $(HOME)/Unix/dynamechs/
endif

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


DYN_DIR ?= $(HOME)/dynamechs/
INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL \
	$(OPENGL_INC)
DYN_LIBS = $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dmGL/libdmGL.a \
	$(DYN_DIR)dm/libdm.a 
LIBS = -lm 

all: objective

objective: \
Simulation.o           Fitness.o              ModifiedContactModel.o\
Controller.o           SinController.o        StepController.o\
ControllerList.o       ObjectiveMain.o        ControllerData.o\
LoggedForce.o          Util.o                 SquareController.o\
ForceList.o            MuscleModel.o          StrapForce.o\
Segments.o             ParameterFile.o        StrapForceAnchor.o
	$(CXX) -o $@ $^ $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)

clean:
	rm -f objective
	rm -f *.o *~ *.bak core *.bck
	rm -rf ii_files

