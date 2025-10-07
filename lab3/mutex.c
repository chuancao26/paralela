#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h> 

#define NUM_THREADS 64

#define N 1000000LL

double sum = 0.0;
pthread_mutex_t mutex;
pthread_t threads[NUM_THREADS];

void* Thread_sum(void* rank) {
    long my_rank = (long) rank;
    double factor;
    double my_sum = 0.0;
    long long i;
    long long my_n = N / NUM_THREADS;
    long long my_first_i = my_n * my_rank;
    long long my_last_i  = my_first_i + my_n;

    if (my_first_i % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
        my_sum += factor / (2 * i + 1);
    }

    pthread_mutex_lock(&mutex);
    sum += my_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void) {
    struct timeval start, end;
    double elapsed;

    pthread_mutex_init(&mutex, NULL);

    gettimeofday(&start, NULL);

    for (long thread = 0; thread < NUM_THREADS; thread++)
        pthread_create(&threads[thread], NULL, Thread_sum, (void*) thread);

    for (long thread = 0; thread < NUM_THREADS; thread++)
        pthread_join(threads[thread], NULL);

    gettimeofday(&end, NULL);

    pthread_mutex_destroy(&mutex);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    double pi_estimate = 4.0 * sum;
    printf("Estimación de PI = %.15f\n", pi_estimate);
    printf("Tiempo de ejecución: %.6f segundos\n", elapsed);

    return 0;
}

