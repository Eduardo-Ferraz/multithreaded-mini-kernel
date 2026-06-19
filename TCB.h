#ifndef _TCB_H
#define _TCB_H

#include "PCB.h"
#include "EstadoProcesso.h"

typedef struct tcb TCB;

TCB *criaTCB(PCB *pcb, int indice);
void destroiTCB(TCB *tcb);
PCB *getPCB(TCB *tcb);
int getIndice(TCB *tcb);

#endif