P1 = scan
P2 = scan_omp
P3 = scan_mpi

GPP = g++
MPI = mpicc
CFLAGS = -g -Wall
OMPFLAG = -fopenmp
MPIFLAG = -lm

all: $(P1) $(P2) $(P3)

$(P1): $(P1).cpp
	$(GPP) $(CFLAGS) $(OMPFLAG) $(P1).cpp -o $(P1) $(MPIFLAG)

$(P2): $(P2).cpp
	$(GPP) $(CFLAGS) $(OMPFLAG) $(P2).cpp -o $(P2) $(MPIFLAG)

$(P3): $(P3).c
	$(MPI) $(CFLAGS) -o $(P3) $(P3).c $(MPIFLAG)

clean:
	rm -vf $(P1) $(P2) $(P3)
