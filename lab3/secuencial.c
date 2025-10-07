#include <stdio.h>
#include <sys/time.h>

int main() {
    struct timeval start, end;
    double elapsed;
    long long n = 100000000; 
    double factor = 1.0;
    double sum = 0.0;

    gettimeofday(&start, NULL);
    for (long long i = 0; i < n; i++, factor = -factor) {
        sum += factor / (2.0 * i + 1.0);
    }

    gettimeofday(&end, NULL);

    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    double pi_estimate = 4.0 * sum;
    printf("Estimación de PI = %.15f\n", pi_estimate);
    printf("Tiempo de ejecución: %.6f segundos\n", elapsed);
    return 0;
}

