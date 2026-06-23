#ifndef _ESCALONADOR_H
#define _ESCALONADOR_H

#include "FilaProntos.h"

// rotina de thread do escalonador FCFS (arg = FilaProntos *)
void *escalonadorFCFS(void *arg);

#endif
