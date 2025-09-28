#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    MPI_Init(&argc, &argv);

    int rank, comm_sz;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int n = 1000000; 
    int local_n = n / comm_sz;

    int *v_block = NULL;
    if(rank==0){
        v_block = malloc(n*sizeof(int));
        for(int i=0;i<n;i++) v_block[i] = i;
    }

    int *local_block = malloc(local_n*sizeof(int));
    MPI_Scatter(v_block, local_n, MPI_INT, local_block, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    int *local_cyclic = malloc(local_n*sizeof(int)); 
    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    for(int i=0;i<local_n;i++){
        int global_idx = rank*local_n + i;
        int target = global_idx % comm_sz;
        int pos = global_idx / comm_sz;
        MPI_Sendrecv(&local_block[i], 1, MPI_INT, target, 0,
                     &local_cyclic[pos], 1, MPI_INT, target, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = MPI_Wtime();
    if(rank==0) printf("Bloque -> Cíclica: %f s\n", t2-t1);

    // --- Cíclica -> Bloque ---
    MPI_Barrier(MPI_COMM_WORLD);
    t1 = MPI_Wtime();

    for(int i=0;i<local_n;i++){
        int global_idx = i*comm_sz + rank;
        int target = global_idx / local_n;
        int pos = global_idx % local_n;
        MPI_Sendrecv(&local_cyclic[i], 1, MPI_INT, target, 0,
                     &local_block[pos], 1, MPI_INT, target, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    if(rank==0) printf("Cíclica -> Bloque: %f s\n", t2-t1);

    free(local_block);
    free(local_cyclic);
    if(rank==0) free(v_block);

    MPI_Finalize();
    return 0;
}
