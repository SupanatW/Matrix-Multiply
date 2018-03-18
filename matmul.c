@@ -1,133 +0,0 @@
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[])
{
	int i,j,k;
	double *A;
	double *B;
	double *C;
	int row_A,col_A,row_B,col_B,row_C,col_C,all_A,all_B,all_C;
	int rank,size,buff=0,count=0;
	double EndTime,StartTime;
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
    	FILE *fp_A;
    	FILE *fp_B;
    	fp_A = fopen(argv[1], "r");
    	fp_B = fopen(argv[2], "r");
    	fscanf(fp_A,"%d %d",&row_A, &col_A);
    	fscanf(fp_B,"%d %d",&row_B, &col_B);
    	all_A = row_A * col_A;
    	all_B = row_B * col_B;
    	row_C = row_A;
    	col_C = col_B;
    	all_C = row_C * col_C;
    	A = (double *)calloc(all_A, sizeof(double));
    	B = (double *)calloc(all_B, sizeof(double));
    	C = (double *)calloc(all_C, sizeof(double));
    	for ( i = 0; i < all_A; i++)
    	{
    		fscanf(fp_A,"%lf",&A[i]);
    	}
    	for ( i = 0; i < row_B; i++)
		{
			for ( j = 0; j < col_B; j++)
			{
				fscanf(fp_B,"%lf",&B[i+(row_B*j)]); // write file 1d 
			}
		}	
    	fclose(fp_A);
    	fclose(fp_B);
    	//StartTime = MPI_Wtime();
    }
   
   	    MPI_Bcast(&row_A, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&col_A, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&row_B, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&col_B, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&row_C, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&col_C, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&all_A, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&all_B, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&all_C, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (rank != 0)
    {
    	A = (double *)calloc(all_A, sizeof(double));
    	B = (double *)calloc(all_B, sizeof(double));
    	C = (double *)calloc(all_C, sizeof(double));
    }
    
	MPI_Bcast(&A[i], all_A, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&B[i], all_B, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		for ( i = 0; i < row_A; i++) //2
		{
			if (rank == i % size)
			{
				for ( j = 0; j < col_B; j++) //4
				{
					buff = 0;
						for ( k = 0; k < row_B; k++)
						{
							buff = buff + (A[k+(i*row_B)] * B[(j*row_B)+k]);
						}
					C[(col_B*i)+j] = buff;
					
				}
			}
		}
		for ( i = 0; i < row_C; i++)
		{
			if (rank != 0 && C[i*col_C] > 0 )
			{
				MPI_Send(&C[i*col_C],col_C,MPI_DOUBLE,0,i, MPI_COMM_WORLD);
			}
		}

		if (rank == 0)
		{
			for ( i = 0; i < row_C; i++)
			{
				if (i % size != 0)
				{
					MPI_Recv(&C[i*col_C],col_C,MPI_DOUBLE,i%size,i,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				}else{
					for ( j = 0; j < col_C; j++)
					{	
						C[(i*col_C)+j] = C[(i*col_C)+j];
						//printf("%.2f\n",C[j] );
					}
				}
			}
			FILE *f_out;
			f_out = fopen(argv[3], "w");
			fprintf(f_out, "%d %d\n",row_C,col_C);
			for ( i = 0; i < all_C; i++)
			{
				//printf("%.2f",C[i] );
				fprintf(f_out, "%.2f",C[i]);
				if ((i+1) % (col_C) == 0  )
				{
					//printf("\n");
					fprintf(f_out, "\n" );
				}else{
					//printf(" ");
					fprintf(f_out, " " );
				}
			}
			//EndTime = MPI_Wtime();
			//printf("Timing : %lf\n",EndTime-StartTime );
		}

    MPI_Finalize();
	return 0;
}