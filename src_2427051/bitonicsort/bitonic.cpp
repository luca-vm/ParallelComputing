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

int main(int argc, char **argv)
{
    
    FILE *times;
  int list_max_power = 22;
  if (argc == 2) { 
    list_max_power = (int) atoi(argv[1]);
  }

  times = fopen("bitonic_serial_times.txt", "w");
 for (int k = 3; k <= list_max_power; k++)
{
    
    long N = static_cast<int>(pow(2, k));
    double start_time, elapsed_time_s;

    int *a = new int[N];

    for (int i = 0; i < N; i++)
    {
        a[i] = rand() % 1000;
    }

    int ascending = 1; // Sort in ascending order
    start_time = omp_get_wtime();
    bitonicSortSerial(a, 0, N, ascending);
    elapsed_time_s = (omp_get_wtime() - start_time);

     fprintf(times, "%ld %lf\n", N, elapsed_time_s);

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
