# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3

# change this in shell 

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -fullwarn -n32 -ptused $(OPT_FLAGS)
AR          = ar rv
# dummy value for ranlib since it is not needed on Irix
RANLIB      = echo

INC_DIRS    = -I/usr/local/include
OPENGL_INC = -I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL \
	-I/usr/share/src/OpenGL/toolkits/include 
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)
# extras for Athlon-MP system
ifeq ($(HOST),linux-huwis.lboro.ac.uk)
CXXFLAGS += -funroll-loops -fomit-frame-pointer -ffast-math -march=athlon-mp -mfpmath=sse
endif

AR          = ar rv
RANLIB      = ranlib
OPENGL_INC = -I/usr/X11R6/include -I/usr/X11R6/include/GL \
	-I/usr/X11R6/include/X11/mesa/GL/ 
endif

ifeq ($(SYSTEM),SunOS)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS)
AR          = ar rv
RANLIB      = ranlib

endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 
AR          = ar rv
RANLIB      = ranlib

OPENGL_INC =  \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
endif

ifeq ($(SYSTEM),CYGWIN_NT-4.0)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 
AR          = ar rv
RANLIB      = ranlib

OPENGL_INC =  \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.0)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = c++-2
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) 
AR          = ar rv
RANLIB      = ranlib

OPENGL_INC = -I${GLUI_DIR} \
	-I${GLUT_DIR}/include -I${GLUT_DIR}/include/GL \
	-I/usr/X11R6/include -I/usr/X11R6/include/GL 
endif

INC_DIRS += -I.

CPPFILES = \
	dmObject.cpp \
	dmSystem.cpp \
	dmArticulation.cpp \
	dmClosedArticulation.cpp \
	dmLink.cpp \
	dmStaticRootLink.cpp \
	dmZScrewTxLink.cpp \
	dmRigidBody.cpp \
	dmMobileBaseLink.cpp \
	dmSphericalLink.cpp \
	dmQuaternionLink.cpp \
	dmMDHLink.cpp \
	dmPrismaticLink.cpp \
	dmRevoluteLink.cpp \
	dmActuator.cpp \
	dmRevDCMotor.cpp \
	dmEnvironment.cpp \
	dmTreadmill.cpp \
	dmForce.cpp \
	dmContactModel.cpp \
	dmSecondaryForce.cpp \
	dmSecondaryJoint.cpp \
	dmSecondarySphericalJoint.cpp \
	dmSecondaryPrismaticJoint.cpp \
	dmSecondaryRevoluteJoint.cpp \
	dmIntegrator.cpp \
	dmIntegEuler.cpp \
	dmIntegRK4.cpp \
	dmIntegRK45.cpp \
	svd_linpack.cpp \
	gldraw.cpp \
        dmUtils.cpp

OBJFILES = $(addsuffix .o, $(basename $(CPPFILES) ) )

all: obj obj_opengl libdm.a libdm_opengl.a

obj:
	mkdir obj

obj_opengl:
	mkdir obj_opengl

libdm.a: $(addprefix obj/, $(OBJFILES) )
	$(AR) $@ $^
	$(RANLIB) $@

libdm_opengl.a: $(addprefix obj_opengl/, $(OBJFILES) )
	$(AR) $@ $^
	$(RANLIB) $@

obj/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

obj_opengl/%.o : %.cpp
	$(CXX) -DUSE_OPENGL $(CXXFLAGS) $(OPENGL_INC) $(INC_DIRS) $(EXTRA_FLAGS) -c $< -o $@

clean:
	rm -f libdm.a libdm_opengl.a
	rm -rf obj obj_opengl
	rm -f *.o *.bak *.bck *~ core *.tmp *.exe
	rm -rf ii_files
