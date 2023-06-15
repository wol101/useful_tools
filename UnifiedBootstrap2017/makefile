# get the system and architecture
ifeq ($(OS),Windows_NT)
	SYSTEM := WIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		ARCH := AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
		ARCH := AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		ARCH := IA32
	endif
	# this will work on cygwin systems
	UNAME_S := $(shell uname -s)
	ifneq (,$(findstring CYGWIN,$(UNAME_S)))
		SYSTEM := CYGWIN
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		SYSTEM := LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		SYSTEM := OSX
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		ARCH := AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		ARCH := IA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		ARCH := ARM
	endif
	ifneq ($(filter ppc%,$(UNAME_P)),)
		ARCH := PPC
	endif
endif
HOST := $(shell hostname)
MPICC_AVAIL := $(shell command -v mpicc 2> /dev/null)

ifeq ($(SYSTEM),OSX)
	# run using 'mpirun -np _nproc_ _executable_ _arguments_'
	# mpicxx --showme
	# g++ -I/usr/local/include -L/usr/local/lib -lmpi_cxx -lmpi -lm
	CXXFLAGS = -g -O0 -DEXPERIMENTAL -DUSE_PCA -DUSE_CBLAS -DUSE_MPI_ABORT -std=c++11 -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DUSE_UNIX_ERRORS
	#CXXFLAGS = -ffast-math -O3  -DEXPERIMENTAL
	LDFLAGS  = -framework Accelerate
	CXX      = mpicxx
	CC       = mpicc
	LIBS = -lm
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA \
	-I/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers
endif

ifeq ($(SYSTEM),LINUX)
	CXXFLAGS = -static -O3  -DEXPERIMENTAL -std=c++11 -DUSE_MPI_ABORT -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
	#CXXFLAGS = -static -O3  -DEXPERIMENTAL -std=c++11 -DUSE_PCA -DUSE_CBLAS -DUSE_F2C -DUSE_MPI_ABORT
ifdef MPICC_AVAIL
	CXX      = mpicxx
	CC       = mpicc
else
	CXX      = CC
	CC       = cc
	LDFLAGS  = -static
endif
	LIBS = -lm
	#LIBS = -L"$(HOME)/Unix/lib" -lode -lANN -lxml2 -lpthread -lm -lz -llapack_LINUX -lf2c
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

# hostname specific changes
ifeq ($(HOST),icalworkstation)
	CXXFLAGS = -O3 -g -ffast-math  -std=c++11 -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
	LDFLAGS  =
	CXX      = mpic++
	CC       = mpicc
	LIBS = -lm
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

ifneq (,$(findstring polaris.leeds.ac.uk,$(HOST)))
	# polaris uses the intel compiler icpc and needs a new version that supports -std=c++11
	# module swap intel/12.1.5.339 intel/15.0.3
	CXXFLAGS = -O3 -fast  -std=c++11 -no-ipo -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
	LDFLAGS  = -no-ipo
	CXX      = mpic++
	CC       = mpicc
	LIBS = -lm
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

ifeq ($(HOST),asl-rocks-cluster.manchester.ac.uk)
	# rocks version of g++ is too old to support -std=c++11
	CXXFLAGS = -O3 -g -ffast-math  -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
	LDFLAGS  =
	CXX      = mpic++
	CC       = mpicc
	LIBS = -lm
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

ifneq (,$(findstring idb1,$(HOST)))
	# this is the hartree Xeon Phi system
	# it uses the intel versions of the mpi commands which have ii in the middle
	# module load intel/15.1.133_mic intel_mpi/5.0.2_mic
	#CXXFLAGS = -O3 -fast  -mmic -no-ipo
	CXXFLAGS = -O0  -mmic -no-ipo -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
	LDFLAGS  = -mmic -no-ipo
	CXX      = mpiicpc
	CC       = mpiicc
	LIBS = -lm
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

ifeq ($(HOST),login)
	# this is the archer knl system
	CXXFLAGS = -static -O3  -DEXPERIMENTAL -DUSE_MPI_ABORT -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
	#CXXFLAGS = -static -O3  -DEXPERIMENTAL -std=c++11 -DUSE_PCA -DUSE_CBLAS -DUSE_F2C -DUSE_MPI_ABORT
	LDFLAGS  = -static
	CXX      = CC
	CC       = cc
	LIBS = -lm
	#LIBS = -L"$(HOME)/knl_unix/lib" -lode -lANN -lxml2 -lpthread -lm -lz -llapack_LINUX -lf2c
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

