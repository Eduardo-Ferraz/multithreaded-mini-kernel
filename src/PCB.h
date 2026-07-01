#ifndef _PCB_H
#define _PCB_H

#include "EstadoProcesso.h"
#include <pthread.h>

typedef struct pcb PCB;

PCB *criaProcesso(int pid, int duracao, int prioridade, int nThreads, int startTime);
void destroiProcesso(PCB *pcb);
void setEstadoProcesso(PCB *pcb, EstadoProcesso state);
int consomeTempoProcesso(PCB *pcb, int milisegundos);
EstadoProcesso aguardaExecucaoOuFimProcesso(PCB *pcb);
void aguardaFimProcesso(PCB *pcb);
void aguardaCedeuOuFimProcesso(PCB *pcb);
void iniciaServicoProcesso(PCB *pcb);
void terminaServicoProcesso(PCB *pcb);
int podeAjudarProcesso(PCB *pcb);
void setThreadIdProcesso(PCB *pcb, int indice, pthread_t threadId);
pthread_t getThreadIdProcesso(PCB *pcb, int indice);
int getPidProcesso(PCB *pcb);
int getPrioridadeProcesso(PCB *pcb);
int getTempoRestanteProcesso(PCB *pcb);
EstadoProcesso getEstadoProcesso(PCB *pcb);
int getStartTimeProcesso(PCB *pcb);
int getNumeroThreadsProcesso(PCB *pcb);

#endif