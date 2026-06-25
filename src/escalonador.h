#ifndef _ESCALONADOR_H
#define _ESCALONADOR_H

#include "FilaProntos.h"

// rotinas de thread dos escalonadores, arg e a FilaProntos
void *escalonadorFCFS(void *arg);
void *escalonadorRR(void *arg);

void *escalonadorPrioridade(void *arg);

#endif
