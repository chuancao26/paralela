#include <mpi.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int local_val, global_sum;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    local_val = my_rank + 1; 
    global_sum = local_val;

    int p = 1;
    while (p * 2 <= comm_sz) p *= 2;

    if (my_rank >= p) {
        int dest = my_rank - p;
        MPI_Send(&global_sum, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    } else {
        if (my_rank + p < comm_sz) {
            int recv_val;
            MPI_Recv(&recv_val, 1, MPI_INT, my_rank + p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_sum += recv_val;
        }

        int steps = (int) log2(p);
        for (int step = 0; step < steps; step++) {
            int partner = my_rank ^ (1 << step);
            int recv_val;

            MPI_Sendrecv(&global_sum, 1, MPI_INT, partner, 0,
                         &recv_val, 1, MPI_INT, partner, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            global_sum += recv_val;
        }
    }

    if (my_rank < p)
        printf("Proceso %d: suma global = %d\n", my_rank, global_sum);

    MPI_Finalize();
    return 0;
}
