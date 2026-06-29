#include "log.h"
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>

// buffer de log com vetor fixo de linhas, suficiente para qualquer caso de teste
// protegido por mutex porque escalonador e workers podem registrar juntos
#define MAX_LINHAS 8192
#define TAM_LINHA 128

static char linhas[MAX_LINHAS][TAM_LINHA];
static int n = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void registraLog(const char *fmt, ...)
{
    pthread_mutex_lock(&mutex);

    if (n < MAX_LINHAS)
    {
        va_list ap;

        va_start(ap, fmt);
        vsnprintf(linhas[n], TAM_LINHA, fmt, ap);
        va_end(ap);

        n++;
    }

    pthread_mutex_unlock(&mutex);
}

void salvaLog(const char *arquivo)
{
    FILE *f = fopen(arquivo, "w");

    for (int i = 0; i < n; i++)
    {
        fprintf(f, "%s\n", linhas[i]);
    }

    fclose(f);
}
