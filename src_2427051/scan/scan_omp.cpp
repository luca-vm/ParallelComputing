#include <bits/stdc++.h>
#include <omp.h>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

void upsweep(int* sum, int p){

    
    for (int d = 0; d <= static_cast<int>(log2(p)) - 1; d++)
    {
        int powerd = static_cast<int>(pow(2, d));
        int powerdplus1 = static_cast<int>(pow(2, d + 1));

#pragma omp parallel for num_threads(p)
        for (int i = 0; i < p; i += powerdplus1)
        {
        
            sum[i + powerdplus1 - 1] = sum[i + powerd - 1] + sum[i + powerdplus1 - 1];
        }
    }

}

void downsweep(int* sum, int p){

      sum[p - 1] = 0; 

    for (int d = static_cast<int>(log2(p)) - 1; d >= 0; d--)
    {
        int powerd = static_cast<int>(pow(2, d));
        int powerdplus1 = static_cast<int>(pow(2, d + 1));

#pragma omp parallel for num_threads(p)
        for (int i = 0; i < p- 1; i += powerdplus1)
        {
            int t = sum[i + powerd - 1];
            sum[i + powerd - 1] = sum[i + powerdplus1 - 1];
            sum[i + powerdplus1 - 1] = t + sum[i + powerdplus1 - 1];
        }
    }

}

void scanSerial(int* out, int* in, int n, int start)
{
    int i = 0;
    out[start] = in[start];
    for (i = start + 1; i <= n; i++)
    {
        out[i] = in[i] + out[i - 1];
    }
}

void scanParallel(int* arr, int* arr2, int n, int p)
{
    omp_set_num_threads(p);

    //splitting arr to each processor
    int sum[p];
 #pragma omp parallel for num_threads(p)
    for (int i = 0; i < p; i++)
    {
        scanSerial(arr, arr2, ((n/p)*i) +((n/p) -1), (n/p)*i);
        sum[i] = arr[((n/p)*i) +((n/p) -1)];
    }

    upsweep(sum, p);

    downsweep(sum, p);

  
 //adding back to original
#pragma omp parallel num_threads(p)
{
    int thread_id = omp_get_thread_num();
    int thread_sum = sum[thread_id];  

    #pragma omp for
    for (int i = 0; i < n; i++)
    {
        arr[i] += thread_sum; 
    }
}

}


int main(int argc, char **argv)
{
    FILE *times;

  int list_max_power = 27;
  if (argc == 2) { 
    list_max_power = (int) atoi(argv[1]);
  }

  times = fopen("scan_omp_times.txt", "w");

  for (int k = 3; k <= list_max_power; k++)
{
    long N = static_cast<int>(pow(2, k));
    double start_time, elapsed_time_p;

    int *arr = new int[N];
    int *arr2 = new int[N];
    int *out = new int[N];

    for (int i = 0; i < N; i++)
    {
        arr2[i] = rand() % 1000;
        arr[i] = 0;
    }

    scanSerial(out, arr2, N, 0);

    start_time = omp_get_wtime();
    scanParallel(arr,arr2, N, 8);

    elapsed_time_p = (omp_get_wtime() - start_time);

     fprintf(times, "%ld %lf\n", N, elapsed_time_p);

    if (k == list_max_power){
       printf("Times added to file successfully\n");
       fclose(times);
    }

    int correct = 0;
    for (int i = 0; i < N; i++)
    {
        if (arr[i] != out[i]){
            correct = 1;
        }
    }

    if (correct == 1){
        printf("Invalid");

    }

    
    free(arr);
    free(arr2);
    free(out);

}

    return 0;
}