ifeq ($(SYSTEM),CYGWIN)
	# cygwin64 version
	# mpicxx --showme
	# g++ -fexceptions -L/usr/lib -lmpi_cxx -lmpi -lopen-rte -lopen-pal -lm -lgdi32
	CXXFLAGS = -O3 -g -ffast-math  -Wa,-mbig-obj -Dexprtk_disable_enhanced_features -std=c++11 \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS -DHAVE_ALLOCA_H \
	-DALLOCA_H_NEEDED -DNEED_BCOPY -DSTRINGS_H_NEEDED
	LDFLAGS  =
	CXX      = mpic++
	CC       = mpicc
	LIBS = -lm
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

ifeq ($(HOST),login2)
	CXXFLAGS = -O3 -g -ffast-math  -DBACKTRACE_ERRORS \
	-DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
	-DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
	LDFLAGS  =
	CXX      = mpic++
	CC       = mpicc
	LIBS = -lm
	INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
	-I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif

ifneq (,$(findstring xenomorph,$(HOST)))
        # this is the Manchester experimental Xeon Phi system (xenomorph accessed through zrek
        # it needs the intel compilers and mpi loading
        # module load compilers/intel/14.0.3 mpi/intel-14.0/mpich2/3.1.4-mic
        CXXFLAGS = -O3 -fast -mmic -no-ipo -DBACKTRACE_ERRORS \
        -Dexprtk_disable_enhanced_features \
        -DdIDEDOUBLE -DTRIMESH_ENABLED -DTRIMESH_OPCODE -DCCD_IDEDOUBLE -DdLIBCCD_ENABLED -DdTHREADING_INTF_DISABLED \
        -DRAPIDXML_NO_EXCEPTIONS -DBYTE_ORDER=LITTLE_ENDIAN -DHAVE_MALLOC_H -DUSE_UNIX_ERRORS
        LDFLAGS  = -mmic -no-ipo
        CXX      = mpic++
        CC       = mpicc
        LIBS = -lm
        INC_DIRS = -I../GaitSym2017/rapidxml-1.13 -I../GaitSym2017/exprtk -I../GaitSym2017/ode-0.15/ode/src -I../GaitSym2017/ode-0.15/libccd/src \
        -I../GaitSym2017/ode-0.15/OPCODE -I../GaitSym2017/ode-0.15/include -I../GaitSym2017/ann_1.1.2/include -I../GaitSym2017/src -I../AsynchronousGA
endif


MPISRC = Bootstrap.cpp UnifiedMPI.cpp

GAITSYMSRC = \
AMotorJoint.cpp\
BallJoint.cpp\
Body.cpp\
BoxCarDriver.cpp\
BoxGeom.cpp\
ButterworthFilter.cpp\
CappedCylinderGeom.cpp\
Contact.cpp\
Controller.cpp\
CyclicDriver.cpp\
CylinderWrapStrap.cpp\
DampedSpringMuscle.cpp\
DataFile.cpp\
DataTarget.cpp\
DataTargetQuaternion.cpp\
DataTargetScalar.cpp\
DataTargetVector.cpp\
Drivable.cpp\
Driver.cpp\
Environment.cpp\
ErrorHandler.cpp\
Face.cpp\
FacetedConicSegment.cpp\
FacetedObject.cpp\
FacetedPolyline.cpp\
FacetedSphere.cpp\
Filter.cpp\
FixedDriver.cpp\
FixedJoint.cpp\
FloatingHingeJoint.cpp\
Geom.cpp\
GLUtils.cpp\
HingeJoint.cpp\
Joint.cpp\
MAMuscle.cpp\
MAMuscleComplete.cpp\
MAMuscleExtended.cpp\
Marker.cpp\
MovingAverage.cpp\
Muscle.cpp\
NamedObject.cpp\
NPointStrap.cpp\
ObjectiveMain.cpp\
PCA.cpp\
PIDMuscleLength.cpp\
PIDTargetMatch.cpp\
PlaneGeom.cpp\
PositionReporter.cpp\
RayGeom.cpp\
Reporter.cpp\
Simulation.cpp\
SliderJoint.cpp\
SphereGeom.cpp\
StackedBoxCarDriver.cpp\
StepDriver.cpp\
Strap.cpp\
StrokeFont.cpp\
SwingClearanceAbortReporter.cpp\
TegotaeDriver.cpp\
ThreePointStrap.cpp\
TIFFWrite.cpp\
TorqueReporter.cpp\
TrimeshGeom.cpp\
TwoCylinderWrapStrap.cpp\
TwoPointStrap.cpp\
UGMMuscle.cpp\
UniversalJoint.cpp\
Util.cpp\
Warehouse.cpp\
XMLConverter.cpp\
TegotaeDriver.cpp\
PIDErrorInController.cpp

