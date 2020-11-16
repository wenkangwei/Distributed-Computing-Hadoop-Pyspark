#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"


int partial_count(int samples){

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


return count; 

}


int main (int argc, char *argv[]){
  int i ,count;
  int samples, tot_samples;
  double pi;
  int count_sum;
  double start_time, elapsed_time, end_time;
  int rank, numtasks;
  
  //printf("argc: %d\n", argc);
  if (argc <2 ){
  printf("Warning: Need to input sample size\n");
  return 0;
  }
  // Initialize MPI environment and obtain number of tasks, rank of process
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  start_time = MPI_Wtime();
  //Compute number of samples in each process
  tot_samples = atoi(argv[1]);
  samples = tot_samples /numtasks;

  if (rank==0){
	  samples = samples + tot_samples%numtasks; 
  }
  // Compute partial count for each process and then reduce to final count
  count = partial_count(samples);
  MPI_Reduce(&count, &count_sum, 1, MPI_INT, MPI_SUM, 0,MPI_COMM_WORLD);

  if (rank == 0){
  //Estimate pi and print result in process0
  pi = 4.0* (double)count_sum/(double) tot_samples;
  end_time = MPI_Wtime();
  elapsed_time  = end_time- start_time;
  //print estimated pi on process0
  printf("Process:%d, number of tasks:%d, Estimate of pi: %7.5f\n",rank, numtasks,pi);
  //print elapsed time on process0
  printf("Elapsed Time: %lf s\n", elapsed_time);
  }
  else{
  // Work for processes except process0 
  // elapsed time of processes, except process0
  elapsed_time  = end_time- start_time;
  //since the assignment doesn't require to print elapsed time on other processes, so do nothing here.
  }
  


  MPI_Finalize();
return 0; 
}
