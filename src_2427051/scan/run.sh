echo =======================================
echo               2427051
echo            Parallel Scan
echo =======================================
echo ---------Make----------
make
echo
echo ---------Serial program running----------
echo
./scan
echo
echo ---------OpenMP program running----------
echo
./scan_omp
echo
echo ---------MPI program running---------
echo 
mpirun -np 2 ./scan_mpi
echo
echo ---------Finished----------