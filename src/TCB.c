#include "TCB.h"
#include <stdio.h>
#include <stdlib.h>

struct tcb
{
    PCB *pcb;
    int indice;
};

TCB *criaTCB(PCB *pcb, int indice)
{
    TCB *tcb = malloc(sizeof(TCB));
    if (tcb == NULL)
    {
        fprintf(stderr, "Erro ao criar TCB do processo %d\n", getPidProcesso(pcb));
        exit(EXIT_FAILURE);
    }

    tcb->pcb = pcb;
    tcb->indice = indice;

    return tcb;
}

void destroiTCB(TCB *tcb)
{
    if (tcb != NULL)
    {
        free(tcb);
    }
}

PCB *getPCB(TCB *tcb)
{
    return tcb->pcb;
}

int getIndice(TCB *tcb)
{
    return tcb->indice;
}
