#ifndef _ESCALONADOR_H
#define _ESCALONADOR_H

#include "FilaProntos.h"

// rotinas de thread dos escalonadores (arg = FilaProntos *)
void *escalonadorFCFS(void *arg);
void *escalonadorRR(void *arg);

#endif
