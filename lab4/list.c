#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_SEGMENTS 10
#define SEGMENT_SIZE 1000
#define N_THREADS 8
#define N_KEYS 1000
#define N_OPS 10000

struct list_node_s {
    int data;
    struct list_node_s* next;
};

pthread_mutex_t segment_mutexes[NUM_SEGMENTS];
struct list_node_s* head = NULL;


int get_segment(int value) {
    return value / SEGMENT_SIZE;
}

int Insert(int value, struct list_node_s** head_p) {
    int segment = get_segment(value);
    pthread_mutex_lock(&segment_mutexes[segment]);

    struct list_node_s* curr_p = *head_p;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;
        if (pred_p == NULL)
            *head_p = temp_p;
        else
            pred_p->next = temp_p;

        pthread_mutex_unlock(&segment_mutexes[segment]);
        return 1;
    } else {
        pthread_mutex_unlock(&segment_mutexes[segment]);
        return 0;
    }
}

int Member(int value, struct list_node_s* head_p) {
    int segment = get_segment(value);
    pthread_mutex_lock(&segment_mutexes[segment]);

    struct list_node_s* curr_p = head_p;
    while (curr_p != NULL && curr_p->data < value)
        curr_p = curr_p->next;

    int result = (curr_p != NULL && curr_p->data == value);

    pthread_mutex_unlock(&segment_mutexes[segment]);
    return result;
}

int Delete(int value, struct list_node_s** head_p) {
    int segment = get_segment(value);
    pthread_mutex_lock(&segment_mutexes[segment]);

    struct list_node_s* curr_p = *head_p;
    struct list_node_s* pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL)
            *head_p = curr_p->next;
        else
            pred_p->next = curr_p->next;
        free(curr_p);

        pthread_mutex_unlock(&segment_mutexes[segment]);
        return 1;
    } else {
        pthread_mutex_unlock(&segment_mutexes[segment]);
        return 0;
    }
}


void* thread_func(void* arg) {
    int tid = *(int*)arg;
    unsigned int seed = time(NULL) ^ tid;

    for (int i = 0; i < N_OPS; i++) {
        int value = rand_r(&seed) % (NUM_SEGMENTS * SEGMENT_SIZE);
        double prob = (rand_r(&seed) % 100) / 100.0;

        if (prob < 0.90) { 
            Member(value, head);
        } else if (prob < 0.95) { 
            Insert(value, &head);
        } else {                 
            Delete(value, &head);
        }
    }
    return NULL;
}


int main() {
    srand(time(NULL));

    for (int i = 0; i < NUM_SEGMENTS; i++)
        pthread_mutex_init(&segment_mutexes[i], NULL);

    for (int i = 0; i < N_KEYS; i++) {
        int value = rand() % (NUM_SEGMENTS * SEGMENT_SIZE);
        Insert(value, &head);
    }

    printf("Lista inicializada con %d elementos.\n", N_KEYS);

    pthread_t threads[N_THREADS];
    int ids[N_THREADS];
    clock_t start = clock();

    for (int i = 0; i < N_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < N_THREADS; i++)
        pthread_join(threads[i], NULL);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Operaciones terminadas en %.4f segundos usando %d hilos.\n",
           elapsed, N_THREADS);

    struct list_node_s* curr_p = head;
    while (curr_p != NULL) {
        struct list_node_s* temp = curr_p;
        curr_p = curr_p->next;
        free(temp);
    }

    for (int i = 0; i < NUM_SEGMENTS; i++)
        pthread_mutex_destroy(&segment_mutexes[i]);

    return 0;
}

