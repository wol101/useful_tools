SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3

# useful functions
MKDEPEND    = makedepend
MKDIR       = mkdir -p
CP          = cp
RM          = rm -rf
MV          = mv

ifeq ($(SYSTEM),IRIX64)
# IRIX 6.x with new 32-bit libraries
CC         = cc
# need to turn off inline keyword
CFLAGS    = -fullwarn -n32 -Dinline="" $(OPT_FLAGS)
LD          = ld
AR          = ar rv
INSTALL     = bsdinst -c
# dummy value for ranlib since it is not needed on Irix
RANLIB      = echo
endif

ifeq ($(SYSTEM),Linux)
# GNU compiler flags (works with g++ and egcs)
CC         = gcc
PRELINK     =
CFLAGS    = -Wall $(OPT_FLAGS)
LD          = g++ -w
AR          = ar rv
INSTALL     = install -c                       
RANLIB      = ranlib
endif

ifeq ($(SYSTEM),Darwin)
# GNU compiler flags (works with g++ and egcs)
CC         = cc
PRELINK     =
CFLAGS    = -Wall $(OPT_FLAGS)
LD          = c++ -w
AR          = ar rv
INSTALL     = install -c                       
RANLIB      = ranlib
endif

ifeq ($(SYSTEM),CYGWIN_NT-5.0)
# GNU compiler flags (works with g++ and egcs)
CC         = gcc
PRELINK     =
CFLAGS    = -Wall $(OPT_FLAGS)
LD          = c++ -w
AR          = ar rv
INSTALL     = install -c                       
RANLIB      = ranlib
endif


# set up a compilation rule for gnu make
.SUFFIXES: .c
.c.o:
	$(CC) $(CFLAGS) $(INC_DIRS) -c $<


sortlib.a: heapsort.o merge.o    qsort.o
	$(AR) $@ $^
	$(RANLIB) $@

clean:
	rm -f sortlib.a
	rm -f *.o *.bak *.bck *~ core *.tmp *.exe
	rm -rf ii_files
