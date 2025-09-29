#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int n, comm_sz, my_rank;
    double *A = NULL, *x = NULL, *y = NULL;
    double *local_A, *local_x, *local_y;
    int local_cols;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    if (my_rank == 0) {
        printf("Ingrese el orden de la matriz (n, divisible por %d): ", comm_sz);
        fflush(stdout);
        scanf("%d", &n);

        A = malloc(n * n * sizeof(double));
        x = malloc(n * sizeof(double));
        y = malloc(n * sizeof(double));

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                A[i*n + j] = 1.0;

        for (int i = 0; i < n; i++)
            x[i] = 1.0; 
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    local_cols = n / comm_sz;

    local_A = malloc(n * local_cols * sizeof(double));
    local_x = malloc(local_cols * sizeof(double));
    local_y = calloc(n, sizeof(double)); 

    if (my_rank == 0) {
        for (int p = 0; p < comm_sz; p++) {
            if (p == 0) {
                for (int j = 0; j < local_cols; j++)
                    for (int i = 0; i < n; i++)
                        local_A[i*local_cols + j] = A[i*n + j];
                for (int j = 0; j < local_cols; j++)
                    local_x[j] = x[j];
            } else {
                double *send_blockA = malloc(n * local_cols * sizeof(double));
                double *send_blockx = malloc(local_cols * sizeof(double));

                for (int j = 0; j < local_cols; j++)
                    for (int i = 0; i < n; i++)
                        send_blockA[i*local_cols + j] = A[i*n + p*local_cols + j];

                for (int j = 0; j < local_cols; j++)
                    send_blockx[j] = x[p*local_cols + j];

                MPI_Send(send_blockA, n*local_cols, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);
                MPI_Send(send_blockx, local_cols, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);

                free(send_blockA);
                free(send_blockx);
            }
        }
    } else {
        MPI_Recv(local_A, n*local_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(local_x, local_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < local_cols; j++) {
            local_y[i] += local_A[i*local_cols + j] * local_x[j];
        }
    }

    if (my_rank == 0) {
        MPI_Reduce(MPI_IN_PLACE, local_y, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    } else {
        MPI_Reduce(local_y, NULL, n, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    if (my_rank == 0) {
        printf("Resultado y = [ ");
        for (int i = 0; i < n; i++)
            printf("%.1f ", local_y[i]);
        printf("]\n");
    }

    free(local_A);
    free(local_x);
    free(local_y);
    if (my_rank == 0) {
        free(A);
        free(x);
        free(y);
    }

    MPI_Finalize();
    return 0;
}
