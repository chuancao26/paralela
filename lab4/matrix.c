#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define CACHE_LINE_SIZE 64
#define MAX_THREADS 1

typedef struct {
    double value;
    char pad[CACHE_LINE_SIZE - sizeof(double)];
} padded_double;

int m, n, thread_count;
double **A;
double *x;
padded_double *y_padded;

void* Pth_mat_vect(void* rank) {
    long my_rank = (long) rank;
    int local_m = m / thread_count;
    int my_first_row = my_rank * local_m;
    int my_last_row = (my_rank + 1) * local_m - 1;

    for (int i = my_first_row; i <= my_last_row; i++) {
        double sum = 0.0;
        for (int j = 0; j < n; j++)
            sum += A[i][j] * x[j];
        y_padded[i].value = sum;
    }

    return NULL;
}

void generate_data() {
    srand(42);
    A = malloc(m * sizeof(double*));
    for (int i = 0; i < m; i++) {
        A[i] = malloc(n * sizeof(double));
        for (int j = 0; j < n; j++)
            A[i][j] = (double)rand() / RAND_MAX;
    }

    x = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++)
        x[i] = (double)rand() / RAND_MAX;

    y_padded = aligned_alloc(CACHE_LINE_SIZE, m * sizeof(padded_double));
}

void free_data() {
    for (int i = 0; i < m; i++)
        free(A[i]);
    free(A);
    free(x);
    free(y_padded);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <m> <n> <num_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    thread_count = atoi(argv[3]);

    generate_data();

    pthread_t* threads = malloc(thread_count * sizeof(pthread_t));

    clock_t start = clock();

    for (long t = 0; t < thread_count; t++)
        pthread_create(&threads[t], NULL, Pth_mat_vect, (void*) t);

    for (int t = 0; t < thread_count; t++)
        pthread_join(threads[t], NULL);

    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tiempo total = %.6f segundos\n", elapsed);

    printf("Ejemplo: y[0]=%.4f  y[m-1]=%.4f\n", y_padded[0].value, y_padded[m-1].value);

    free_data();
    free(threads);
    return 0;
}

