echo =======================================
echo               2427051
echo         Parallel Bitonic Sort
echo =======================================
echo ---------Make----------
make
echo
echo ---------Serial program running----------
echo
./bitonic
echo
echo ---------OpenMP program running----------
echo
./bitonic_omp
echo
echo ---------MPI program running---------
echo 
mpirun -np 2 ./bitonic_mpi
echo
echo ---------Finished----------