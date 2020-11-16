// Auther: Wenkang Wei
// Course: CPSC6770
// This parallel version of program uses strip partition along rows
// and hence two rows of ghost points are added to partition buffer
//
// Main steps in program:
// 	1. take inputs from terminal and define settings
// 	2. Initialize MPI environment and use strip partition and assign work for each process
// 	   Note: Since the values in edges of grid are fixed to 20 degrees, I don't assign the first row and the last row to processors
//	3. Initialize buffers, grids for computing partial results and final results
//	4. Loop for communication, heat updating
//	5. Synchronize all processes and gather partial results to process 0
//	6. convert image to jpg format and finalize MPI environment
//
//
// Usage: 
// compile this program:
// 	mpicc MPI_heat_distribution.c -o MPI_heat_distribution
//
// 	or  if using the makefile I write
//
//	make MPI_heat_distribution
//
//Run this program with 16 cpus, 1000 x 1000 grid, 50000 iterations
//
//	mpiexec -n 16 MPI_heat_distribution 1000 50000
//
//Note:  if don't input iterations number in terminal, iteration =5000 as default
//Requirements:
//	1. need to install mpi
//	2. make sure you install  pnmtojpeg software in your computer, since I use 
//	pnmtojpeg  tool to convert pnm image to jpeg image

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// definition of color
#define WHITE    "15 15 15 "
#define RED      "15 00 00 "
#define ORANGE   "15 05 00 "
#define YELLOW   "15 10 00 "
#define LTGREEN  "00 13 00 "
#define GREEN    "05 10 00 "
#define LTBLUE   "00 05 10 "
#define BLUE     "00 00 10 "
#define DARKTEAL "00 05 05 "
#define BROWN    "03 03 00 "
#define BLACK    "00 00 00 "

// define grid size
int mesh_cols =1000, mesh_rows = 1000;

void PrintImage(float grid[][mesh_cols], int rows, int cols);
void PrintGrid(float grid[][mesh_cols], int rows, int cols);

void CopyNewToOld(float new[][mesh_cols], float old[][mesh_cols],		int rows, int cols);

void CalculateNew(float new[][mesh_cols], float old[][mesh_cols], int source_cnt[2], int source_displ[2], int rows, int cols);

int main(int argc, char **argv){

int rank, size;
MPI_Status status;

//initialize MPI environment
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD,&size);

//default iterations = 5000
int iter = 5000;
// flag indicating print grid or not
char flag_print = 0;
// parameter settings
if(argc >=2){
	// the first input argument control the size of grid /mesh
	mesh_cols = atoi(argv[1]);
	mesh_rows = atoi(argv[1]);
	//the second input argument control the number of iteration
	if (argc>=3){
		iter = atoi(argv[2]);
	}
	// the third input argument control if print buffer to test or not
	if(argc>=4){
	flag_print = (char) atoi(argv[3]);
	}
}

if(rank ==0){
printf("Graph size: %d by %d, iterations:%d\n",mesh_rows,mesh_cols,iter);
}


//set mesh temperature
int edge_temp = 20;
// Define fireplace here
// temperature of fireplace = 300 degree
int fireplace_temp = 300;
int fireplace_width = 0.4* mesh_cols;

// y, x position/displacement of the upper left corner of fireplace
// fireplace_disp[0]: y position. fireplace_disp[1]: x position
int fireplace_displ[] = {1, 0.5*(mesh_cols- fireplace_width) };
// amount of rows and columns of fireplace
// fireplace_counts[0]: amount of rows, fireplace_counts[1]: amount of columns
int fireplace_counts[] = {1, fireplace_width};

//Since the fireplace is one row on the top of the grid
//Only the partition assigned to process 0 contains fireplace
//So set the area of fireplace in processes to 0.
if(rank!=0){
	fireplace_counts[0] = 0;
	fireplace_counts[1] = 0;
}




//buffer to store displacement of task of each process in grid
int displ[size];
//buffer to store workload / the amount of rows assigned to each process
int workload[size];
// buffer to store the total count of pixels in partition assigned to each process
int buffer_cnts[size];

