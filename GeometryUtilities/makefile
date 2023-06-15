%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: RotateVector QuaternionFromAxis

RotateVector: RotateVector.o
	$(CXX) $(LDFLAGS) -o $@ $^

QuaternionFromAxis: QuaternionFromAxis.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	rm -f RotateVector
	rm -f *~ *.bak *.bck *.tmp *.o 
