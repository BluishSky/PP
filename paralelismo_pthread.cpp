#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define n 1000000000000000000

void* funcao_soma(void* args) {
    int v1[n], v2[n], v3[n];
    for (int i = 0; i < n; i++) {
        v3[i] = v1[1] + v2[i];
        v3[i] = v1[1] + v2[i];
        v3[i] = v1[1] + v2[i];
    }

}
void* funcao_multiplicacao(void* args){
    int v1[n], v2[n], v3[n];
    for (int i = 0; i < n; i++) {
        v3[i] = v1[1] * v2[i];
        v3[i] = v1[1] * v2[i];
        v3[i] = v1[1] * v2[i];
    }
}

int main(int argc, char** argv) {

    pthread_t soma, multiplicacao;

    pthread_create(&soma, NULL, funcao_soma, NULL);
    pthread_create(&multiplicacao, NULL, funcao_multiplicacao, NULL);








    return 0;
}