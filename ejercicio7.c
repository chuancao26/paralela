#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int msg = 42;
    int n_pingpong = 100000;
    clock_t start_clock, end_clock;
    double start_wtime, end_wtime;

    MPI_Barrier(MPI_COMM_WORLD); 

    if(rank==0) start_clock = clock();
    if(rank==0) start_wtime = MPI_Wtime();

    for(int i=0;i<n_pingpong;i++){
        if(rank==0){
            MPI_Send(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if(rank==1){
            MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank==0){
        end_clock = clock();
        end_wtime = MPI_Wtime();
        printf("Tiempo con clock(): %f s\n", (double)(end_clock - start_clock)/CLOCKS_PER_SEC);
        printf("Tiempo con MPI_Wtime(): %f s\n", end_wtime - start_wtime);
    }

    MPI_Finalize();
    return 0;
}
