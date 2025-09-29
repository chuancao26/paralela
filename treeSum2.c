#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int local_val, sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_val = rank + 1;
    sum = local_val;

    int power2 = 1;
    while (power2 * 2 <= size) power2 *= 2;

    if (rank >= power2) {
        int target = rank - power2;
        MPI_Send(&sum, 1, MPI_INT, target, 0, MPI_COMM_WORLD);
    } else if (rank < size - power2) {
        int recv_val;
        MPI_Recv(&recv_val, 1, MPI_INT, rank + power2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sum += recv_val;
    }

    for (int step = 1; step < power2; step *= 2) {
        if (rank % (2*step) == 0 && rank < power2) {
            int partner = rank + step;
            int recv_val;
            if (partner < power2) {
                MPI_Recv(&recv_val, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum += recv_val;
            }
        } else if (rank < power2) {
            int partner = rank - step;
            MPI_Send(&sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            break;
        }
    }

    if (rank == 0) {
        printf("Suma global = %d\n", sum);
    }

    MPI_Finalize();
    return 0;
}
