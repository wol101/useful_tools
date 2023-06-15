SYSTEM = $(shell uname)

# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c

OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 \
        -lGL -lX11 -lXmu -lXt
OPENGL_INC = -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
PRELINK     =
CXXFLAGS    = -Wall -fexceptions -g
CXX_INC_DIR = /usr/include/g++-3/
LD          = g++ -w
AR          = ar rv
INSTALL     = install -c                       

OPENGL_LIBS = -L/usr/X11R6/lib \
        -lglut -lMesaGLU -lMesaGL -lXmu -lXi -lX11 -lXt
OPENGL_INC = -I/usr/X11R6/include -I/usr/X11R6/include/GL \
        -I/usr/X11R6/include/X11/mesa/GL/
VTK_LIBS = -L$(VTKDIR)/lib -lVTKGraphics -lVTKImaging \
        -lVTKCommon -lVTKContrib -lVTKPatented
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = g++
PRELINK     =
CXXFLAGS    = -Wall -fexceptions -O3
CXX_INC_DIR = /usr/include/gcc/darwin/2.95.2/g++/
LD          = g++ -w
AR          = ar rv
INSTALL     = install -c                       

OPENGL_LIBS = /usr/local/lib/libGL.a -L/usr/X11R6/lib \
	-lXext -lXt -lSM -lICE -lX11 
OPENGL_INC = -I/usr/local/include/ -I/usr/local/include/GL/ \
        -I/usr/X11R6/include -I/usr/X11R6/include/GL 
VTKDIR = $(HOME)/Unix/src/vtk3.2
VTK_LIBS = -lVTKGraphics -lVTKImaging \
        -lVTKCommon -lVTKContrib -lVTKPatented -lVTKCommon
endif


INC_DIRS = -I$(VTKDIR)/graphics -I$(VTKDIR)/imaging -I$(VTKDIR)/common \
        -I$(VTKDIR)/contrib -I$(VTKDIR)/patented
        
LIBS = -lpthread -lm 

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<



all: decimator


decimator: decimator.o
	$(CXX) -o $@ $^ $(VTK_LIBS) $(OPENGL_LIBS) $(LIBS) 
        
        
clean:
	rm -f *.o *.bak *.bck *~ core *.tmp
	rm -rf ii_files
	rm -f viewer
        
