#ifndef _TEMPO_H
#define _TEMPO_H

// marca o instante inicial
void inicia_relogio(void);

// milissegundos decorridos desde inicia_relogio
long agora_ms(void);

// bloqueia a thread
void dorme_ms(long ms);

#endif
