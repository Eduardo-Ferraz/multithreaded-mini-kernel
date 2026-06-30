#include "escalonador.h"
#include "PCB.h"
#include "TCB.h"
#include "tempo.h"
#include "log.h"

#define QUANTUM_MS 500

// no modo multi cada linha de execucao indica qual processador rodou
#if NUM_CPUS > 1
#define TAG_CPU " // processador %d"
#else
#define TAG_CPU ""
#endif

// FCFS cada processo roda ate o fim antes de passar ao proximo
void *escalonadorFCFS(void *arg)
{
    Processador *cpu = (Processador *)arg;
    FilaProntos *fila = cpu->fila;
    PCB *pcb;

    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[FCFS] Executando processo PID %d" TAG_CPU, getPidProcesso(pcb), cpu->id);

        // roda ate o tempo restante zerar, descontando em fatias
        int restante;
        do
        {
            dorme_ms(QUANTUM_MS);
            restante = consomeTempoProcesso(pcb, QUANTUM_MS);
        } while (restante > 0);

        registraLog("[FCFS] Processo PID %d finalizado", getPidProcesso(pcb));
    }

    return NULL;
}

// Round Robin cada processo roda um quantum e se sobrar tempo volta pra fila
void *escalonadorRR(void *arg)
{
    Processador *cpu = (Processador *)arg;
    FilaProntos *fila = cpu->fila;
    PCB *pcb;

    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[RR] Executando processo PID %d com quantum %dms" TAG_CPU, getPidProcesso(pcb), QUANTUM_MS, cpu->id);

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

    return NULL;
}

// Prioridade preemptiva roda sempre o de maior prioridade e a cada quantum
// se chegou alguem de prioridade estritamente maior preempta e reinsere
void *escalonadorPrioridade(void *arg)
{
    Processador *cpu = (Processador *)arg;
    FilaProntos *fila = cpu->fila;
    PCB *pcb;

    while ((pcb = desenfileiraPrioridade(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[PRIORITY] Executando processo PID %d prioridade %d " TAG_CPU, getPidProcesso(pcb), getPrioridadeProcesso(pcb), cpu->id);

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

    return NULL;
}

void *executaThread(void *arg)
{
    TCB *tcb = (TCB *)arg;

    PCB *pcb = getPCB(tcb);

    while (1)
    {
        EstadoProcesso estado = aguardaExecucaoOuFimProcesso(pcb);
        if (estado == FINISHED)
        {
            break;
        }

        dorme_ms(QUANTUM_MS);
    }

    destroiTCB(tcb);

    return NULL;
}
