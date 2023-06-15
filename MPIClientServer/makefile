CC = cc
CCFLAGS = -O3 -static

all: MPIClientServer

MPIClientServer: MPIClientServer.o
	$(CC) $(CCFLAGS) $(LDLIBS) -o $@ $^
        
%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDE) -c -o $@ $^ 

clean:
	rm -f *~ *.bak *.bck *.tmp *.o MPIClientServer.sh* MPIClientServer

