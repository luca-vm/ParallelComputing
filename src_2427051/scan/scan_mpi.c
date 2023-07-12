#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

void upsweep(int* arr, int* final_sum, int rank, int p, int split)
{
  MPI_Status status;

  int sum = 0.0;
  for (int i = 0; i < split; i++) {
    sum += arr[i];
  }
 

  int still_alive = 1;
  int tree_depth;
  for (tree_depth = 0; tree_depth < (int)log2(p); tree_depth++) {
    
    if (still_alive) {
      int pos = rank / (int)pow(2, tree_depth);

      if (pos % 2 != 0) {
        int r_rank = rank - (int)pow(2, tree_depth);

        MPI_Send(&sum, 1, MPI_INT, r_rank, 0,
                 MPI_COMM_WORLD);
        still_alive = 0;
        
      }

      else {
        int s_sum;
        int s_rank = rank + (int)pow(2, tree_depth);

        MPI_Recv(&s_sum, 1, MPI_INT, s_rank,
                 0, MPI_COMM_WORLD, &status);

        sum = sum + s_sum;
   
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

  *final_sum = sum;
}

void downsweep(int* arr, int initial_sum, int rank, int p, int split)
{
  MPI_Status status;

  int sum;
  int tree_depth;

  if (rank == 0) {
    sum = initial_sum;
  }

  for (tree_depth = (int)log2(p) - 1; tree_depth >= 0; tree_depth--) {

    if (tree_depth == 0 || rank % (int)pow(2, tree_depth) == 0) {
      int pos = rank / (int)pow(2, tree_depth);

      if (pos % 2 != 0) {

      int r_rank = rank - (int)pow(2, tree_depth);

        MPI_Recv(&sum, 1, MPI_INT, r_rank, 0, MPI_COMM_WORLD, &status);

        MPI_Send(&initial_sum, 1, MPI_INT, r_rank, 0, MPI_COMM_WORLD);

        
      }
      else{
        int s_sum;
        int s_rank = rank + (int)pow(2, tree_depth);

        MPI_Send(&sum, 1, MPI_INT, s_rank, 0, MPI_COMM_WORLD);

        MPI_Recv(&s_sum, 1, MPI_INT, s_rank, 0, MPI_COMM_WORLD, &status);

        sum = sum - s_sum;
        
      }

    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

  int next = arr[split-1];
  arr[split-1] = sum;

  for (int j = split - 2; j >= 0; j--) {
    int temp = arr[j];
    arr[j] = arr[j+1] - next;
    next = temp;
  }
}

void scanSerial(int out[], int in[], int n)
{
    int i = 0;
    out[0] = in[0];
    for (i = 1; i < n; i++)
    {
        out[i] = in[i] + out[i - 1];
    }
}

int main(int argc, char **argv)
{
  FILE *times;
  int rank, p;
  long N;
  int list_max_power = 27;
  if (argc == 2) { 
    list_max_power = (int) atoi(argv[1]);
  }


  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if(rank == 0){
        times = fopen("scan_mpi_times.txt", "w");
   }


for (int k = 3; k <= list_max_power; k++)
{

  N = pow(2, k);
    
  int* output = (int*)malloc(sizeof(int) * N);
  

  MPI_Barrier(MPI_COMM_WORLD);

  
  int split = N / p;
  int* arr = (int*)malloc(sizeof(int) * split);
   int* check = (int*)malloc(sizeof(int) * split);

    srand(N);
    for (int i = 0; i < split; i++)
    {
        arr[i] = rand() % 1000;
        
    }



  scanSerial(check, arr, split);

 MPI_Barrier(MPI_COMM_WORLD);

//If you want to print the check array

// MPI_Gather(arr, split, MPI_INT, check, split, MPI_INT, 0, MPI_COMM_WORLD);
//    if (rank == 0) {
//         printf("Input sums:\n");
//         for (int i = 0; i < N; i++) {
//             printf("%d ", check[i]);
//         }
//         printf("\n");
//     }

  int sum;
  double start = MPI_Wtime();

  upsweep(arr,&sum, rank, p, split);
  downsweep(arr, sum, rank, p, split);

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Gather(arr, split, MPI_INT, output, split, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the prefix sums
    // if (rank == 0) {
    //     printf("Prefix sums:\n");
    //     for (int i = 0; i < N; i++) {
    //         printf("%d ", output[i]);
    //     }
    //     printf("\n");
    // }

  double end;

  if (rank == 0) {
    end = MPI_Wtime() - start;
   // printf("Parrallel time: %lf seconds\n", end);
    fprintf(times, "%ld %lf\n", N, end);

    if (k == list_max_power){
       printf("Times added to file successfully\n");
       fclose(times);
    }
    

    int correct = 0;
    for (int i = 0; i < split; i++)
    {
        if (arr[i] != check[i]){
            correct = 1;
        }
    }

    if (correct == 1){
        printf("Invalid");

    }
  }

  free(arr);
  free(check);
  free(output);
  
  }

    MPI_Finalize();

  return 0;
}
