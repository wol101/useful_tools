
HDRS= Bar.h             ForceList.h  LoggedForce.h  SinController.h \
Controller.h      GripModel.h  MuscleModel.h  Util.h \
ControllerList.h  LoadFile.h   Simulation.h

SRCS= Bar.cpp             ForceList.cpp  LoggedForce.cpp  SinController.cpp \
Controller.cpp      GripModel.cpp  MuscleModel.cpp  Util.cpp \
ControllerList.cpp  LoadFile.cpp   Simulation.cpp   main.cpp

OBJS= Bar.o             ForceList.o  LoggedForce.o  SinController.o \
Controller.o      GripModel.o  MuscleModel.o  Util.o \
ControllerList.o  LoadFile.o   Simulation.o   main.o

DYN_DIR = $(HOME)/Unix/src/dynamechs_4.0pre1/
INC_DIRS= -I. -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL -I/usr/X11R6/include -I/usr/X11R6/include/GL -I/usr/include/GL -I$(HOME)/Unix/src/glut3.7/include -I$(HOME)/Unix/src/glut3.7/include/GL
LIBS = $(DYN_DIR)lib/platform.darwin.ppc/libdmGL.a $(DYN_DIR)lib/platform.darwin.ppc/libdmu.a $(DYN_DIR)lib/platform.darwin.ppc/libdm.a 
OPENGL_LIBS = -L/usr/X11R6/lib -L$(HOME)/Unix/src/glut3.7/lib/ -lglut -lGLU -lGL -lXmu -lXext -lX11

.SUFFIXES: .cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<
        
gibbon: $(OBJS)
	$(CXX) -o $@ $^ $(LIBS) -lm $(OPENGL_LIBS)

clean:
	$(RM) gibbon
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) $(SRCS)

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