MERGESRC = \
MergeExpressionParser.cpp MergeXML.cpp XMLContainer.cpp  XMLContainerList.cpp

AGASRC = \
DataFile.cpp Genome.cpp Mating.cpp Population.cpp Preferences.cpp Random.cpp Statistics.cpp GAMPI.cpp

LIBCCDSRC = \
alloc.c \
ccd.c \
mpr.c \
polytope.c \
support.c \
vec3.c

ODESRC = \
array.cpp \
box.cpp \
capsule.cpp \
collision_convex_trimesh.cpp \
collision_cylinder_box.cpp \
collision_cylinder_plane.cpp \
collision_cylinder_sphere.cpp \
collision_cylinder_trimesh.cpp \
collision_kernel.cpp \
collision_libccd.cpp \
collision_quadtreespace.cpp \
collision_sapspace.cpp \
collision_space.cpp \
collision_transform.cpp \
collision_trimesh_box.cpp \
collision_trimesh_ccylinder.cpp \
collision_trimesh_disabled.cpp \
collision_trimesh_distance.cpp \
collision_trimesh_gimpact.cpp \
collision_trimesh_opcode.cpp \
collision_trimesh_plane.cpp \
collision_trimesh_ray.cpp \
collision_trimesh_sphere.cpp \
collision_trimesh_trimesh_new.cpp \
collision_trimesh_trimesh.cpp \
collision_util.cpp \
convex.cpp \
cylinder.cpp \
error.cpp \
export-dif.cpp \
fastdot.cpp \
fastldlt.cpp \
fastlsolve.cpp \
fastltsolve.cpp \
heightfield.cpp \
lcp.cpp \
mass.cpp \
mat.cpp \
matrix.cpp \
memory.cpp \
misc.cpp \
nextafterf.c \
objects.cpp \
obstack.cpp \
ode.cpp \
odeinit.cpp \
odemath.cpp \
odeou.cpp \
odetls.cpp \
plane.cpp \
quickstep.cpp \
ray.cpp \
rotation.cpp \
sphere.cpp \
step.cpp \
threading_base.cpp \
threading_impl.cpp \
threading_pool_posix.cpp \
threading_pool_win.cpp \
timer.cpp \
util.cpp 

ODEJOINTSSRC = \
amotor.cpp \
ball.cpp \
contact.cpp \
dball.cpp \
dhinge.cpp \
fixed.cpp \
floatinghinge.cpp \
hinge.cpp \
hinge2.cpp \
joint.cpp \
lmotor.cpp \
null.cpp \
piston.cpp \
plane2d.cpp \
pr.cpp \
pu.cpp \
slider.cpp \
transmission.cpp \
universal.cpp

OPCODEICESRC = \
IceAABB.cpp \
IceContainer.cpp \
IceHPoint.cpp \
IceIndexedTriangle.cpp \
IceMatrix3x3.cpp \
IceMatrix4x4.cpp \
IceOBB.cpp \
IcePlane.cpp \
IcePoint.cpp \
IceRandom.cpp \
IceRay.cpp \
IceRevisitedRadix.cpp \
IceSegment.cpp \
IceTriangle.cpp \
IceUtils.cpp

