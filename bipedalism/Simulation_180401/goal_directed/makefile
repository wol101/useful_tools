# makefile for the subprocess objective option

# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -DUSE_OLD_STREAMS -ptused 
CXX_INC_DIR = /usr/include/CC
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<


DYN_DIR = /users/ws/dynamechs/
GA_DIR = /users/ws/galib/
INC_DIRS= -I. -I$(GA_DIR) -I$(DYN_DIR)dm -I$(DYN_DIR)dmu -I$(DYN_DIR)dmGL \
	-I/usr/include/GL -I/usr/share/src/OpenGL/toolkits/include/GL
DYN_LIBS = $(DYN_DIR)dmu/libdmu.a $(DYN_DIR)dmGL/libdmGL.a \
	$(DYN_DIR)dm/libdm.a 
LIBS = -lm 
OPENGL_LIBS = -L/usr/share/src/OpenGL/toolkits/lib32 \
	-lglut -lGLU -lGL -lX11 -lXmu
GA_LIBS = $(GA_DIR)ga/libga.a

all: ga objective
	
objective: \
Simulation.o           Fitness.o              ModifiedContactModel.o\
Controller.o           SinController.o        StepController.o\
ControllerList.o       LoadFile.o             ObjectiveMain.o\
LoggedForce.o          Util.o                 SquareController.o\
ForceList.o            MuscleModel.o          strtok.o
	$(CXX) -o $@ $^ $(GA_LIBS) $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)

ga: \
GA.o        Util.o              Objective.o
	$(CXX) -o $@ $^ $(GA_LIBS) $(DYN_LIBS) $(OPENGL_LIBS) $(LIBS)

clean:
	$(RM) ga objective
	$(RM) *.o *~ *.bak core *.bck

depend:
	$(MKDEPEND) -I$(CXX_INC_DIR) $(INC_DIRS) \
GA.cc                   ObjectiveMain.cc        ModifiedContactModel.cc\
Controller.cc           Simulation.cc           StepController.cc\
ControllerList.cc       LoadFile.cc             SinController.cc\
Fitness.cc              LoggedForce.cc          Objective.cc\
ForceList.cc            MuscleModel.cc          Util.cc\
SquareController.cc

# DO NOT DELETE THIS LINE -- make  depend  depends  on it.

GA.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
GA.o: /usr/include/gnu/stubs.h
GA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
GA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
GA.o: /usr/include/bits/types.h /usr/include/libio.h /usr/include/_G_config.h
GA.o: /usr/include/bits/stdio_lim.h /usr/include/assert.h
GA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/exception
GA.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
GA.o: /usr/include/endian.h /usr/include/bits/endian.h
GA.o: /usr/include/sys/select.h /usr/include/bits/select.h
GA.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
GA.o: /usr/include/alloca.h Util.h /usr/include/math.h
GA.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
GA.o: /usr/include/bits/mathcalls.h
GA.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
ObjectiveMain.o: /usr/include/stdio.h /usr/include/features.h
ObjectiveMain.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ObjectiveMain.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
ObjectiveMain.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
ObjectiveMain.o: /usr/include/bits/types.h /usr/include/libio.h
ObjectiveMain.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
ObjectiveMain.o: /usr/include/assert.h /usr/include/unistd.h
ObjectiveMain.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
ObjectiveMain.o: /usr/include/getopt.h Fitness.h Util.h /usr/include/math.h
ObjectiveMain.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
ObjectiveMain.o: /usr/include/bits/mathcalls.h
ObjectiveMain.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
ObjectiveMain.o: Simulation.h ForceList.h Controller.h ControllerList.h
ModifiedContactModel.o: ForceList.h Simulation.h ModifiedContactModel.h
ModifiedContactModel.o: ./LoggedForce.h
Controller.o: Controller.h ControllerList.h Simulation.h
Simulation.o: /usr/include/assert.h /usr/include/features.h
Simulation.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h LoadFile.h
Simulation.o: ForceList.h ControllerList.h Simulation.h
StepController.o: /usr/include/math.h /usr/include/features.h
StepController.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
StepController.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
StepController.o: /usr/include/bits/mathcalls.h
StepController.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
StepController.o: /usr/include/assert.h StepController.h Controller.h
ControllerList.o: Controller.h ControllerList.h
LoadFile.o: LoadFile.h SinController.h Controller.h SquareController.h
LoadFile.o: MuscleModel.h ./LoggedForce.h Simulation.h ModifiedContactModel.h
LoadFile.o: StepController.h
SinController.o: /usr/include/math.h /usr/include/features.h
SinController.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
SinController.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
SinController.o: /usr/include/bits/mathcalls.h
SinController.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
SinController.o: /usr/include/assert.h SinController.h Controller.h
Fitness.o: /usr/include/stdio.h /usr/include/features.h
Fitness.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Fitness.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
Fitness.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
Fitness.o: /usr/include/bits/types.h /usr/include/libio.h
Fitness.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
Fitness.o: /usr/include/assert.h /usr/include/math.h
Fitness.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Fitness.o: /usr/include/bits/mathcalls.h
Fitness.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
Fitness.o: ForceList.h Simulation.h Controller.h ControllerList.h
Fitness.o: ./LoggedForce.h Util.h Fitness.h ModifiedContactModel.h
LoggedForce.o: /usr/include/stdio.h /usr/include/features.h
LoggedForce.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
LoggedForce.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
LoggedForce.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
LoggedForce.o: /usr/include/bits/types.h /usr/include/libio.h
LoggedForce.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
LoggedForce.o: /usr/include/assert.h /usr/include/string.h ./LoggedForce.h
Objective.o: /usr/include/stdio.h /usr/include/features.h
Objective.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Objective.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
Objective.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
Objective.o: /usr/include/bits/types.h /usr/include/libio.h
Objective.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
Objective.o: /usr/include/assert.h Util.h /usr/include/math.h
Objective.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Objective.o: /usr/include/bits/mathcalls.h
Objective.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
ForceList.o: ForceList.h
MuscleModel.o: /usr/include/assert.h /usr/include/features.h
MuscleModel.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
MuscleModel.o: MuscleModel.h ./LoggedForce.h Controller.h Util.h
MuscleModel.o: /usr/include/math.h /usr/include/bits/huge_val.h
MuscleModel.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
MuscleModel.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
MuscleModel.o: ForceList.h Simulation.h
Util.o: /usr/include/math.h /usr/include/features.h /usr/include/sys/cdefs.h
Util.o: /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h
Util.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
Util.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
Util.o: /usr/include/stdio.h
Util.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stddef.h
Util.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/stdarg.h
Util.o: /usr/include/bits/types.h /usr/include/libio.h
Util.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
Util.o: /usr/include/assert.h /usr/include/string.h /usr/include/errno.h
Util.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
Util.o: /usr/include/asm/errno.h /usr/include/sys/types.h /usr/include/time.h
Util.o: /usr/include/endian.h /usr/include/bits/endian.h
Util.o: /usr/include/sys/select.h /usr/include/bits/select.h
Util.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
Util.o: /usr/include/sys/wait.h /usr/include/bits/waitflags.h
Util.o: /usr/include/bits/waitstatus.h /usr/include/bits/siginfo.h
Util.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
Util.o: /usr/include/bits/confname.h /usr/include/getopt.h Util.h
SquareController.o: /usr/include/math.h /usr/include/features.h
SquareController.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
SquareController.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
SquareController.o: /usr/include/bits/mathcalls.h
SquareController.o: /usr/lib/gcc-lib/i586-mandrake-linux/2.95.3/include/float.h
SquareController.o: /usr/include/assert.h SquareController.h Controller.h
