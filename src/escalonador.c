#include "escalonador.h"
#include "PCB.h"
#include "tempo.h"
#include "log.h"

#define QUANTUM_MS 500

// FCFS: cada processo roda ate o fim antes de passar ao proximo
void *escalonadorFCFS(void *arg)
{
    FilaProntos *fila = (FilaProntos *)arg;
    PCB *pcb;

    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[FCFS] Executando processo PID %d", getPidProcesso(pcb));

        // roda ate o tempo restante zerar, descontado em fatias
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

// Round Robin: cada processo roda um quantum, se sobra tempo, volta pra fila
void *escalonadorRR(void *arg)
{
    FilaProntos *fila = (FilaProntos *)arg;
    PCB *pcb;

    while ((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);
        registraLog("[RR] Executando processo PID %d com quantum %dms",
                    getPidProcesso(pcb), QUANTUM_MS);

        // passa um quantum e desconta tempo do processo
        dorme_ms(QUANTUM_MS);
        int restante = consomeTempoProcesso(pcb, QUANTUM_MS);

        if (restante == 0)
        {
            registraLog("[RR] Processo PID %d finalizado", getPidProcesso(pcb));
        }
        else
        {
            // ainda tem tempo, preempta e reinsere no fim da fila
            setEstadoProcesso(pcb, READY);
            enfileira(fila, pcb);
        }
    }

    registraLog("Escalonador terminou execução de todos processos");
    return NULL;
}
