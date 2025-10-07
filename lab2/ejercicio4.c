#include <mpi.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int local_val, global_sum;
    int partner, step;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    local_val = my_rank + 1; 
    global_sum = local_val;

    int steps = (int) log2(comm_sz);

    for (step = 0; step < steps; step++) {
        partner = my_rank ^ (1 << step);  
        int recv_val;

        MPI_Sendrecv(&global_sum, 1, MPI_INT, partner, 0,
                     &recv_val, 1, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        global_sum += recv_val;
    }

    printf("Proceso %d: suma global = %d\n", my_rank, global_sum);

    MPI_Finalize();
    return 0;
}
