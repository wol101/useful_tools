# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
#OPT_FLAGS ?= -O3 -ffast-math
OPT_FLAGS ?= -g
EXTRA_FLAGS ?= 

ifeq ($(SYSTEM),Linux)
# GNU compiler flags 
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
# extras for Beowulf compilation
ifeq ($(HOSTNAME),ulgbc1.liv.ac.uk)
OPT_FLAGS = -O3 -ffast-math -march=k6
endif
ifeq ($(HOSTNAME),microlith)
#OPT_FLAGS += -march=opteron
OPT_FLAGS = -O3 -ffast-math
endif
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs)
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
# suggested by linker
LDFLAGS = -Xlinker -bind_at_load
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.1)
# GNU compiler flags 
CXXFLAGS = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
endif

LIBS += -L"$(HOME)/Unix/lib" -lxml2 -lpthread -lm
INC_DIRS += -I"$(HOME)/Unix/include" -I"$(HOME)/Unix/include/libxml2" -I/usr/local/include/libxml2 -I/usr/include/libxml2
SOCKET_LIBS += "$(HOME)/Unix/lib/libptypes.a"

SRC = \
DataFile.cpp          GA.cpp      Mating.cpp     Population.cpp  Statistics.cpp\
ExpressionParser.cpp  Genome.cpp  Objective.cpp  Random.cpp      XMLConverter.cpp\
UDP.cpp               fec.cpp     TCP.cpp

OBJ = $(addsuffix .o, $(basename $(SRC) ) )

#BINARIES = ../bin/ga_fs ../bin/ga_socket ../bin/ga_udp
BINARIES = ../bin/ga_fs ../bin/ga_udp ../bin/ga_tcp

all: directories binaries 

directories: ../bin ../obj 

binaries: $(BINARIES)

../obj: 
	-mkdir ../obj
	-mkdir ../obj/fs
	-mkdir ../obj/socket
	-mkdir ../obj/udp
	-mkdir ../obj/tcp

../bin:
	-mkdir ../bin
	 
../obj/fs/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@ -D USE_FS

../bin/ga_fs: $(addprefix ../obj/fs/, $(OBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

../obj/socket/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@ -D USE_SOCKETS

../bin/ga_socket: $(addprefix ../obj/socket/, $(OBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(SOCKET_LIBS) $(LIBS)

../obj/udp/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@ -D USE_UDP

../bin/ga_udp: $(addprefix ../obj/udp/, $(OBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(UDP_LIBS) $(LIBS)

../obj/tcp/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS)  -c $< -o $@ -D USE_TCP

../bin/ga_tcp: $(addprefix ../obj/tcp/, $(OBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(TCP_LIBS) $(LIBS)

clean:
	rm -rf ../obj ../bin
	rm -f *~ *.bak *.bck *.tmp *.o 

superclean:
	rm -rf ../obj ../bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	rm -rf project_builder/build project_builder/GeneralGA.pbproj/*.pbxuser
	find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;

