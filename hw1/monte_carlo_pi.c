#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"


double estimate_pi(int samples){

  int i ,count;
  double x, y;
  double pi;
  
  count = 0;
  for(i=0; i< samples; i++){
    x = (double) rand() / RAND_MAX;
    y = (double) rand() / RAND_MAX;
    if (x*x + y*y <=1){
    	count ++;
    }

  }
  pi = 4.0* (double)count/(double) samples;
  printf("Count = %d, Samples = %d, Estimated pi = %7.5f\n", count, samples, pi);


return pi; 

}


int main (int argc, char *argv[]){
  int i ,count;
  double x, y;
  int samples;
  double pi;
  int rank, numtasks;
  printf("argc: %d\n", argc);
  
  if (argc <2){
  printf("Warning: Need to input sample size\n");
  return 0;
  }
  samples = atoi(argv[1]);
  pi = estimate_pi(samples);
  printf("Estimate of pi: %7.5f  with samples %d, remainer %d \n",pi, samples/4,samples%4);

return 0; 
}
