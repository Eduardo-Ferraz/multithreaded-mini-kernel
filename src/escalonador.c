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

#if NUM_CPUS > 1
    while (1)
    {
        int ehAjuda;
        pcb = desenfileiraOuAjuda(fila, &ehAjuda);
        if (pcb == NULL)
        {
            break; // fila encerrada e vazia
        }

        if (ehAjuda)
        {
            // co-atende um processo multi-thread que esta rodando sozinho
            if (podeAjudarProcesso(pcb))
            {
                iniciaServicoProcesso(pcb);
                registraLog("[FCFS] Executando processo PID %d" TAG_CPU, getPidProcesso(pcb), cpu->id);
                aguardaFimProcesso(pcb);
                terminaServicoProcesso(pcb);
            }
            continue;
        }

        // atende como primario, se esta sozinho e tem varias threads oferece a outra CPU
        iniciaServicoProcesso(pcb);
        if (getNumeroThreadsProcesso(pcb) > 1 && filaVazia(fila))
        {
            ofereceCompartilhavel(fila, pcb);
        }
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[FCFS] Executando processo PID %d" TAG_CPU, getPidProcesso(pcb), cpu->id);
        aguardaFimProcesso(pcb);
        limpaCompartilhavel(fila, pcb);
        registraLog("[FCFS] Processo PID %d finalizado", getPidProcesso(pcb));
        terminaServicoProcesso(pcb);
    }
#else
    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[FCFS] Executando processo PID %d" TAG_CPU, getPidProcesso(pcb), cpu->id);

        // as threads descontam o tempo, aqui so esperamos o processo terminar
        aguardaFimProcesso(pcb);

        registraLog("[FCFS] Processo PID %d finalizado", getPidProcesso(pcb));
    }
#endif

    return NULL;
}

// Round Robin cada processo roda um quantum e se sobrar tempo volta pra fila
void *escalonadorRR(void *arg)
{
    Processador *cpu = (Processador *)arg;
    FilaProntos *fila = cpu->fila;
    PCB *pcb;

    // IF FINISHED ?

    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[RR] Executando processo PID %d com quantum %dms" TAG_CPU, getPidProcesso(pcb), QUANTUM_MS, cpu->id);

        // as threads descontam a fatia e cedem, aqui so esperamos ceder ou terminar
        aguardaCedeuOuFimProcesso(pcb);

        if (getEstadoProcesso(pcb) == FINISHED)
        {
            registraLog("[RR] Processo PID %d finalizado", getPidProcesso(pcb));
        }
        else
        {
            // cedeu com tempo restante entao reinsere no fim da fila
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

    // IF FINISHED ?

    while ((pcb = desenfileiraPrioridade(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[PRIORITY] Executando processo PID %d prioridade %d " TAG_CPU, getPidProcesso(pcb), getPrioridadeProcesso(pcb), cpu->id);

        int preemptado = 0;
        while (!preemptado)
        {
            // espera a thread ceder a fatia ou terminar
            aguardaCedeuOuFimProcesso(pcb);

            if (getEstadoProcesso(pcb) == FINISHED)
            {
                registraLog("[PRIORITY] Processo PID %d finalizado", getPidProcesso(pcb));
                break;
            }

            // cedeu, se chegou alguem de prioridade estritamente maior preempta
            if (prioridadeMaisAltaNaFila(fila) < getPrioridadeProcesso(pcb))
            {
                enfileira(fila, pcb);
                preemptado = 1;
            }
            else
            {
                // continua com mesmo processo e reconcede a proxima fatia
                setEstadoProcesso(pcb, RUNNING);
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

        // Executa fatia e desconta tempo do processo
        dorme_ms(QUANTUM_MS);
        consomeTempoProcesso(pcb, QUANTUM_MS);
    }

    destroiTCB(tcb);

    return NULL;
}
