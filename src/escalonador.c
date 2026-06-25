#include "escalonador.h"
#include "PCB.h"
#include "tempo.h"
#include "log.h"
#include <limits.h>

#define QUANTUM_MS 500

// FCFS cada processo roda ate o fim antes de passar ao proximo
void *escalonadorFCFS(void *arg)
{
    FilaProntos *fila = (FilaProntos *)arg;

    PCB *pcb;

    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[FCFS] Executando processo PID %d", getPidProcesso(pcb));

        // roda ate o tempo restante zerar, descontando em fatias
        int restante;
        do
        {
            dorme_ms(QUANTUM_MS);
            restante = consomeTempoProcesso(pcb, QUANTUM_MS);
        } while (restante > 0);

        registraLog("[FCFS] Processo PID %d finalizado", getPidProcesso(pcb));
    }

    registraLog("Escalonador terminou execução de todos processos");
    return NULL;
}

// Round Robin cada processo roda um quantum e se sobrar tempo volta pra fila
void *escalonadorRR(void *arg)
{
    FilaProntos *fila = (FilaProntos *)arg;
    PCB *pcb;

    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[RR] Executando processo PID %d com quantum %dms", getPidProcesso(pcb), QUANTUM_MS);

        // passa um quantum e desconta esse tempo do processo
        dorme_ms(QUANTUM_MS);
        int restante = consomeTempoProcesso(pcb, QUANTUM_MS);

        if (restante == 0)
        {
            registraLog("[RR] Processo PID %d finalizado", getPidProcesso(pcb));
        }
        else
        {
            // ainda tem tempo entao preempta e reinsere no fim da fila
            setEstadoProcesso(pcb, READY);
            enfileira(fila, pcb);
        }
    }

    registraLog("Escalonador terminou execução de todos processos");
    return NULL;
}

// menor numero de prioridade presente na fila, INT_MAX se vazia
static int prioridadeMaisAltaNaFila(FilaProntos *fila)
{
    int n = tamanhoFila(fila);
    int melhor = INT_MAX;

    for (int i = 0; i < n; i++)
    {
        PCB *p = espiaProcesso(fila, i);
        if (p != NULL && getPrioridadeProcesso(p) < melhor)
        {
            melhor = getPrioridadeProcesso(p);
        }
    }

    return melhor;
}

// Prioridade preemptiva roda sempre o de maior prioridade e a cada quantum
// se chegou alguem de prioridade estritamente maior preempta e reinsere
void *escalonadorPrioridade(void *arg)
{
    FilaProntos *fila = (FilaProntos *)arg;
    PCB *pcb;

    while ((pcb = desenfileiraPrioridade(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);

        registraLog("[PRIORITY] Executando processo PID %d prioridade %d ", getPidProcesso(pcb), getPrioridadeProcesso(pcb));

        int preemptado = 0;

        while (!preemptado)
        {
            dorme_ms(QUANTUM_MS);
            int restante = consomeTempoProcesso(pcb, QUANTUM_MS);

            if (restante == 0)
            {
                registraLog("[PRIORITY] Processo PID %d finalizado", getPidProcesso(pcb));
                break;
            }

            // chegou alguem de prioridade estritamente maior entao preempta
            if (prioridadeMaisAltaNaFila(fila) < getPrioridadeProcesso(pcb))
            {
                setEstadoProcesso(pcb, READY);
                enfileira(fila, pcb);
                preemptado = 1;
            }
        }
    }

    registraLog("Escalonador terminou execução de todos processos");
    return NULL;
}
