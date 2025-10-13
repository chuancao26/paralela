#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>

#define THREAD_COUNT 3
#define MAX_LINE 256

char* my_strtok(char* str, const char* delim, char** saveptr) {
    char* start;
    char* end;

    if (str == NULL)
        str = *saveptr;

    while (*str && strchr(delim, *str))
        str++;

    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }

    start = str;

    while (*str && !strchr(delim, *str))
        str++;

    if (*str) {
        *str = '\0';  
        str++;
    }

    *saveptr = str; 
    return start;
}


void* tokenize_thread(void* arg) {
    char* line = (char*) arg;
    char* token;
    char* saveptr; 

    printf("Hilo %lu procesando: \"%s\"\n", pthread_self(), line);

    token = my_strtok(line, " \t\n", &saveptr);
    while (token != NULL) {
        printf("  [%lu] token: %s\n", pthread_self(), token);
        token = my_strtok(NULL, " \t\n", &saveptr);
    }

    return NULL;
}


int main() {
    pthread_t threads[THREAD_COUNT];

    char lines[THREAD_COUNT][MAX_LINE] = {
        "Pease porridge hot.",
        "Pease porridge cold.",
        "Pease porridge in the pot nine days old."
    };

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, tokenize_thread, lines[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

