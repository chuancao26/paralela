#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int local_val, partner;
    int step, sum;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Valor local = simplemente el rank para probar
    local_val = rank + 1; // ejemplo: cada proceso aporta (rank+1)
    sum = local_val;

    // Tree-structured sum (solo funciona si size es potencia de 2)
    for (step = 1; step < size; step *= 2) {
        if (rank % (2*step) == 0) {
            partner = rank + step;
            if (partner < size) {
                int recv_val;
                MPI_Recv(&recv_val, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum += recv_val;
            }
        } else {
            partner = rank - step;
            MPI_Send(&sum, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            break; // una vez que envía, este proceso sale
        }
    }

    if (rank == 0) {
        printf("Suma global = %d\n", sum);
    }

    MPI_Finalize();
    return 0;
}
