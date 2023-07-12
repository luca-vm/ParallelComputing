#include <stdio.h>        
#include <math.h>       
#include <stdlib.h>     
#include <mpi.h>      


int ComparisonFunc(const void * a, const void * b) {
    return ( * (int *)a - * (int *)b );
}

void LowerHalf(int * arr,int split, int rank, int j) {
    int min;
    int currRank = rank ^ (1 << j);

    int s_count = 0;
    int * s_buff = malloc((split + 1) * sizeof(int));
    int * r_buff = malloc((split + 1) * sizeof(int));

    MPI_Send(&arr[split - 1], 1, MPI_INT, currRank, 0, MPI_COMM_WORLD);
    MPI_Recv(&min, 1, MPI_INT, currRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 0; i < split; i++) {
        if (arr[i] > min) {
            s_buff[s_count + 1] = arr[i];
            s_count++;
        } else {
            break;     
        }
    }

    s_buff[0] = s_count;
    
    MPI_Send(s_buff, s_count, MPI_INT, currRank, 0, MPI_COMM_WORLD);
    MPI_Recv(r_buff, split, MPI_INT, currRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 1; i < r_buff[0] + 1; i++) {
        if (arr[split - 1] < r_buff[i]) {
            arr[split - 1] = r_buff[i];
        } else {
            break;    
        }
    }

    qsort(arr, split, sizeof(int), ComparisonFunc);

    free(s_buff);
    free(r_buff);
}


void UpperHalf(int * arr,int split, int rank, int j) {
    int max;
    int currRank = rank ^ (1 << j);

    int r_count, s_count = 0;
    int * s_buff = malloc((split + 1) * sizeof(int));
    int * r_buff = malloc((split + 1) * sizeof(int));

    MPI_Recv(&max, 1, MPI_INT, currRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&arr[0], 1, MPI_INT, currRank, 0, MPI_COMM_WORLD);

    for (int i = 0; i < split; i++) {
        if (arr[i] < max) {
            s_buff[s_count + 1] = arr[i];
            s_count++;
        } else {
            break;     
        }
    }

    r_count = r_buff[0];
    s_buff[0] = s_count;

    MPI_Recv(r_buff, split, MPI_INT, currRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
    MPI_Send(s_buff, s_count, MPI_INT, currRank, 0, MPI_COMM_WORLD);


    for (int i = 1; i < r_count + 1; i++) {
        if (r_buff[i] > arr[0]) {
            arr[0] = r_buff[i];
        } else {
            break;    
        }
    }

    qsort(arr, split, sizeof(int), ComparisonFunc);

    free(s_buff);
    free(r_buff);
}


int main(int argc, char * argv[]) {
    FILE *times;
    int rank;
    int p;

  int list_max_power = 22;
  if (argc == 2) { 
    list_max_power = (int) atoi(argv[1]);
  }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if(rank == 0){
        times = fopen("bitonic_mpi_times.txt", "w");
   }


    for (int k = 3; k <= list_max_power; k++)
{
    int num = pow(2, k);
    int split = num / p;
    int * arr = (int *) malloc(split * sizeof(int));

    srand(num);
    for (int i = 0; i < split; i++)
    {
        arr[i] = rand() % 1000;
        
    }

    MPI_Barrier(MPI_COMM_WORLD);

 
   double start = MPI_Wtime();
    
    
    qsort(arr, split, sizeof(int), ComparisonFunc);

    for (int i = 0; i < (int)(log2(p)); i++) {
        for (int j = i; j >= 0; j--) {
            if (((rank >> (i + 1)) % 2 != 0 && (rank >> j) % 2 != 0) || ((rank >> (i + 1)) % 2 == 0 && (rank >> j) % 2 == 0)) {
                LowerHalf(arr, split,rank, j);
            } else {
                UpperHalf(arr,split, rank, j);
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double end = MPI_Wtime() - start;

     if (rank == 0) {
    end = MPI_Wtime() - start;
    fprintf(times, "%d %lf\n", num, end);

    if (k == list_max_power){
       printf("Times added to file successfully\n");
         fclose(times);
    }

        int correct = 0;
    for(int l = 0; l < split - 1; l++){
        if (arr[l] > arr[l+1]){
            correct = 1;
        }
    }

    if (correct == 1){
        printf("Invalid");
    }

     }

         free(arr);

}
    MPI_Finalize();
    return 0;
}
