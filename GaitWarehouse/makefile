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
    # mpicxx --showme
    # g++ -I/usr/local/include -L/usr/local/lib -lmpi_cxx -lmpi -lm
    CXXFLAGS = -g -O0 -DdDOUBLE -DEXPERIMENTAL -DUSE_PCA -DUSE_CBLAS
    #CXXFLAGS = -ffast-math -O3 -DdDOUBLE -DEXPERIMENTAL
    LDFLAGS  = -framework Accelerate
    CXX      = mpicxx
    CC       = mpicc
    LIBS = -L"$(HOME)/Unix/lib" -lode -lANN -lxml2 -lpthread -lm -lz 
    INC_DIRS = -I../GaitSymODE/ode/include -I"$(HOME)/Unix/include" -I/usr/include/libxml2 -I/System/Library/Frameworks/OpenCL.framework/Versions/A/Headers/
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
        CXXFLAGS = -static -O3 -DdDOUBLE -DEXPERIMENTAL 
        #CXXFLAGS = -static -O3 -DdDOUBLE -DEXPERIMENTAL -std=c++11 -DUSE_PCA -DUSE_CBLAS -DUSE_F2C
        LDFLAGS  = -static 
        CXX      = CC
        CC       = cc
        LIBS = -L"$(HOME)/Unix/lib" -lode -lANN -lxml2 -lpthread -lm -lz 
        #LIBS = -L"$(HOME)/Unix/lib" -lode -lANN -lxml2 -lpthread -lm -lz -llapack_LINUX -lf2c
        INC_DIRS = -I"$(HOME)/Unix/include" -I/usr/include/libxml2 
    endif
endif

MPISRC = UnifiedMPI.cpp WarehouseProcessor.cpp

GAITSYMSRC = \
BallJoint.cpp                   ExpressionFunTransform3D.cpp    HingeJoint.cpp                  Simulation.cpp\
Body.cpp                        ExpressionMat.cpp               IrrlichtRoutines.cpp            SphereGeom.cpp\
CappedCylinderGeom.cpp          ExpressionParser.cpp            Joint.cpp                       StepDriver.cpp\
Contact.cpp                     ExpressionRef.cpp               MAMuscle.cpp                    Strap.cpp\
CyclicDriver.cpp                ExpressionVar.cpp               MAMuscleComplete.cpp            StrokeFont.cpp\
CylinderWrapStrap.cpp           ExpressionVec.cpp               MAMuscleExtended.cpp            PositionReporter.cpp\
DampedSpringMuscle.cpp          Face.cpp                        TIFFWrite.cpp\
DataFile.cpp                    FacetedConicSegment.cpp         Marker.cpp                      ThreePointStrap.cpp\
DataTarget.cpp                  FacetedObject.cpp               Muscle.cpp                      TorqueReporter.cpp\
DataTargetQuaternion.cpp        FacetedPolyline.cpp             NPointStrap.cpp                 TrimeshGeom.cpp\
DataTargetScalar.cpp            FacetedSphere.cpp               NamedObject.cpp                 TwoPointStrap.cpp\
DataTargetVector.cpp            FixedJoint.cpp                  ObjectiveMain.cpp               UniversalJoint.cpp\
Driver.cpp                      FloatingHingeJoint.cpp          UGMMuscle.cpp                   SwingClearanceAbortReporter.cpp\
Environment.cpp                 GLUIRoutines.cpp                PlaneGeom.cpp                   Util.cpp\
ErrorHandler.cpp                GLUtils.cpp                     RayGeom.cpp                     XMLConverter.cpp\
ExpressionFolder.cpp            Geom.cpp                        Reporter.cpp                    fec.cpp\
Controller.cpp                  PIDMuscleLength.cpp             Drivable.cpp                    AMotorJoint.cpp\
TwoCylinderWrapStrap.cpp        SliderJoint.cpp                 BoxGeom.cpp                     BoxCarDriver.cpp\
StackedBoxCarDriver.cpp         PIDTargetMatch.cpp              Warehouse.cpp                   PCA.cpp\
FixedDriver.cpp

MERGESRC = \
DataFile.cpp            ExpressionParser.cpp  MergeXML.cpp  XMLContainer.cpp  XMLContainerList.cpp

AGASRC = \
Genome.cpp      Mating.cpp      Population.cpp  Random.cpp      Statistics.cpp  main.cpp  Preferences.cpp DataFile.cpp

MPIOBJ = $(addsuffix .o, $(basename $(MPISRC) ) )
AGAOBJ = $(addsuffix .o, $(basename $(AGASRC) ) )
GAITSYMOBJ = $(addsuffix .o, $(basename $(GAITSYMSRC) ) )
MERGEOBJ = $(addsuffix .o, $(basename $(MERGESRC) ) )

all: directories binaries 

directories: bin obj
 
binaries: bin/GaitWarehouse 

obj: 
	-mkdir obj
	-mkdir obj/gaitsym
	-mkdir obj/merge
	-mkdir obj/mpi
	-mkdir obj/aga

bin: 
	-mkdir bin

obj/gaitsym/%.o : ../GaitSymODE/src/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/merge/%.o : ../MergeXML/src/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/aga/%.o : ../AsynchronousGA/%.cpp
	$(CXX) -DUSE_MPI $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

obj/mpi/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@


bin/GaitWarehouse: $(addprefix obj/gaitsym/, $(GAITSYMOBJ) ) $(addprefix obj/aga/, $(AGAOBJ) ) \
$(addprefix obj/mpi/, $(MPIOBJ) ) $(addprefix obj/merge/, $(MERGEOBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 

install: all
	cp -f bin/GaitWarehouse $(HOME)/work/

debug:
	echo $(SYSTEM)
	echo $(ARCH)
	echo $(HOST)
	echo $(CXXFLAGS)
	echo $(LDFLAGS)  
	echo $(CXX)     
	echo $(CC)     
	echo $(LIBS) 
	echo $(INC_DIRS)


