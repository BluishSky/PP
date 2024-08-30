#include <pthread.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>

pthread_barrier_t barreira;
pthread_barrier_t barreira_fim;
int **A, **B, **C;

void* exemplo(void* argumentos){
    int i = *((int*)argumentos);
    printf("Sou a thread %d, estou antes da barreira");
    pthread_barrier_wait(&barreira);
    printf("Sou a thread %d, estou depois da barreira");
    //A[0][0] = B[0][0] + C[0][0];
    return NULL;
}

int main(int argc, char** argv){
    int n = atoi(argv[1]);
    int t = atoi(argv[2]);

    
    for(int i = 0 ; i < n ; i++){
        A = (int**)calloc(n * sizeof(int*));
        B = (int**)calloc(n * sizeof(int*));
        C = (int**)calloc(n * sizeof(int*));
    }

    pthread_barrier_init(&barreira, NULL, 3);
    pthread_barrier_init(&barreira_fim, NULL, 4);
    pthread_t threads[3];

    for(int i = 0 ; i < 3 ; i++){
        pthread_create(&threads[i], NULL, exemplo, (void*)&i);
    }

    pthread_barrier_wait(&barreira_fim);

    for(int i = 0 ; i < n ; i++){
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    pthread_exit(NULL);
    return 0;
}