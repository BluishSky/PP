#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <size>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    // Tarefa 1: soma de vetores
    int *v1, *v2, *v3;
    v1 = (int*)malloc(n * sizeof(int));
    v2 = (int*)malloc(n * sizeof(int));
    v3 = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        v1[i] = i;  // Inicialização para evitar valores não definidos
        v2[i] = i;  // Inicialização para evitar valores não definidos
        v3[i] = v1[i] + v2[i];
    }

    free(v1);
    free(v2);
    free(v3);

    int *v4, *v5, *v6;
    v4 = (int*)malloc(n * sizeof(int));
    v5 = (int*)malloc(n * sizeof(int));
    v6 = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        v4[i] = i;  // Inicialização para evitar valores não definidos
        v5[i] = i;  // Inicialização para evitar valores não definidos
        v6[i] = v4[i] + v5[i];
    }

    free(v4);
    free(v5);
    free(v6);

    return 0;
}
