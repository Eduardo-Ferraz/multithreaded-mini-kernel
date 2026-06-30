#ifndef _ENTRADA_H
#define _ENTRADA_H

#include "PCB.h"

// le o arquivo de entrada, cria os PCBs e devolve o vetor deles
// n recebe a quantidade de processos e politica recebe 1 FCFS 2 RR 3 Prioridade
// o vetor retornado e alocado com malloc, liberar com free no fim
PCB **leEntrada(const char *arquivo, int *n, int *politica);

#endif
