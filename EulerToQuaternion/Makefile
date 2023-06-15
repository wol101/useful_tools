CC = cc -g

all: EulerSample test1 test2 EulerToQuaternion QuaternionToEuler EulerToMatrix MatrixToEuler EulerToAxisAngle

test1: test1.o EulerAngles.o
	$(CC) -o $@ $^ -lm

test2: test2.o EulerAngles.o
	$(CC) -o $@ $^ -lm

EulerSample: EulerSample.o EulerAngles.o
	$(CC) -o $@ $^ -lm

EulerToQuaternion: EulerToQuaternion.o EulerAngles.o
	$(CC) -o $@ $^ -lm

EulerToAxisAngle: EulerToAxisAngle.o EulerAngles.o
	$(CC) -o $@ $^ -lm

QuaternionToEuler: QuaternionToEuler.o EulerAngles.o
	$(CC) -o $@ $^ -lm

EulerToMatrix: EulerToMatrix.o EulerAngles.o
	$(CC) -o $@ $^ -lm

MatrixToEuler: MatrixToEuler.o EulerAngles.o
	$(CC) -o $@ $^ -lm

clean:
	rm -f *.o EulerSample test1 test2 EulerToQuaternion QuaternionToEuler EulerToMatrix MatrixToEuler EulerToAxisAngle
	rm -f *.o *~ *.bak core *.bck
	rm -rf ii_files

EulerAngles.o: EulerAngles.h QuatTypes.h
