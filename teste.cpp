#include <cstring>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define n 100000

void* funcao_soma(void *argumentos) {
    int v1[n], v2[n], v3[n];

    for(int i = 0 ; i < n ; i++) {
        v3[i] = v1[i] + v2[i];
        v3[i] = v1[i] + v2[i];
        v3[i] = v1[i] + v2[i];
    }

    pthread_exit(NULL);
    return NULL;
}

void* funcao_multiplicacao(void *argumentos) {
    int v1[n], v2[n], v3[n];

    for(int i = 0 ; i < n ; i++) {
        v3[i] = v1[i] * v2[i];
        v3[i] = v1[i] * v2[i];
        v3[i] = v1[i] * v2[i];
    }

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t soma, multiplicacao;

    timeval start, end;
    gettimeofday(&start, NULL);
    pthread_create(&soma, NULL, funcao_soma, NULL);
    pthread_create(&multiplicacao, NULL, funcao_multiplicacao, NULL);
    pthread_join(multiplicacao, NULL);
    pthread_join(soma, NULL);
    gettimeofday(&end, NULL);

    double tempo = 0;
    tempo += (end.tv_sec - start.tv_sec)*1000;
    tempo += (end.tv_usec - start.tv_usec)/1000;
    printf("TEMPO: %f\n", tempo);

    pthread_exit(NULL);
    return 0;
}