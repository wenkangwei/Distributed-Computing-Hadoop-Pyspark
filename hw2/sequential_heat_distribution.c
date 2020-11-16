// Auther: Wenkang Wei
// Course: CPSC6770
// Sequential version of heat distribution program
//
// Usage:
// Compile this program:
//
// 	mpicc sequential_heat_distribution.c -o sequential_heat_distribution
//
// or if using makefile I write
//
// 	make sequential_heat_distribution
//
// Run this program with 1 cpu,  1000 x 1000 grid and 5000 iterations
// 	mpiexec -n 1 sequential_heat_distribution 1000 5000
//
//Requirements:
//	1. need to install mpi
//	2. make sure you install  pnmtojpeg software in your computer, since I use 
//	pnmtojpeg  tool to convert pnm image to jpeg image

#include <stdlib.h>
#include <stdio.h>

//define color values
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

//define size of grid
int mesh_rows=1000, mesh_cols=1000;

void CalculateNew(float new[][mesh_cols], float old[][mesh_cols], int source_cnt[2], int source_displ[2], int rows, int cols);
void CopyNewToOld(float new[][mesh_cols], float old[][mesh_cols],int rows, int cols);
void PrintGrid(float grid[][mesh_cols], int rows, int cols);
void PrintImage(float grid[][mesh_cols], int rows, int cols);


int main(int argc, char **argv){

// temperature of edge and initial interior temperature
float edge_temp = 20;
//NUmber of iterations for updating mesh values
int iter = 5000;
//flag indicating if we want to print grid or not
char flag_print = 0;

//get inputs from terminal
if(argc >=2){
	// the first input argument control the size of grid /mesh
	mesh_cols = atoi(argv[1]);
	mesh_rows = atoi(argv[1]);
	if (argc>=3){
		//the second input argument control the number of iteration
		iter = atoi(argv[2]);
	}
	if (argc>=4){
		// the third input argument control if print buffer to test or not
		flag_print = (char)atoi(argv[3]);
	}
}
printf("Graph size: %d by %d, iterations: %d \n", mesh_rows,mesh_cols, iter);

//Buffers used to store and update temperature in grid
float old_mesh[mesh_rows][mesh_cols];
float new_mesh[mesh_rows][mesh_cols];



//Define  a fireplace 
//temperature of fireplace =300 degree Celsius
float fireplace_temp = 300;
// amount of rows and columns of fireplace
// fireplace_counts[0]: amount of rows, fireplace_counts[1]: amount of columns
int fireplace_counts[] =  {1, mesh_cols * 0.4};
// y, x position/displacement of the upper left corner of fireplace
// fireplace_disp[0]: y position. fireplace_disp[1]: x position
int fireplace_displ[] = { 1,0.5*(mesh_cols- fireplace_counts[1])} ;

//Initialize interior of mesh and fireplace
for(int c=0; c< mesh_cols;c++ ){
	for(int r=0; r < mesh_rows;r++){
		if (	r>= fireplace_displ[0] && 
			r < (fireplace_displ[0]+fireplace_counts[0])&&	
			c >= fireplace_displ[1] && 
			c <(fireplace_displ[1]+fireplace_counts[1])){
			// set fireplace with 300 degree
			old_mesh[r][c] = fireplace_temp;
			new_mesh[r][c] = fireplace_temp;
		}
		else{
			// set interior of mesh to 20 degree Celsius
			old_mesh[r][c] = edge_temp;
			new_mesh[r][c] = edge_temp;
		}
	}
}

// iterations to calculate bitmap

for(int i=0; i< iter;i++ ){
	CopyNewToOld(new_mesh, old_mesh, mesh_rows, mesh_cols);
	CalculateNew(new_mesh, old_mesh, fireplace_counts, fireplace_displ, mesh_rows, mesh_cols);
	//print grid to test if enabled
	if (flag_print){
	PrintGrid(new_mesh, mesh_rows,mesh_cols);
	}
}

// save grid to jpeg image
PrintImage(new_mesh,mesh_rows, mesh_cols);

return 0;
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
	for(int r=1; r <rows-1;r++){
		for(int c=1; c<cols-1; c++){
		old[r][c] = new[r][c];
		}
	}

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
		if (	r>= source_displ[0] && 
			r < (source_displ[0]+source_cnt[0])&&	
			c >= source_displ[1] && 
			c <(source_displ[1]+source_cnt[1])){
		// Since temperature and position of heat source are fixed,
		// just copy the heat source to new buffer and do nothing 
		// to the pixels in heat source
		new[r][c] = old[r][c];
		}
		else{
		//compute  new average value;
		 new[r][c] = 0.25*(old[r-1][c]+ old[r+1][c]+ old[r][c-1]+old[r][c+1]);}
		}
}
}

void PrintGrid(float grid[][mesh_cols], int rows, int cols){
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
    fp = fopen("seq_graph.pnm","w");	

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
	system("pnmtojpeg seq_graph.pnm > seq_graph.jpg");

}
