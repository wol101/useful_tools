# get the system name
SYSTEM = $(shell uname)

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CXX         = CC
CXXFLAGS    = -g -fullwarn -n32 -ptused 
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CXX         = g++
CXXFLAGS    = -Wall -fexceptions -g
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs) - Darwin version
CXX         = g++
CXXFLAGS    = -Wall -Wuninitialized -fexceptions -g -pedantic -O1 \
-Wtraditional -Wpointer-arith -Wconversion -Wstrict-prototypes
endif

# set up a compilation rule for gnu make
.SUFFIXES: .cc
.cc.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<

all: convert_to_xan convert_to_obj join_bones move_bone flipz_bone xan_to_obj

flipz_bone: flipz_bone.o
	$(CXX) -o $@ $^ $(LIBS)

join_bones: join_bones.o
	$(CXX) -o $@ $^ $(LIBS)

move_bone: move_bone.o
	$(CXX) -o $@ $^ $(LIBS)

convert_to_xan: convert_to_xan.o
	$(CXX) -o $@ $^ $(LIBS)

convert_to_obj: convert_to_obj.o
	$(CXX) -o $@ $^ $(LIBS)

xan_to_obj: xan_to_obj.o
	$(CXX) -o $@ $^ $(LIBS)

clean:
	$(RM) convert_to_obj convert_to_xan join_bones move_bone flipz_bone xan_to_obj
	$(RM) *.o *~ *.bak core *.bck
	$(RM) -rf ii_files

