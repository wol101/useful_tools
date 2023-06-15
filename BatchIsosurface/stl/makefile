# required files for ubuntu
# sudo apt-get install libvtk7-dev libvtk-dicom-dev

BatchIsosurfaceSrc = \
DataFile.cpp \
InputFileParser.cpp \
PipelineObject.cpp \
main.cpp \
vtkASCIIPointCloudReader.cpp \
pystring.cpp

CXXFLAGS = -O3 -std=c++14 -static
LDFLAGS = -static
CXX      = g++
CC       = gcc
LIBS = -L$(HOME)/Unix/lib \
-Wl,--start-group \
-lvtkChartsCore-7.1 \
-lvtkCommonColor-7.1 \
-lvtkCommonComputationalGeometry-7.1 \
-lvtkCommonCore-7.1 \
-lvtkCommonDataModel-7.1 \
-lvtkCommonExecutionModel-7.1 \
-lvtkCommonMath-7.1 \
-lvtkCommonMisc-7.1 \
-lvtkCommonSystem-7.1 \
-lvtkCommonTransforms-7.1 \
-lvtkDICOMParser-7.1 \
-lvtkDomainsChemistry-7.1 \
-lvtkFiltersAMR-7.1 \
-lvtkFiltersCore-7.1 \
-lvtkFiltersExtraction-7.1 \
-lvtkFiltersFlowPaths-7.1 \
-lvtkFiltersGeneral-7.1 \
-lvtkFiltersGeneric-7.1 \
-lvtkFiltersGeometry-7.1 \
-lvtkFiltersHybrid-7.1 \
-lvtkFiltersHyperTree-7.1 \
-lvtkFiltersImaging-7.1 \
-lvtkFiltersModeling-7.1 \
-lvtkFiltersParallel-7.1 \
-lvtkFiltersParallelImaging-7.1 \
-lvtkFiltersPoints-7.1 \
-lvtkFiltersProgrammable-7.1 \
-lvtkFiltersSMP-7.1 \
-lvtkFiltersSelection-7.1 \
-lvtkFiltersSources-7.1 \
-lvtkFiltersStatistics-7.1 \
-lvtkFiltersTexture-7.1 \
-lvtkFiltersVerdict-7.1 \
-lvtkIOAMR-7.1 \
-lvtkIOCore-7.1 \
-lvtkIOEnSight-7.1 \
-lvtkIOExodus-7.1 \
-lvtkIOGeometry-7.1 \
-lvtkIOImage-7.1 \
-lvtkIOImport-7.1 \
-lvtkIOInfovis-7.1 \
-lvtkIOLSDyna-7.1 \
-lvtkIOLegacy-7.1 \
-lvtkIOMINC-7.1 \
-lvtkIOMovie-7.1 \
-lvtkIONetCDF-7.1 \
-lvtkIOPLY-7.1 \
-lvtkIOParallel-7.1 \
-lvtkIOParallelXML-7.1 \
-lvtkIOSQL-7.1 \
-lvtkIOTecplotTable-7.1 \
-lvtkIOVideo-7.1 \
-lvtkIOXML-7.1 \
-lvtkIOXMLParser-7.1 \
-lvtkImagingColor-7.1 \
-lvtkImagingCore-7.1 \
-lvtkImagingFourier-7.1 \
-lvtkImagingGeneral-7.1 \
-lvtkImagingHybrid-7.1 \
-lvtkImagingMath-7.1 \
-lvtkImagingMorphological-7.1 \
-lvtkImagingSources-7.1 \
-lvtkImagingStatistics-7.1 \
-lvtkImagingStencil-7.1 \
-lvtkInfovisCore-7.1 \
-lvtkInfovisLayout-7.1 \
-lvtkNetCDF-7.1 \
-lvtkNetCDF_cxx-7.1 \
-lvtkParallelCore-7.1 \
-lvtkRenderingContext2D-7.1 \
-lvtkRenderingCore-7.1 \
-lvtkRenderingFreeType-7.1 \
-lvtkalglib-7.1 \
-lvtkexoIIc-7.1 \
-lvtkexpat-7.1 \
-lvtkfreetype-7.1 \
-lvtkhdf5-7.1 \
-lvtkhdf5_hl-7.1 \
-lvtkjpeg-7.1 \
-lvtkjsoncpp-7.1 \
-lvtklibxml2-7.1 \
-lvtkmetaio-7.1 \
-lvtkoggtheora-7.1 \
-lvtkpng-7.1 \
-lvtksqlite-7.1 \
-lvtksys-7.1 \
-lvtktiff-7.1 \
-lvtkverdict-7.1 \
-lvtkzlib-7.1 \
-Wl,--end-group \
-lpthread \
-lm \
-ltiff \
-lz \
-ldl
INC_DIRS = -I$(HOME)/Unix/include/vtk-7.1

BatchIsosurfaceObj = $(addsuffix .o, $(basename $(BatchIsosurfaceSrc) ) )
BatchIsosurfaceHeader = $(addsuffix .h, $(basename $(BatchIsosurfaceSrc) ) )

BINARIES = bin/batch_isosurface

all: directories binaries

directories: bin obj

binaries: $(BINARIES)

obj:
	-mkdir obj

bin:
	-mkdir bin

obj/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

bin/batch_isosurface: $(addprefix obj/, $(BatchIsosurfaceObj) )
	$(CXX) -DUSE_CL $(LDFLAGS) -o $@ $^ $(LIBS)


clean:
	rm -rf obj bin
