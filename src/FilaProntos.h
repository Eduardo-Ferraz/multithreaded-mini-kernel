#ifndef _FILA_PRONTOS_H
#define _FILA_PRONTOS_H

#include "PCB.h"

typedef struct filaProntos FilaProntos;

// cria a fila circular usando o numero de processos como capacidade
FilaProntos *criaFilaProntos(int capacidade);
void destroiFilaProntos(FilaProntos *fila);

// insere o processo no fim e acorda quem espera em desenfileiraEspera
void enfileira(FilaProntos *fila, PCB *processo);

// remove e retorna o processo do inicio, NULL se vazia, nao bloqueia
PCB *desenfileira(FilaProntos *fila);

// como desenfileira mas bloqueia ate ter processo ou a fila ser encerrada
// retorna NULL apenas quando a fila esta vazia e ja foi encerrada
PCB *desenfileiraEspera(FilaProntos *fila);

// co-atendimento no multiprocessador
// oferece e retira um processo para outra CPU ajudar a atender
// desenfileiraOuAjuda bloqueia ate haver processo na fila ou oferta ou encerramento
void ofereceCompartilhavel(FilaProntos *fila, PCB *processo);
void limpaCompartilhavel(FilaProntos *fila, PCB *processo);
PCB *desenfileiraOuAjuda(FilaProntos *fila, int *ehAjuda);

// como desenfileiraEspera mas remove o de maior prioridade menor numero
PCB *desenfileiraPrioridade(FilaProntos *fila);

// menor numero de prioridade presente na fila, INT_MAX se vazia
int prioridadeMaisAltaNaFila(FilaProntos *fila);

// remove um processo especifico usado na preempcao, 1 se removeu, 0 se nao achou
int removeProcesso(FilaProntos *fila, PCB *processo);

// le sem remover o processo na posicao logica dada, 0 e o inicio, NULL se invalida
PCB *espiaProcesso(FilaProntos *fila, int posicao);

int filaVazia(FilaProntos *fila);
int tamanhoFila(FilaProntos *fila);

// marca a fila como encerrada e acorda quem espera para o escalonador sair
void encerraFila(FilaProntos *fila);

#endif