// Assign work to each process
// Note: 
// 	Since each edge of grid is fixed, I don't assign the top edge row and 
// 	bottom edge row to the process 0 and the last process.
// 	Then workload = total rows -2
int total_workload = mesh_rows - 2;
for(int i =0; i< size;i++){
	workload[i] = total_workload/size;
	// compute displacement in 2-D array
	displ[i] = mesh_cols*( (total_workload/size )*i+1);
	if (total_workload %size!=0 && i ==size-1 ){
	// if the grid can not be assigned to each processor evenly
	// then add the rest workload of the last process
	workload[i] += (total_workload % size);
	}
	buffer_cnts[i] = workload[i]* mesh_cols;
}
//print information
MPI_Barrier(MPI_COMM_WORLD);
printf("Process: %d, partition size: %d rows by %d columns\n",rank, workload[rank],mesh_cols);



// Using Strip partition along rows
// Note: 
// 	The first row and the last row in buffers are ghost points
//
//buffers with 2 rows of ghost points used to compute and store temperature
float new_mesh[workload[rank]+2][mesh_cols];
float old_mesh[workload[rank]+2][mesh_cols];

// Initialize the buffer used to store the final result
float result[mesh_rows][mesh_cols];
for(int c=0; c<mesh_cols; c++){
	for(int r=0; r< mesh_rows;r++){
	result[r][c] =edge_temp;
	}
}
// Initialize buffers used to compute partial result in each process
for(int c=0; c< mesh_cols;c++ ){
	//need to initialize the ghost points in the first row
	//and last row of array, so +2 here
	for(int r=0; r < workload[rank]+2;r++){
		//fireplace is in the partition of process 0
		if (r>= fireplace_displ[0] && 
			 r < (fireplace_displ[0]+fireplace_counts[0])&&	
			c >= fireplace_displ[1] && 
			c <(fireplace_displ[1]+fireplace_counts[1])){
			// set fireplace with 300 degree
			old_mesh [r][c] = fireplace_temp;
			new_mesh [r][c] = fireplace_temp;
		}
		else{
			// set interior of mesh to 20 degree Celsius
			old_mesh[r][c] = edge_temp;
			new_mesh[r][c] = edge_temp;
		}
	}
}
	



//Iterations to update the grid
for (int i =0;i < iter;i++){
	//Exchange and Update ghost points among processes
	if(rank==0){
		MPI_Send(new_mesh[workload[rank]],mesh_cols, MPI_FLOAT, rank+1,0, MPI_COMM_WORLD );
		MPI_Recv(old_mesh[workload[rank]+1], mesh_cols, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD,&status);
	}
	else if(rank!= size-1){
		MPI_Send(new_mesh[1],mesh_cols, MPI_FLOAT, rank-1,0, MPI_COMM_WORLD );
		MPI_Recv(old_mesh[0], mesh_cols, MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD,&status);
		MPI_Recv(old_mesh[workload[rank]+1], mesh_cols, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD,&status);
		MPI_Send(new_mesh[workload[rank]],mesh_cols, MPI_FLOAT, rank+1,0, MPI_COMM_WORLD );
	}
	else if (rank == size-1){
		MPI_Send(new_mesh[1],mesh_cols, MPI_FLOAT, rank-1,0, MPI_COMM_WORLD );
		MPI_Recv(old_mesh[0], mesh_cols, MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD,&status);

	}

	//copy new buffer to old buffer
	CopyNewToOld(new_mesh, old_mesh, workload[rank]+2, mesh_cols);
	//compute  new average value;
	CalculateNew(new_mesh, old_mesh, fireplace_counts, fireplace_displ,workload[rank]+2, mesh_cols);

	//print buffer if enabled
	if(flag_print){
		printf("Process: %d\n",rank);
		PrintGrid(new_mesh, workload[rank]+2, mesh_cols);	
	}
}



//synchronize all processors and collect partial results to the result buffer
MPI_Barrier(MPI_COMM_WORLD);
int send_size = workload[rank]*mesh_cols;
float send_buf[workload[rank]][mesh_cols];
//copy the content to send_buffer, excluding the rows of ghost points
for (int r=0; r< workload[rank];r++){
	for(int c=0;c <mesh_cols; c++){
	send_buf[r][c] = new_mesh[r+1][c];
	}
}


// send partial results to process 0 to get final result
MPI_Gatherv(send_buf,send_size,MPI_FLOAT, result,buffer_cnts,displ, MPI_FLOAT, 0, MPI_COMM_WORLD );


