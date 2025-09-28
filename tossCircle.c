#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char* argv[]) {
    int rank, size;
    long long int tosses, local_tosses;
    long long int local_in_circle = 0;
    long long int total_in_circle = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Ingrese el numero total de lanzamientos: ");
        fflush(stdout);
        scanf("%lld", &tosses);
    }

    MPI_Bcast(&tosses, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    local_tosses = tosses / size;

    unsigned int seed = (unsigned int)(time(NULL) + rank);

    for (long long int i = 0; i < local_tosses; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / RAND_MAX * 2.0 - 1.0;
        double distance_squared = x*x + y*y;
        if (distance_squared <= 1.0) local_in_circle++;
    }

    MPI_Reduce(&local_in_circle, &total_in_circle, 1, MPI_LONG_LONG_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi_estimate = 4.0 * (double)total_in_circle / ((double)tosses);
        printf("Estimacion de pi = %.10f\n", pi_estimate);
    }

    MPI_Finalize();
    return 0;
}

