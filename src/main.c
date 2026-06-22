#include "tempo.h"
#include <stdio.h>

int main(void)
{
    inicia_relogio();

    long t0 = agora_ms();
    dorme_ms(300);
    long t1 = agora_ms();
    dorme_ms(500);
    long t2 = agora_ms();

    printf("t0 %ld\n", t0);
    printf("t1 %ld\n", t1);
    printf("t2 %ld\n", t2);

    return 0;
}
