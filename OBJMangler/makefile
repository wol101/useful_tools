# makefile for the subprocess objective option

# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug and -O3 for optimisation
OPT_FLAGS ?= -O3 -ffast-math

ifeq ($(SYSTEM),Linux)
# GNU compiler flags 
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
OPENGL_LIBS += -L"$(HOME)/Unix/lib" -lglui \
        -lglut -lGLU -lGL \
        -L/usr/X11R6/lib -lXmu -lXext -lX11 -lXi
OPENGL_INC += -I"$(HOME)/Unix/include" -I/usr/local/include/GL -I/usr/X11R6/include/GL -I/usr/X11R6/include 
# extras for Beowulf compilation
ifeq ($(HOSTNAME),ulgbc1.liv.ac.uk)
OPT_FLAGS = -O3 -ffast-math -march=k6
endif
ifeq ($(HOSTNAME),microlith)
#OPT_FLAGS += -march=opteron
OPT_FLAGS = -O3 -ffast-math
endif
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs)
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
# suggested by linker
LDFLAGS = -Xlinker -bind_at_load 
OPENGL_LIBS += -L"$(HOME)/Unix/lib" -lglui \
        -L/usr/X11R6/lib -framework GLUT -lGLU -lGL \
        -lXmu -lXext -lX11 -lXi \
-dylib_file \
/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:\
/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
OPENGL_INC += -I/System/Library/Frameworks/GLUT.framework/Versions/A/Headers \
-I"$(HOME)/Unix/include/glui" -I"$(HOME)/Unix/include" -I/usr/local/include/GL -I/usr/X11R6/include/GL -I/usr/X11R6/include 
ifeq ($(OPT_FLAGS), -g)
LIBS = -lMallocDebug
endif
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.1)
# GNU compiler flags 
CXXFLAGS = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
OPENGL_LIBS += -L"$(HOME)/Unix/lib" -lglui \
        -lglut -lGLU -lGL \
        -L/usr/X11R6/lib -lXmu -lXext -lX11 -lXi
OPENGL_INC += -I"$(HOME)/Unix/include" -I/usr/local/include/GL -I/usr/X11R6/include/GL -I/usr/X11R6/include 
endif


# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


INC_DIRS= -I. $(OPENGL_INC)
LIBS ?= -lode -lANN -lpthread -lm 
BIN_DEST ?= $(HOME)/Unix/bin/
EXTRA_FLAGS = -DUSE_ODE -DdDOUBLE -DdTRIMESH_ENABLED

all: sort/sortlib.a OBJManglerGUI

sort/sortlib.a:
	(cd sort; make)

OBJManglerGUI: \
GLUIRoutines.o FacetedObject.o MyFace.o OBJMangler.o TextFile.o \
Shapes.o Intersection.o VoxelObject.o FacetedObjectList.o l3ds.o \
MarchingCubes.o Dicom.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(OPENGL_LIBS) $(LIBS)

clean:
	rm -f OBJMangler
	rm -f *.o *~ *.bak core *.bck *.tmp *.exe
	rm -rf ii_files

install: OBJManglerGUI
	cp -f OBJManglerGUI $(BIN_DEST)

