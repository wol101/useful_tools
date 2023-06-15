# makefile for the subprocess objective option

# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug and -O3 for optimisation
#OPT_FLAGS ?= -O3
OPT_FLAGS ?= -g

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -fullwarn -n32 -ptused $(OPT_FLAGS)

OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 -L$(GLUI_DIR)/lib \
	-lglui -lglut -lGLU -lGL -lX11 -lXmu
OPENGL_INC = -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL \
	-I/usr/share/src/OpenGL/toolkits/include -I$(GLUI_DIR)
GLUI_DIR = $(HOME)/src/glui_v2_1_beta/
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)

OPENGL_LIBS = -L/usr/X11R6/lib -L$(GLUI_DIR)/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXi -lX11
OPENGL_INC = -I/usr/X11R6/include -I/usr/X11R6/include/GL -I/usr/X11R6/include/X11/mesa/GL/ -I$(GLUI_DIR)
GLUI_DIR = $(HOME)/src/glui_v2_1_beta/
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 

OPENGL_LIBS = -L${GLUI_DIR}/lib -L${GLUT_DIR}/lib -L/usr/X11R6/lib \
	-lglui -lglut -lGLU -lGL -lXmu -lXext -lX11
OPENGL_INC = -I${GLUI_DIR} \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
GLUI_DIR = $(HOME)/Unix/src/glui_v2_1_beta/
GLUT_DIR = $(HOME)/Unix/src/glut3.7/
LIBS = -lm 
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.0)
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
endif

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) $(OPENGL_INC) -c $<

OBJECTS = ParameterFile.o DisplayGenealogy.o Cohort.o Axis.o StrokeFont.o \
write_dxf.o

all: DisplayGenealogy

DisplayGenealogy: $(OBJECTS)    
	$(CXX) -o $@ $(OBJECTS) $(OPENGL_LIBS) -lm

clean:
	rm -f *.o ga objective *.bak *.bck *~ core *.tmp
	rm -rf ii_files
	rm -f DisplayGenealogy DisplayGenealogy.exe
