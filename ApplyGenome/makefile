# get the system and host
SYSTEM := $(shell uname)
HOST := $(shell hostname)

# OPT_FLAGS ?= -O3 -ffast-math
OPT_FLAGS ?= -O0 -g
CXXFLAGS += $(OPT_FLAGS)

LIBS = -lm
INC_DIRS = -I../GaitSym2016/src -I../GaitSym2016/exprtk

ifeq ($(SYSTEM),Darwin)
CXXFLAGS += -Wall -fexceptions 
LDFLAGS = -Xlinker -bind_at_load
endif

ifeq ($(SYSTEM),CYGWIN_NT-10.0)
# GNU compiler flags 
CXXFLAGS = -Wall -fexceptions -Wa,-mbig-obj -O0
LDFLAGS = -Wl,--no-keep-memory -Wl,--reduce-memory-overheads
# CXX = clang++
#CC=clang
endif

APPLYSRC = apply_genome.cpp

GAITSYMSRC = XMLConverter.cpp DataFile.cpp

APPLYOBJ = $(addsuffix .o, $(basename $(APPLYSRC) ) )
GAITSYMOBJ = $(addsuffix .o, $(basename $(GAITSYMSRC) ) )

BINARIES = bin/apply_genome

all: directories binaries 

directories: bin obj 

binaries: $(BINARIES)

bin: 
	-mkdir bin

obj: 
	-mkdir obj
	-mkdir obj/apply
	-mkdir obj/gaitsym

obj/apply/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

obj/gaitsym/%.o : ../GaitSym2016/src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

bin/apply_genome: $(addprefix obj/gaitsym/, $(GAITSYMOBJ) ) $(addprefix obj/apply/, $(APPLYOBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)


clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	rm -f .DS_Store

debug:
	@echo $(SYSTEM)
	@echo $(ARCH)
	@echo $(HOST)
	@echo $(INC_DIRS)
	@echo $(LIBS)