OPCODESRC = \
OPC_AABBCollider.cpp \
OPC_AABBTree.cpp \
OPC_BaseModel.cpp \
OPC_Collider.cpp \
OPC_Common.cpp \
OPC_HybridModel.cpp \
OPC_LSSCollider.cpp \
OPC_MeshInterface.cpp \
OPC_Model.cpp \
OPC_OBBCollider.cpp \
OPC_OptimizedTree.cpp \
OPC_Picking.cpp \
OPC_PlanesCollider.cpp \
OPC_RayCollider.cpp \
OPC_SphereCollider.cpp \
OPC_TreeBuilders.cpp \
OPC_TreeCollider.cpp \
OPC_VolumeCollider.cpp \
Opcode.cpp \
StdAfx.cpp

ANNSRC = \
ANN.cpp \
bd_fix_rad_search.cpp \
bd_pr_search.cpp \
bd_search.cpp \
bd_tree.cpp \
brute.cpp \
kd_dump.cpp \
kd_fix_rad_search.cpp \
kd_pr_search.cpp \
kd_search.cpp \
kd_split.cpp \
kd_tree.cpp \
kd_util.cpp \
perf.cpp 

MPIOBJ = $(addsuffix .o, $(basename $(MPISRC) ) )
AGAOBJ = $(addsuffix .o, $(basename $(AGASRC) ) )
GAITSYMOBJ = $(addsuffix .o, $(basename $(GAITSYMSRC) ) )
MERGEOBJ = $(addsuffix .o, $(basename $(MERGESRC) ) )
LIBCCDOBJ = $(addsuffix .o, $(basename $(LIBCCDSRC) ) )
ODEOBJ = $(addsuffix .o, $(basename $(ODESRC) ) )
ODEJOINTSOBJ = $(addsuffix .o, $(basename $(ODEJOINTSSRC) ) )
OPCODEICEOBJ = $(addsuffix .o, $(basename $(OPCODEICESRC) ) )
OPCODEOBJ = $(addsuffix .o, $(basename $(OPCODESRC) ) )
ANNOBJ = $(addsuffix .o, $(basename $(ANNSRC) ) )

all: directories binaries

directories: bin obj

binaries: bin/UnifiedBootstrapAsync2017

obj:
	-mkdir obj
	-mkdir obj/gaitsym
	-mkdir obj/merge
	-mkdir obj/mpi
	-mkdir obj/aga
	-mkdir obj/libccd
	-mkdir obj/ode
	-mkdir obj/odejoints
	-mkdir obj/opcodeice
	-mkdir obj/opcode
	-mkdir obj/ann 

bin:
	-mkdir bin

obj/gaitsym/%.o : ../GaitSym2017/src/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/merge/%.o : ../MergeXML/src/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/aga/%.o : ../AsynchronousGA/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/mpi/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/libccd/%.o : ../GaitSym2017/ode-0.15/libccd/src/%.c
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/ode/%.o : ../GaitSym2017/ode-0.15/ode/src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/ode/%.o : ../GaitSym2017/ode-0.15/ode/src/%.c
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/odejoints/%.o : ../GaitSym2017/ode-0.15/ode/src/joints/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/opcodeice/%.o : ../GaitSym2017/ode-0.15/OPCODE/Ice/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/opcode/%.o : ../GaitSym2017/ode-0.15/OPCODE/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/ann/%.o :  ../GaitSym2017/ann_1.1.2/src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

bin/UnifiedBootstrapAsync2017: $(addprefix obj/gaitsym/, $(GAITSYMOBJ) ) $(addprefix obj/aga/, $(AGAOBJ) ) \
$(addprefix obj/mpi/, $(MPIOBJ) ) $(addprefix obj/merge/, $(MERGEOBJ) ) \
$(addprefix obj/libccd/, $(LIBCCDOBJ) ) $(addprefix obj/ode/, $(ODEOBJ) ) \
$(addprefix obj/odejoints/, $(ODEJOINTSOBJ) ) $(addprefix obj/opcodeice/, $(OPCODEICEOBJ) ) $(addprefix obj/opcode/, $(OPCODEOBJ) ) \
$(addprefix obj/ann/, $(ANNOBJ) ) 
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o

install: all
	cp -f bin/UnifiedBootstrapAsync2017 $(HOME)/work/
	cp -f bin/UnifiedBootstrapAsync2017 $(HOME)/Unix/bin/

knl_install: all
	cp -f bin/UnifiedBootstrapAsync2017 $(HOME)/knl_work/

