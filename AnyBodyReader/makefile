INC_DIRS = -I/usr/include/libxml2
LIBS = -lxml2 -lpthread -lm -lz

ABRSRC = \
AnyDrawSurf.cpp         AnyMat.cpp              AnyRefNode.cpp          AnyVar.cpp              Face.cpp\
AnyFolder.cpp           AnyMuscle.cpp           AnySeg.cpp              AnyVec.cpp              FacetedObject.cpp\
AnyFunTransform3D.cpp   AnyMuscleModel.cpp      AnyString.cpp           DataFile.cpp            main.cpp\
AnyJoint.cpp            AnyRef.cpp              AnySurfCylinder.cpp     ExpressionParser.cpp

ABROBJ = $(addsuffix .o, $(basename $(ABRSRC) ) )

anybodyreader: $(ABROBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

clean:
	rm -f anybodyreader
	rm -f *.obj
	rm -f *~ *.bak *.bck *.tmp *.o 
