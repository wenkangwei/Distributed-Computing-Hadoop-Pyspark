#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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


int mesh_cols =20, mesh_rows = 20;

void PrintImage(float grid[][mesh_cols], int rows, int cols);
void PrintGrid(float grid[][mesh_cols], int xsource, int ysource, int rows, int cols);

void CopyNewToOld(float new[][mesh_cols], float old[][mesh_cols],		int rows, int cols);

void CalculateNew(float new[][mesh_cols], float old[][mesh_cols], int source_cnt[2], int source_displ[2], int rows, int cols);

int main(int argc, char **argv){

int rank, size;
MPI_Status status;

//initialize MPI environment
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD,&size);

//define settings
int iter = 100;
// flag indicating print grid or not
char flag_print = 1;
//set temperature
int edge_temp = 20;
int fireplace_temp = 300;
int fireplace_width = 0.4* mesh_cols;
//starting point in grid of fireplace
int fireplace_displ[] = {1, 0.5*(mesh_cols- fireplace_width) };
int fireplace_counts[] = {1, fireplace_width};
//the fireplace is in the partition of process 0 only
if(rank!=0){
	fireplace_counts[0] = 0;
	fireplace_counts[1] = 0;
}

int displ[size];
int workload[size];
int buffer_cnts[size];
// assign work to each process
// Since we don't want to change the edge, we don't assign the left and 
// right edge columns to the process 0 and the last process
int total_workload = mesh_rows - 2;
for(int i =0; i< size;i++){
	workload[i] = total_workload/size;
	displ[i] = mesh_cols*( (total_workload/size )*i+1);
	if (total_workload %size!=0 && i ==size-1 ){
	// if the grid can not be assigned to each processor evenly
	// then add the rest to workload of the last process
	workload[i] += (total_workload % size);
	}
	buffer_cnts[i] = workload[i]* mesh_cols;
}
//new_mesh: a buffer with 2 rows of ghost points to stored new calculated data
float new_mesh[workload[rank]+2][mesh_cols];

//old_mesh: a buffer with 2 rows of ghost points to exchange rows
//The rows of ghost points are the first row and the last row in array.
float old_mesh[workload[rank]+2][mesh_cols];

// Initialize  degree of the grid and the fireplace
//buffer to store final result
float result[mesh_rows][mesh_cols];
for(int c=0; c<mesh_cols; c++){
	for(int r=0; r< mesh_rows;r++){
	result[r][c] =edge_temp;
	}
}

for(int c=0; c< mesh_cols;c++ ){
	//need to initialize the ghost points in the first row
	//and last row of array, so +2 here
	for(int r=0; r < workload[rank]+2;r++){
		//fireplace is in the partition of process 0
		if (r-1>= fireplace_displ[0] && 
			 r-1 < (fireplace_displ[0]+fireplace_counts[0])&&	
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
	

//iteration
for (int i =0;i < iter;i++){
//send , receive rows to/ from ghost points
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
//if it is not process 0, then fireplace is not on this process
//so set fireplace area =0
//compute  new average value;
CalculateNew(new_mesh, old_mesh, fireplace_counts, fireplace_displ,workload[rank]+2, mesh_cols);

//print buffer
if(flag_print){
	printf("Process: %d\n",rank);
	PrintGrid(new_mesh, 0, 0, workload[rank]+2, mesh_cols);
	
	}
}
//synchronize all computing nodes and collect partial results to the result buffer
MPI_Barrier(MPI_COMM_WORLD);

int send_size = workload[rank]*mesh_cols;
float send_buf[workload[rank]][mesh_cols];
//copy the content to send buffer excluding 
for (int r=0; r< workload[rank];r++){
	for(int c=0;c <mesh_cols; c++){
	send_buf[r][c] = new_mesh[r+1][c];
	}
}
MPI_Gatherv(send_buf,send_size,MPI_FLOAT, result,buffer_cnts,displ, MPI_FLOAT, 0, MPI_COMM_WORLD );

if(rank ==0){
//print and save bitmap to pnm file in process 0
printf("------------------------------------------\n");
PrintGrid(result, 0, 0, mesh_rows, mesh_cols);
PrintImage(result, mesh_rows, mesh_cols);
}

MPI_Finalize();

return 0;
}



void CalculateNew(float new[][mesh_cols], float old[][mesh_cols], int source_cnt[2], int source_displ[2], int rows, int cols){
//compute  new average value;
	for(int r=1; r <rows-1;r++){
		for(int c=1; c<cols-1; c++){
		if (	r>= source_displ[0] && 
			r < (source_displ[0]+source_cnt[0])&&	
			c >= source_displ[1] && 
			c <(source_displ[1]+source_cnt[1])){
		new[r][c] = old[r][c];
		}
		else{

		 new[r][c] = 0.25*(old[r-1][c]+ old[r+1][c]+ old[r][c-1]+old[r][c+1]);}
		}
}
}



void CopyNewToOld(float new[][mesh_cols], float old[][mesh_cols],
		int rows, int cols)
{
// copy new computed array to old array
// don't copy the edges, since temperature of each edge is fixed
	for(int r=1; r <rows+1;r++){
		for(int c=1; c<cols+1; c++){
		old[r][c] = new[r][c];
		}
	}
}

void PrintGrid(float grid[][mesh_cols], int xsource, int ysource, int rows, int cols){
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
				color = colors[num_ranges-1];}
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
