#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREADS 64
#define N 10000000

double sum = 0.0;
int flag = 0;
pthread_t threads[NUM_THREADS];

void* Thread_sum(void* rank) {
    long my_rank = (long) rank;
    double factor;
    long long i;
    long long my_n = N / NUM_THREADS;
    long long my_first_i = my_n * my_rank;
    long long my_last_i  = my_first_i + my_n;

    if (my_first_i % 2 == 0)
        factor = 1.0;
    else
        factor = -1.0;

    for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
        while (flag != my_rank); 
        sum += factor / (2.0 * i + 1.0);
        flag = (flag + 1) % NUM_THREADS;
    }
    return NULL;
}

int main() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (long thread = 0; thread < NUM_THREADS; thread++)
        pthread_create(&threads[thread], NULL, Thread_sum, (void*) thread);

    for (int thread = 0; thread < NUM_THREADS; thread++)
        pthread_join(threads[thread], NULL);

    gettimeofday(&end, NULL);

    double pi = 4.0 * sum;
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;

    printf("PI (Busy-Wait dentro) = %.15f\n", pi);
    printf("Tiempo = %.6f segundos\n", elapsed);
    return 0;
}

