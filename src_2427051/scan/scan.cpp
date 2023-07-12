#include <bits/stdc++.h>
#include <omp.h>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

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

  int list_max_power = 27;
  if (argc == 2) { 
    list_max_power = (int) atoi(argv[1]);
  }

  times = fopen("scan_serial_times.txt", "w");

  for (int k = 3; k <= list_max_power; k++)
{
    
    long N = static_cast<int>(pow(2, k));

    double start_time, elapsed_time_s;

    // int N = 131072;
    int *in = new int[N];
    int *out = new int[N];

    //int in[] = {3, 1, 7, 0, 4, 1, 6, 3};

    for (int i = 0; i < N; i++)
    {
        in[i] = rand() % 1000;
        out[i] = 0;
    }

    start_time = omp_get_wtime();
    scanSerial(out, in, N);
    elapsed_time_s = (omp_get_wtime() - start_time);

    
     fprintf(times, "%ld %lf\n", N, elapsed_time_s);

    if (k == list_max_power){
       printf("Times added to file successfully\n");
       fclose(times);
    }

    free(in);
    free(out);

}

    return 0;
}