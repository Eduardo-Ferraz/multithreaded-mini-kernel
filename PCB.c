#include "PCB.h"
#include "EstadoProcesso.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct pcb
{
    int pid;
    int tempoTotal;
    int tempoRestante;
    int prioridade;
    int nThreads;
    int startTime;

    EstadoProcesso estado;

    pthread_mutex_t mutex;
    pthread_cond_t cv;
    pthread_t *threadIds;
};

PCB *criaProcesso(int pid, int tempoTotal, int prioridade, int nThreads, int startTime)
{
    PCB *p = malloc(sizeof(PCB));
    if (p == NULL)
        return NULL;

    p->pid = pid;
    p->tempoTotal = tempoTotal;
    p->tempoRestante = tempoTotal;
    p->prioridade = prioridade;
    p->nThreads = nThreads;
    p->startTime = startTime;
    p->estado = READY;

    pthread_mutex_init(&p->mutex, NULL);
    pthread_cond_init(&p->cv, NULL);

    p->threadIds = malloc(sizeof(pthread_t) * nThreads);
    if (p->threadIds == NULL)
    {
        pthread_cond_destroy(&p->cv);
        pthread_mutex_destroy(&p->mutex);
        free(p);
        return NULL;
    }

    return p;
}

void destroiProcesso(PCB *pcb)
{
    if(pcb!=NULL) {
        pthread_cond_destroy(&pcb->cv);
        pthread_mutex_destroy(&pcb->mutex);
        free(pcb->threadIds);
        free(pcb);
    }
}

void setEstadoProcesso(PCB *pcb, EstadoProcesso estado)
{
   pthread_mutex_lock(&pcb->mutex);

    pcb->estado = estado;

    if (estado == RUNNING)
        pthread_cond_broadcast(&pcb->cv);

    pthread_mutex_unlock(&pcb->mutex);
}

static void terminaProcesso(PCB *pcb)
{
    pcb->estado = FINISHED;
    pthread_cond_broadcast(&pcb->cv);
}

int consomeTempoProcesso(PCB *pcb, int milissegundos)
{
    pthread_mutex_lock(&pcb->mutex);
    pcb->tempoRestante -= milissegundos;
    if(pcb->tempoRestante <= 0) {
        pcb->tempoRestante = 0;
        terminaProcesso(pcb);
    }
    int tempoRestante = pcb->tempoRestante;
    pthread_mutex_unlock(&pcb->mutex);

    return tempoRestante;
}

void setThreadId(PCB *pcb, int indice, pthread_t threadId);
pthread_t getThreadId(PCB *pcb, int indice);
int getPid(const PCB *pcb);
int getPrioridade(const PCB *pcb);
int getTempoRestante(PCB *pcb);
EstadoProcesso getEstadoProcesso(PCB *pcb);
int getStartTime(const PCB *pcb);
int getNumeroThreads(const PCB *pcb);
pthread_t *getThreadIds(PCB *pcb);