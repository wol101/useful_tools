# get the system name
SYSTEM = $(shell uname)

LIBS += -lxml2 
INC_DIRS += -I/usr/include/libxml2

SRC = \
DataFile.cpp	main.cpp

OBJ = $(addsuffix .o, $(basename $(SRC) ) )

BINARIES = bin/ModelConstructor

all: directories binaries 

directories: bin obj 

binaries: $(BINARIES)

obj: 
	-mkdir obj

bin:
	-mkdir bin
	 
obj/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@

bin/ModelConstructor: $(addprefix obj/, $(OBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 

superclean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	rm -rf project_builder/build project_builder/MergeXML.pbproj/*.pbxuser
	find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;

