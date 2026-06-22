#include "tempo.h"
#include <sys/time.h>
#include <unistd.h>

static long inicio = 0;

// tempo absoluto atual em ms
static long relogio_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000L + tv.tv_usec / 1000L;
}

void inicia_relogio(void)
{
    inicio = relogio_ms();
}

long agora_ms(void)
{
    return relogio_ms() - inicio;
}

void dorme_ms(long ms)
{
    // usleep pode falhar com valores >= 1.000.000 us, entao dorme em fatias
    while (ms > 0)
    {
        long fatia = (ms > 500) ? 500 : ms;
        usleep(fatia * 1000);
        ms -= fatia;
    }
}
