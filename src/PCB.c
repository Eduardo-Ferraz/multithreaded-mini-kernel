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

    int servidores;       // quantas CPUs atendem este processo agora
    int descontosNoPasso; // descontos ja feitos no passo atual, um por servidor

    pthread_mutex_t mutex;
    pthread_cond_t cv;
    pthread_t *threadIds;
};

PCB *criaProcesso(int pid, int tempoTotal, int prioridade, int nThreads, int startTime)
{
    PCB *p = malloc(sizeof(PCB));
    if (p == NULL)
    {
        fprintf(stderr, "Erro ao criar processo %d\n", pid);
        exit(EXIT_FAILURE);
    }

    p->pid = pid;
    p->tempoTotal = tempoTotal;
    p->tempoRestante = tempoTotal;
    p->prioridade = prioridade;
    p->nThreads = nThreads;
    p->startTime = startTime;
    p->estado = READY;
    p->servidores = 0;
    p->descontosNoPasso = 0;

    pthread_mutex_init(&p->mutex, NULL);
    pthread_cond_init(&p->cv, NULL);

    p->threadIds = malloc(sizeof(pthread_t) * nThreads);
    if (p->threadIds == NULL)
    {
        fprintf(stderr, "Erro ao criar threadIds do processo %d\n", pid);
        exit(EXIT_FAILURE);
    }

    return p;
}

void destroiProcesso(PCB *pcb)
{
    if (pcb != NULL)
    {
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
    {
        pthread_cond_broadcast(&pcb->cv);
    }

    pthread_mutex_unlock(&pcb->mutex);
}

int consomeTempoProcesso(PCB *pcb, int milissegundos)
{
    pthread_mutex_lock(&pcb->mutex);

    // so desconta enquanto o processo esta RUNNING, evita desconto em dobro
    // o passo so termina quando servidores threads descontaram, uma por CPU que atende
    // com um servidor cede depois de uma fatia, com dois cede depois de duas
    if (pcb->estado == RUNNING)
    {
        pcb->tempoRestante -= milissegundos;
        pcb->descontosNoPasso++;
        if (pcb->tempoRestante <= 0)
        {
            pcb->tempoRestante = 0;
            pcb->estado = FINISHED;
            pcb->descontosNoPasso = 0;
        }
        else if (pcb->descontosNoPasso >= pcb->servidores)
        {
            pcb->estado = READY;
            pcb->descontosNoPasso = 0;
        }
        pthread_cond_broadcast(&pcb->cv);
    }

    int tempoRestante = pcb->tempoRestante;
    pthread_mutex_unlock(&pcb->mutex);

    return tempoRestante;
}

// registra que mais uma CPU passou a atender o processo
void iniciaServicoProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);
    pcb->servidores++;
    pthread_mutex_unlock(&pcb->mutex);
}

// registra que uma CPU parou de atender o processo
void terminaServicoProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);
    pcb->servidores--;
    pthread_mutex_unlock(&pcb->mutex);
}

// diz se o processo ainda tem thread livre para outra CPU co-atender
int podeAjudarProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);
    int pode = pcb->servidores < pcb->nThreads && pcb->estado != FINISHED;
    pthread_mutex_unlock(&pcb->mutex);
    return pode;
}

EstadoProcesso aguardaExecucaoOuFimProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);
    while (pcb->estado == READY)
    {
        pthread_cond_wait(&pcb->cv, &pcb->mutex);
    }
    EstadoProcesso atual = pcb->estado;
    pthread_mutex_unlock(&pcb->mutex);
    return atual;
}

// mantem o processo rodando fatia a fatia ate terminar
// usado no FCFS
// como a thread cede a CPU a cada fatia, aqui reconcedemos ate FINISHED
void aguardaFimProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);

    while (pcb->estado != FINISHED)
    {
        if (pcb->estado == READY)
        {
            pcb->estado = RUNNING;
            pthread_cond_broadcast(&pcb->cv);
        }
        pthread_cond_wait(&pcb->cv, &pcb->mutex);
    }

    pthread_mutex_unlock(&pcb->mutex);
}

// espera o processo ceder a CPU ou terminar, usado no RR e na prioridade
void aguardaCedeuOuFimProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);

    while (pcb->estado == RUNNING)
    {
        pthread_cond_wait(&pcb->cv, &pcb->mutex);
    }

    pthread_mutex_unlock(&pcb->mutex);
}

void setThreadIdProcesso(PCB *pcb, int indice, pthread_t threadId) // não precisa de lock pois é escrito uma vez antes de entrar na fila
{
    pcb->threadIds[indice] = threadId;
}

pthread_t getThreadIdProcesso(PCB *pcb, int indice) // não precisa de lock
{
    return pcb->threadIds[indice];
}
int getPidProcesso(PCB *pcb)
{
    return pcb->pid;
}
int getPrioridadeProcesso(PCB *pcb)
{
    return pcb->prioridade;
}
int getTempoRestanteProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);
    int tempoRestante = pcb->tempoRestante;
    pthread_mutex_unlock(&pcb->mutex);

    return tempoRestante;
}
EstadoProcesso getEstadoProcesso(PCB *pcb)
{
    pthread_mutex_lock(&pcb->mutex);
    EstadoProcesso estado = pcb->estado;
    pthread_mutex_unlock(&pcb->mutex);

    return estado;
}
int getStartTimeProcesso(PCB *pcb)
{
    return pcb->startTime;
}
int getNumeroThreadsProcesso(PCB *pcb)
{
    return pcb->nThreads;
}