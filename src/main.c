#include "entrada.h"
#include "escalonador.h"
#include "FilaProntos.h"
#include "PCB.h"
#include "TCB.h"
#include "tempo.h"
#include "log.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// cria as threads de cada processo no seu tempo de chegada e o enfileira
static void geradorChegada(PCB **lista, int n, FilaProntos *fila)
{
    // ordena os processos por tempo de chegada, desempate pela ordem de leitura
    int *ordem = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
    {
        ordem[i] = i;
    }
    for (int i = 1; i < n; i++)
    {
        int chave = ordem[i];
        int j = i - 1;
        while (j >= 0 && getStartTimeProcesso(lista[ordem[j]]) > getStartTimeProcesso(lista[chave]))
        {
            ordem[j + 1] = ordem[j];
            j--;
        }
        ordem[j + 1] = chave;
    }

    for (int k = 0; k < n; k++)
    {
        PCB *pcb = lista[ordem[k]];

        // espera ate o tempo de chegada do processo
        long espera = getStartTimeProcesso(pcb) - agora_ms();
        if (espera > 0)
        {
            dorme_ms(espera);
        }

        // cria as threads do processo e guarda seus ids no PCB
        for (int t = 0; t < getNumeroThreadsProcesso(pcb); t++)
        {
            TCB *tcb = criaTCB(pcb, t);
            pthread_t tid;
            pthread_create(&tid, NULL, executaThread, tcb);
            setThreadIdProcesso(pcb, t, tid);
        }

        enfileira(fila, pcb);
    }

    free(ordem);
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "erro de uso\n");
        return 1;
    }

    inicia_relogio();

    int n, politica;
    PCB **lista = leEntrada(argv[1], &n, &politica);

    FilaProntos *fila = criaFilaProntos(n);

    // escolhe o escalonador conforme a politica lida
    void *(*escalonador)(void *) = escalonadorFCFS;
    if (politica == 2)
    {
        escalonador = escalonadorRR;
    }
    else if (politica == 3)
    {
        escalonador = escalonadorPrioridade;
    }

    // cria um processador por CPU, todos sobre a mesma fila
    Processador cpus[NUM_CPUS];
    pthread_t threadsCPU[NUM_CPUS];
    for (int c = 0; c < NUM_CPUS; c++)
    {
        cpus[c].id = c;
        cpus[c].fila = fila;
        pthread_create(&threadsCPU[c], NULL, escalonador, &cpus[c]);
    }

    // gera as chegadas e enfileira os processos
    geradorChegada(lista, n, fila);

    // todos os processos ja chegaram, encerra a fila para os escalonadores sairem
    encerraFila(fila);
    for (int c = 0; c < NUM_CPUS; c++)
    {
        pthread_join(threadsCPU[c], NULL);
    }

    registraLog("Escalonador terminou execução de todos processos");

    // espera todas as threads de todos os processos terminarem
    for (int i = 0; i < n; i++)
    {
        for (int t = 0; t < getNumeroThreadsProcesso(lista[i]); t++)
        {
            pthread_join(getThreadIdProcesso(lista[i], t), NULL);
        }
    }

    salvaLog("log_execucao_minikernel.txt");

    for (int i = 0; i < n; i++)
    {
        destroiProcesso(lista[i]);
    }

    free(lista);
    destroiFilaProntos(fila);

    return 0;
}
