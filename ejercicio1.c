#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int Find_bin(float value, float min_meas, float max_meas, int bin_count) {
    float bin_width = (max_meas - min_meas) / bin_count;
    int bin = (int)((value - min_meas) / bin_width);
    if (bin == bin_count) bin--;
    return bin;
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    float data[] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3,
                    4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};
    int data_count = 20;
    float min_meas = 0.0, max_meas = 5.0;
    int bin_count = 5;

    int local_n = data_count / size;
    float* local_data = (float*)malloc(local_n * sizeof(float));

    MPI_Scatter(data, local_n, MPI_FLOAT,
                local_data, local_n, MPI_FLOAT,
                0, MPI_COMM_WORLD);

    int* loc_bin_counts = (int*)calloc(bin_count, sizeof(int));
    for (int i = 0; i < local_n; i++) {
        int bin = Find_bin(local_data[i], min_meas, max_meas, bin_count);
        loc_bin_counts[bin]++;
    }

    int* bin_counts = NULL;
    if (rank == 0) {
        bin_counts = (int*)calloc(bin_count, sizeof(int));
    }

    MPI_Reduce(loc_bin_counts, bin_counts, bin_count, MPI_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Histograma global:\n");
        for (int b = 0; b < bin_count; b++) {
            printf("Bin %d: %d\n", b, bin_counts[b]);
        }
    }

    free(local_data);
    free(loc_bin_counts);
    if (rank == 0) free(bin_counts);

    MPI_Finalize();
    return 0;
}

