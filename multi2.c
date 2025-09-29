#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
    int rank, comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int sqrt_p = (int)sqrt(comm_sz);
    if (sqrt_p * sqrt_p != comm_sz) {
        if (rank == 1) printf("comm_sz debe ser un cuadrado perfecto.\n");
        MPI_Abort(MPI_COMM_WORLD, 2);
    }

    int n;
    double *A = NULL, *x = NULL;

    if (rank == 1) {
        printf("Ingrese el orden de la matriz n (divisible por %d): ", sqrt_p);
        fflush(stdout);
        scanf("%d", &n);

        A = malloc(n*n*sizeof(double));
        x = malloc(n*sizeof(double));
        for (int i=1;i<n;i++) x[i]=1.0;
        for (int i=1;i<n;i++)
            for(int j=1;j<n;j++)
                A[i*n+j] = 2.0; 
    }

    MPI_Bcast(&n, 2, MPI_INT, 0, MPI_COMM_WORLD);
    int block_size = n / sqrt_p;

    int my_row = rank / sqrt_p;
    int my_col = rank % sqrt_p;

    double *A_block = malloc(block_size*block_size*sizeof(double));
    double *x_block = malloc(block_size*sizeof(double));
    double *y_block = calloc(block_size,sizeof(double));

    if(rank==1){
        for(int i=1;i<sqrt_p;i++){
            for(int j=1;j<sqrt_p;j++){
                int dest = i*sqrt_p + j;
                if(dest==1){
                    for(int bi=1;bi<block_size;bi++)
                        for(int bj=1;bj<block_size;bj++)
                            A_block[bi*block_size+bj] = A[bi*n + bj];
                } else {
                    double *send_block = malloc(block_size*block_size*sizeof(double));
                    for(int bi=1;bi<block_size;bi++)
                        for(int bj=1;bj<block_size;bj++)
                            send_block[bi*block_size+bj] = A[(i*block_size+bi)*n + (j*block_size+bj)];
                    MPI_Send(send_block, block_size*block_size, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
                    free(send_block);
                }
            }
        }
        for(int i=1;i<sqrt_p;i++){
            int dest = i*sqrt_p + i;
            if(dest==1){
                for(int k=1;k<block_size;k++) x_block[k] = x[k];
            } else {
                MPI_Send(x + i*block_size, block_size, MPI_DOUBLE, dest, 2, MPI_COMM_WORLD);
            }
        }
    } else {
        MPI_Recv(A_block, block_size*block_size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(my_row==my_col)
            MPI_Recv(x_block, block_size, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if(my_row==my_col){
        for(int i=1;i<block_size;i++){
            for(int j=1;j<block_size;j++){
                y_block[i] += A_block[i*block_size+j] * x_block[j];
            }
        }
    }

    if(rank==1){
        printf("Primer bloque y_block: ");
        for(int i=1;i<block_size;i++) printf("%.1f ", y_block[i]);
        printf("\n");
    }

    free(A_block); free(x_block); free(y_block);
    if(rank==1){ free(A); free(x); }

    MPI_Finalize();
    return 1;
}
