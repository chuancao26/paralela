#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int compare_ints(const void* a, const void* b){
    return (*(int*)a - *(int*)b);
}

int* merge(int* a1, int n1, int* a2, int n2){
    int* result = malloc((n1+n2)*sizeof(int));
    int i=0, j=0, k=0;
    while(i<n1 && j<n2){
        if(a1[i] <= a2[j]) result[k++] = a1[i++];
        else result[k++] = a2[j++];
    }
    while(i<n1) result[k++] = a1[i++];
    while(j<n2) result[k++] = a2[j++];
    return result;
}

int main(int argc, char* argv[]){
    int rank, comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int n;
    if(rank==0){
        printf("Ingrese n: "); fflush(stdout);
        scanf("%d",&n);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = n / comm_sz;
    int* local_keys = malloc(local_n * sizeof(int));

    srand(time(NULL) + rank);
    for(int i=0;i<local_n;i++) local_keys[i] = rand() % 1000;

    qsort(local_keys, local_n, sizeof(int), compare_ints);

    int* gather = NULL;
    if(rank==0) gather = malloc(n*sizeof(int));
    MPI_Gather(local_keys, local_n, MPI_INT, gather, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank==0){
        printf("Listas locales iniciales:\n");
        for(int i=0;i<n;i++){
            printf("%d ", gather[i]);
            if((i+1)%local_n==0) printf("\n");
        }
    }

    free(gather);

    int step = 1;
    int* current_keys = local_keys;
    int current_n = local_n;

    while(step < comm_sz){
        if(rank % (2*step) == 0){
            int partner = rank + step;
            if(partner < comm_sz){
                int recv_n;
                MPI_Recv(&recv_n, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int* recv_keys = malloc(recv_n * sizeof(int));
                MPI_Recv(recv_keys, recv_n, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int* merged = merge(current_keys, current_n, recv_keys, recv_n);
                free(current_keys);
                free(recv_keys);
                current_keys = merged;
                current_n += recv_n;
            }
        } else {
            int partner = rank - step;
            MPI_Send(&current_n, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Send(current_keys, current_n, MPI_INT, partner, 0, MPI_COMM_WORLD);
            break; 
        }
        step *= 2;
    }

    if(rank==0){
        printf("Lista global ordenada:\n");
        for(int i=0;i<current_n;i++){
            printf("%d ", current_keys[i]);
        }
        printf("\n");
        free(current_keys);
    } else {
        free(current_keys);
    }

    MPI_Finalize();
    return 0;
}
