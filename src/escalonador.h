#ifndef _ESCALONADOR_H
#define _ESCALONADOR_H

#include "FilaProntos.h"

#ifndef NUM_CPUS
#define NUM_CPUS 1
#endif

// processador roda escalonador sobre fila compartilhada
// id numera processadores ( começa no 0)
typedef struct
{
    int id;
    FilaProntos *fila;
} Processador;

// rotinas de thread dos escalonadores, arg e um Processador
void *escalonadorFCFS(void *arg);
void *escalonadorRR(void *arg);
void *escalonadorPrioridade(void *arg);

void *executaThread(void *arg);

#endif