if(rank ==0){
//print result and save bitmap to jpg file in process 0
PrintImage(result, mesh_rows, mesh_cols);
 if(flag_print){
   printf("------------------------------------------\n");
   PrintGrid(result, mesh_rows, mesh_cols);
 }
}


//Finalize MPI environment
MPI_Finalize();
return 0;
}





void CalculateNew(float new[][mesh_cols], float old[][mesh_cols], int source_cnt[2], int source_displ[2], int rows, int cols){
/*
 * new[][mesh_cols]: buffer storing new temperature data
 * old[][mesh_cols]: buffer storing old temperature data
 * source_cnt:	     buffer storing amount of rows and columns in fireplace
 * source_displ:     buffer storing y,x position of the upper left corner of 
 * 		     fireplace
 * rows: 	     the number of rows of buffers for new, old grids
 * cols:             the number of columns of  buffer for new, old grids
 *
 * Note: buffer new[][]  and buffer old[][] have the same shape
 * */
	for(int r=1; r <rows-1;r++){
		for(int c=1; c<cols-1; c++){
		if (r >= source_displ[0] && 
			r < (source_displ[0]+source_cnt[0])&&	
			c >= source_displ[1] && 
			c <(source_displ[1]+source_cnt[1])){
		// Since temperature and position of heat source are fixed
		// just copy heat source to new buffer
		new[r][c] = old[r][c];
		}
		else{
		// compute average
		 new[r][c] = 0.25*(old[r-1][c]+ old[r+1][c]+ old[r][c-1]+old[r][c+1]);
		}

		}
}
}



void CopyNewToOld(float new[][mesh_cols], float old[][mesh_cols],
		int rows, int cols)
{
/*
 * new[][mesh_cols]: buffer storing new temperature data
 * old[][mesh_cols]: buffer storing old temperature data
 * rows: 	     the number of rows of buffers for new, old grids
 * cols:             the number of columns of  buffer for new, old grids
 *
 * Note: buffer new[][]  and buffer old[][] have the same shape
 * */

	// copy new computed array to old array
	// don't copy the edges, since temperature of each edge is fixed
	for(int r=1; r <rows-1;r++){
		for(int c=1; c<cols-1; c++){
		old[r][c] = new[r][c];
		}
	}
}

void PrintGrid(float grid[][mesh_cols],int rows, int cols){
/*
 * grid[][mesh_cols]:  grid we want to print
 * rows: the amount of rows in grid
 * cols: the amount of columns in grid
 *
 * */
	// print mesh value
	for (int r=0; r<rows; r++){
		for(int c=0; c<cols; c++){
			printf(" %.2f", grid[r][c]);
		}
	printf("\n");
	}
	printf("\n\n\n");
}


void PrintImage(float grid[][mesh_cols], int rows, int cols){
/*
 * grid[][mesh_cols]:  grid we want to convert to jpg file
 * rows: the amount of rows in grid
 * cols: the amount of columns in grid
 *
 * */
	FILE *fp;
    fp = fopen("mpi_graph.pnm","w");	

    fprintf(fp, "P3\n%d %d\n15\n", cols, rows);
	char * colors[10] = { RED, ORANGE, YELLOW, LTGREEN, GREEN, 
                         LTBLUE, BLUE, DARKTEAL, BROWN, BLACK };
	char * color;
	float color_range[] = {250, 180,120, 80,60,50,40,30,20};
	int num_ranges = sizeof(color_range)/sizeof(color_range[0]);
	for (int r=0; r<rows; r++){
		for(int c=0; c<cols; c++){
			// iterate each color range and select the color from color list
			for(int i=0; i< num_ranges+1; i++){
				if (i==0 && grid[r][c]>color_range[0] ){
					color= colors[0];
				}
				else if(i == num_ranges && grid[r][c]<=color_range[num_ranges-1]){
				color = colors[num_ranges];}
				else if(grid[r][c]<= color_range[i-1] && grid[r][c] >color_range[i]){
				color = colors[i];
				}
			}

				
				fprintf(fp, "%s", color);
		}
	fprintf(fp,"\n");
	}
	// close file pointer
	fclose(fp);
	system("pnmtojpeg mpi_graph.pnm > mpi_graph.jpg");

}

