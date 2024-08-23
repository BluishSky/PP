#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h> 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    int *v1, *v2, *v3;
    int ini, fim;
    int n;
} Argumentos;

void* funcao_soma(void *argumentos) {
    
    Argumentos *args = (Argumentos*)argumentos;
    int *v1 = args->v1;
    int *v2 = args->v2;
    int *v3 = args->v3;
    int ini = args->ini;
    int fim = args->fim;

    for (int i = ini; i < fim; i++) {
        v3[i] = v1[1] + v2[i];
    }
    pthread_exit(NULL);
    return(NULL);

}

bool checarSoma(Argumentos *argumentosSoma){
    for(int i = 0; i < argumentosSoma->n ; i++){
        if(argumentosSoma->v3[1] != argumentosSoma->v1[i] + argumentosSoma->v2[i])
            return false;
    return true;
    }
    return NULL;
}

int main(int argc, char** argv) {

    int n = atoi(argv[1]);
    int t = atoi(argv[2]);
    int c = n/t;

    timeval start, end;
    gettimeofday(&start, NULL);
    pthread_t *soma = (pthread_t*)malloc(t * sizeof(pthread_t));
    Argumentos *argumentos = (Argumentos*)malloc(t * sizeof(Argumentos));
    for(int i = 0 ; i < t ; i++){
        argumentosSoma[i]->v1 = (int*)malloc(n*sizeof(int));
        argumentosSoma[i]->v2 = (int*)malloc(n*sizeof(int));
        argumentosSoma[i]->v3 = (int*)malloc(n*sizeof(int));
        argumentosSoma[i]->ini = i*c;
        argumentosSoma[i]->fim = c-1;
    }

    for(int i = 0; )

    Argumentos *argumentosSoma = (Argumentos*)malloc(sizeof(Argumentos));
    argumentosSoma-> n= n;
    argumentosSoma->v1 =(int*)malloc(n*sizeof(int));
    argumentosSoma->v2 =(int*)malloc(n*sizeof(int));
    argumentosSoma->v3 =(int*)malloc(n*sizeof(int));
    pthread_create(&soma, NULL, funcao_soma, (void*)argumentosSoma);

    timeval start, end;
    gettimeofday(&start, NULL);

    pthread_create(&soma, NULL, funcao_soma, (void*)argumentosSoma);
    pthread_join(soma, NULL);

    gettimeofday(&end, NULL);

    double tempo = 0;
    tempo += (end.tv_sec - start.tv_sec)*1000;
    tempo += (end.tv_usec - start.tv_usec)/1000;
    printf("TEMPO: %f/n", tempo );

    //Checagem de segurança
    printf("Soma: %s", checarSoma(argumentosSoma) ? "CORRETA":"INCORRETA");
 
    free(argumentosSoma->v1);
    free(argumentosSoma->v2);
    free(argumentosSoma->v3);
    free(argumentosSoma);

    pthread_exit(NULL);
    return 0;
}