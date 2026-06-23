#include "escalonador.h"
#include "PCB.h"
#include "log.h"

// FCFS: atende os processos na ordem da fila, cada um ate o fim.
void *escalonadorFCFS(void *arg)
{
    FilaProntos *fila = (FilaProntos *)arg;
    PCB *pcb;

    // pega processos ate a fila esvaziar E ter sido encerrada (retorna NULL)
    while((pcb = desenfileiraEspera(fila)) != NULL)
    {
        setEstadoProcesso(pcb, RUNNING);   // acorda as threads do processo
        registraLog("[FCFS] Executando processo PID %d", getPidProcesso(pcb));

        aguardaFimProcesso(pcb);           // espera o processo terminar por completo
        registraLog("[FCFS] Processo PID %d finalizado", getPidProcesso(pcb));
    }

    registraLog("Escalonador terminou execução de todos processos");
    return NULL;
}
