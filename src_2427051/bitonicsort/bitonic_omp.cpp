#include <bits/stdc++.h>
#include <omp.h>
#include <iostream>

using namespace std;

void compareAndSwap(int arr[], int i, int j, int direction)
{
    if (direction == (arr[i] > arr[j]))
        swap(arr[i], arr[j]);
}

void bitonicMerge(int arr[], int low, int count, int direction)
{
    if (count > 1)
    {
        int k = count / 2;
        for (int i = low; i < low + k; i++)
            compareAndSwap(arr, i, i + k, direction);
        bitonicMerge(arr, low, k, direction);
        bitonicMerge(arr, low + k, k, direction);
    }
}

void bitonicSortSerial(int arr[], int low, int count, int direction)
{
    if (count > 1)
    {
        int k = count / 2;

        bitonicSortSerial(arr, low, k, 1);
        bitonicSortSerial(arr, low + k, k, 0);

        bitonicMerge(arr, low, count, direction);
    }
}

void bitonicSortParallel(int arr[], int low, int count, int dir)
{
    if (count <= 2048)
    {
        bitonicSortSerial(arr, low, count, dir);
    }
    else
    {
        if (count > 1)
        {
            int k = count / 2;
            omp_set_num_threads(log2(count));
            omp_set_dynamic(1);

            #pragma omp parallel
            {
                #pragma omp sections
                {
                #pragma omp section
                    bitonicSortParallel(arr, low, k, 1); 
                #pragma omp section
                    bitonicSortParallel(arr, low + k, k, 0);
                }
            }
            bitonicMerge(arr, low, count, dir);
          
        }
    }
}

int main(int argc, char **argv)
{

    FILE *times;
  int list_max_power = 22;
  if (argc == 2) { 
    list_max_power = (int) atoi(argv[1]);
  }

  times = fopen("bitonic_omp_times.txt", "w");
 for (int k = 3; k <= list_max_power; k++)
{
  
    long N = static_cast<int>(pow(2, k));
    double start_time, elapsed_time_p;


    int *a = new int[N];

    for (int i = 0; i < N; i++)
    {
        a[i] = rand() % 1000;
    }

    start_time = omp_get_wtime();
    bitonicSortParallel(a, 0, N, 1);
    elapsed_time_p = (omp_get_wtime() - start_time);

     fprintf(times, "%ld %lf\n", N, elapsed_time_p);

        //Checking correctness
    int correct = 0;
    for(int l = 0; l < N - 1; l++){
        if (a[l] > a[l+1]){
            correct = 1;
        }
    }

    if (correct == 1){
        printf("Invalid");
    }


    if (k == list_max_power){
       printf("Times added to file successfully\n");
       fclose(times);
    }

free(a);

}

    return 0;
}
