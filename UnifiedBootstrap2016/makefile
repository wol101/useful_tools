# get the system and architecture
ifeq ($(OS),Windows_NT)
    SYSTEM := WIN32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64) OR ($(PROCESSOR_ARCHITEW6432),AMD64)
        ARCH := AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        ARCH := IA32
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

ifeq ($(SYSTEM),OSX)
    # run using 'mpirun -np _nproc_ _executable_ _arguments_'
    # mpicxx --showme
    # g++ -I/usr/local/include -L/usr/local/lib -lmpi_cxx -lmpi -lm
    CXXFLAGS = -g -O0 -DdDOUBLE -DEXPERIMENTAL -DUSE_PCA -DUSE_CBLAS -DUSE_MPI_ABORT -std=c++11 -DUSE_OLD_ODE
    #CXXFLAGS = -ffast-math -O3 -DdDOUBLE -DEXPERIMENTAL
    LDFLAGS  = -framework Accelerate
    CXX      = mpicxx
    CC       = mpicc
    LIBS = -L"$(HOME)/Unix/lib" -lode -lm -lANN
    INC_DIRS = -I../GaitSym2016/rapidxml-1.13 -I../GaitSym2016/exprtk  -I../GaitSym2016/src -I"$(HOME)/Unix/include"
endif

ifeq ($(SYSTEM),LINUX)
    ifeq ($(ARCH),PPC)
        #CXXFLAGS = -O3 -DdDOUBLE -DEXPERIMENTAL -qlanglvl=extc1x
        CXXFLAGS = -O3 -DdDOUBLE -DEXPERIMENTAL -DUSE_ESSL # -DUSE_PCA
        LDFLAGS  =
        CXX      = mpicxx
        CC       = mpicc
        LIBS = -L/bgsys/ibm_essl/prod/opt/ibmmath/lib64 -lesslbg -L"$(HOME)/Unix/lib" -lode -lANN -lxml2 -lpthread -lm
        #LIBS = -L/bgsys/ibm_essl/prod/opt/ibmmath/lib64 -lesslbg -L"$(HOME)/Unix/lib" -lode -lANN -lxml2 -lpthread -lm \
        #       -L/bgsys/linux/RHEL6.3_V1R2M0-36/opt/ibmcmp/xlf/bg/14.1/lib64 -lxlopt -lxl -lxlf90 -lxlfmath -lm
        INC_DIRS = -I"$(HOME)/Unix/include" -I"$(HOME)/Unix/include/libxml2" -I/bgsys/ibm_essl/prod/opt/ibmmath/essl/5.1/include
    endif

    ifeq ($(ARCH),AMD64)
        CXXFLAGS = -static -O3 -DdDOUBLE -DEXPERIMENTAL -std=c++11 -DUSE_MPI_ABORT -DUSE_OLD_ODE
        #CXXFLAGS = -static -O3 -DdDOUBLE -DEXPERIMENTAL -std=c++11 -DUSE_PCA -DUSE_CBLAS -DUSE_F2C -DUSE_MPI_ABORT
        LDFLAGS  = -static
        CXX      = CC
        CC       = cc
        LIBS = -L"$(HOME)/Unix/lib" -lode -lm -lANN
        #LIBS = -L"$(HOME)/Unix/lib" -lode -lANN -lxml2 -lpthread -lm -lz -llapack_LINUX -lf2c
        INC_DIRS = -I../GaitSym2016/rapidxml-1.13 -I../GaitSym2016/exprtk  -I../GaitSym2016/src -I"$(HOME)/Unix/include"
    endif
endif

# hostname specific changes
ifeq ($(HOST),LabHPC-64core2)
CXXFLAGS = -O3 -g -ffast-math -DdDOUBLE -DUSE_OLD_ODE -std=c++11
LDFLAGS  =
CXX      = mpic++
CC       = mpicc
LIBS = -L"$(HOME)/Unix/lib" -lode -lm
INC_DIRS = -I"$(HOME)/Unix/include" -I../GaitSym2016/rapidxml-1.13 -I../GaitSym2016/exprtk  -I../GaitSym2016/src
endif

