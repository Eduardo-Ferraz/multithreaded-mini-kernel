#include "entrada.h"
#include <stdio.h>
#include <stdlib.h>

PCB **leEntrada(const char *arquivo, int *n, int *politica)
{
    FILE *f = fopen(arquivo, "r");

    if (f == NULL)
    {
        fprintf(stderr, "erro ao abrir o arquivo\n");

        exit(EXIT_FAILURE);
    }

    fscanf(f, "%d", n);

    PCB **lista = malloc(sizeof(PCB *) * (*n));

    for (int i = 0; i < *n; i++)
    {
        int duracao, prioridade, nThreads, chegada;

        fscanf(f, "%d", &duracao);
        fscanf(f, "%d", &prioridade);
        fscanf(f, "%d", &nThreads);
        fscanf(f, "%d", &chegada);

        // o PID e atribuido na leitura, na ordem em que os processos aparecem
        lista[i] = criaProcesso(i + 1, duracao, prioridade, nThreads, chegada);
    }

    fscanf(f, "%d", politica);

    fclose(f);
    return lista;
}
