#ifndef _ESTADO_H
#define _ESTADO_H

typedef enum estadoProcesso EstadoProcesso;

enum estadoProcesso{
    READY,
    RUNNING,
    FINISHED
};

#endif