ifneq (,$(findstring polaris.leeds.ac.uk,$(HOST)))
# polaris uses the intel compiler icpc and needs a new version that supports -std=c++11
# module swap intel/12.1.5.339 intel/15.0.3
CXXFLAGS = -O3 -fast -DdDOUBLE -DUSE_OLD_ODE -std=c++11 -no-ipo
LDFLAGS  = -no-ipo
CXX      = mpic++
CC       = mpicc
LIBS = -L"$(HOME)/Unix/lib" -lode -lm
INC_DIRS = -I"$(HOME)/Unix/include" -I../GaitSym2016/rapidxml-1.13 -I../GaitSym2016/exprtk  -I../GaitSym2016/src
endif

ifeq ($(HOST),asl-rocks-cluster.manchester.ac.uk)
# rocks version of g++ is too old to support -std=c++11
CXXFLAGS = -O3 -g -ffast-math -DdDOUBLE -DUSE_OLD_ODE
LDFLAGS  =
CXX      = mpic++
CC       = mpicc
LIBS = -L"$(HOME)/Unix/lib" -lode -lm
INC_DIRS = -I"$(HOME)/Unix/include" -I../GaitSym2016/rapidxml-1.13 -I../GaitSym2016/exprtk  -I../GaitSym2016/src
endif

ifneq (,$(findstring idb1,$(HOST)))
# this is the hartree Xeon Phi system
# it uses the intel versions of the mpi commands which have ii in the middle
# module load intel/15.1.133_mic intel_mpi/5.0.2_mic
#CXXFLAGS = -O3 -fast -DdDOUBLE -DUSE_OLD_ODE -mmic -no-ipo
CXXFLAGS = -O0 -DdDOUBLE -DUSE_OLD_ODE -mmic -no-ipo
LDFLAGS  = -mmic -no-ipo
CXX      = mpiicpc
CC       = mpiicc
LIBS = -L"$(HOME)/Unix/lib" -lode -lm
INC_DIRS = -I"$(HOME)/Unix/include" -I../GaitSym2016/rapidxml-1.13 -I../GaitSym2016/exprtk -I../GaitSym2016/src
endif

ifeq ($(HOST),login)
# this is the arche knl system
CXXFLAGS = -static -O3 -DdDOUBLE -DEXPERIMENTAL -std=c++11 -DUSE_MPI_ABORT -DUSE_OLD_ODE
#CXXFLAGS = -static -O3 -DdDOUBLE -DEXPERIMENTAL -std=c++11 -DUSE_PCA -DUSE_CBLAS -DUSE_F2C -DUSE_MPI_ABORT
LDFLAGS  = -static
CXX      = CC
CC       = cc
LIBS = -L"$(HOME)/knl_unix/lib" -lode -lm -lANN
#LIBS = -L"$(HOME)/knl_unix/lib" -lode -lANN -lxml2 -lpthread -lm -lz -llapack_LINUX -lf2c
INC_DIRS = -I../GaitSym2016/rapidxml-1.13 -I../GaitSym2016/exprtk  -I../GaitSym2016/src -I"$(HOME)/knl_unix/include"
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
XMLConverter.cpp

MERGESRC = \
MergeExpressionParser.cpp MergeXML.cpp XMLContainer.cpp  XMLContainerList.cpp

AGASRC = \
Genome.cpp Mating.cpp Population.cpp Preferences.cpp Random.cpp Statistics.cpp main.cpp

MPIOBJ = $(addsuffix .o, $(basename $(MPISRC) ) )
AGAOBJ = $(addsuffix .o, $(basename $(AGASRC) ) )
GAITSYMOBJ = $(addsuffix .o, $(basename $(GAITSYMSRC) ) )
MERGEOBJ = $(addsuffix .o, $(basename $(MERGESRC) ) )

all: directories binaries

directories: bin obj

binaries: bin/UnifiedBootstrapAsync2016

obj:
	-mkdir obj
	-mkdir obj/gaitsym
	-mkdir obj/merge
	-mkdir obj/mpi
	-mkdir obj/aga

bin:
	-mkdir bin

obj/gaitsym/%.o : ../GaitSym2016/src/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/merge/%.o : ../MergeXML/src/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/aga/%.o : ../AsynchronousGA/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/mpi/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

bin/UnifiedBootstrapAsync2016: $(addprefix obj/gaitsym/, $(GAITSYMOBJ) ) $(addprefix obj/aga/, $(AGAOBJ) ) \
$(addprefix obj/mpi/, $(MPIOBJ) ) $(addprefix obj/merge/, $(MERGEOBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o

install: all
	cp -f bin/UnifiedBootstrapAsync2016 $(HOME)/work/
	cp -f bin/UnifiedBootstrapAsync2016 $(HOME)/Unix/bin/

