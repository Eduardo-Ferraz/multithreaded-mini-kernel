#ifndef _PCB_H
#define _PCB_H

#include "EstadoProcesso.h"
#include <pthread.h>

typedef struct pcb PCB;

PCB *criaProcesso(int pid, int duracao, int prioridade, int nThreads, int startTime);
void destroiProcesso(PCB *pcb);
void setEstadoProcesso(PCB *pcb, EstadoProcesso state);
int consomeTempoProcesso(PCB *pcb, int milisegundos);
void setThreadId(PCB *pcb, int indice, pthread_t threadId);
pthread_t getThreadId(PCB *pcb, int indice);
int getPid(const PCB *pcb);
int getPrioridade(const PCB *pcb);
int getTempoRestante(PCB *pcb);
EstadoProcesso getEstadoProcesso(PCB *pcb);
int getStartTime(const PCB *pcb);
int getNumeroThreads(const PCB *pcb);
pthread_t *getThreadIds(PCB *pcb);


#endif