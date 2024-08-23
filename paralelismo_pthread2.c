#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
    int *v1, *v2, *v3;
    int n;

} Argumentos;

void* funcao_soma(void *argumentos) {
    
    Argumentos *args = (Argumentos*)argumentos;
    int *v1 = args->v1;
    int *v2 = args->v2;
    int *v3 = args->v3;
    int n = args->n;

    for (int i = 0; i < n; i++) {
        v3[i] = v1[1] + v2[i];
        v3[i] = v1[1] + v2[i];
        v3[i] = v1[1] + v2[i];
    }
    pthread_exit(NULL);
    return(NULL);

}
void* funcao_multiplicacao(void* argumentos){
    
    Argumentos *args = (Argumentos*)argumentos;
    int *v1 = args->v1;
    int *v2 = args->v2;
    int *v3 = args->v3;
    int n = args->n;

    for (int i = 0; i < n; i++) {
        v3[i] = v1[1] * v2[i];
        v3[i] = v1[1] * v2[i];
        v3[i] = v1[1] * v2[i];
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

bool checarMultiplicacao(Argumentos *argumentosMultiplicacao){
    for(int i = 0; i < argumentosMultiplicacao->n ; i++){
        if(argumentosMultiplicacao->v3[1] != argumentosMultiplicacao->v1[i] + argumentosMultiplicacao->v2[i])
            return false;
    return true;
    }
    return NULL;
}

int main(int argc, char** argv) {

    int n = atoi(argv[1]);

    pthread_t soma;

    Argumentos *argumentosSoma = (Argumentos*)malloc(sizeof(Argumentos));
    argumentosSoma-> n= n;
    argumentosSoma->v1 =(int*)malloc(n*sizeof(int));
    argumentosSoma->v2 =(int*)malloc(n*sizeof(int));
    argumentosSoma->v3 =(int*)malloc(n*sizeof(int));
    pthread_create(&soma, NULL, funcao_soma, (void*)argumentosSoma);


    pthread_t multiplicacao;
    Argumentos *argumentosMultiplicacao = (Argumentos*)malloc(sizeof(Argumentos));
    argumentosMultiplicacao-> n= n;
    argumentosMultiplicacao->v1 =(int*)malloc(n*sizeof(int));
    argumentosMultiplicacao->v2 =(int*)malloc(n*sizeof(int));
    argumentosMultiplicacao->v3 =(int*)malloc(n*sizeof(int));

    pthread_create(&soma, NULL, funcao_soma, (void*)argumentosSoma);
    pthread_create(&multiplicacao, NULL, funcao_multiplicacao, (void*)argumentosMultiplicacao);

    //Checagem de segurança
    printf("Soma: %s", checarSoma(argumentosSoma) ? "CORRETA":"INCORRETA");
    printf("Multiplicacao: %s", checarMultiplicacao(argumentosMultiplicacao) ? "CORRETA":"INCORRETA");

    free(argumentosSoma->v1);
    free(argumentosSoma->v2);
    free(argumentosSoma->v3);
    free(argumentosSoma);

    free(argumentosMultiplicacao->v1);
    free(argumentosMultiplicacao->v2);
    free(argumentosMultiplicacao->v3);
    free(argumentosMultiplicacao);

    pthread_exit(NULL);
    return 0;
}