#include "FilaProntos.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct filaProntos
{
    PCB **buffer; // vetor circular de ponteiros para PCB
    int capacidade;
    int inicio;    // posicao do primeiro elemento
    int qtd;       // quantidade atual de elementos
    int encerrada; // flag de encerramento

    pthread_mutex_t mutex;
    pthread_cond_t naoVazia;
};

FilaProntos *criaFilaProntos(int capacidade)
{
    FilaProntos *fila = malloc(sizeof(FilaProntos));
    if (fila == NULL)
    {
        fprintf(stderr, "Erro ao criar fila de prontos\n");
        exit(EXIT_FAILURE);
    }

    fila->buffer = malloc(sizeof(PCB *) * capacidade);
    if (fila->buffer == NULL)
    {
        fprintf(stderr, "Erro ao criar buffer da fila de prontos\n");
        exit(EXIT_FAILURE);
    }

    fila->capacidade = capacidade;
    fila->inicio = 0;
    fila->qtd = 0;
    fila->encerrada = 0;

    pthread_mutex_init(&fila->mutex, NULL);
    pthread_cond_init(&fila->naoVazia, NULL);

    return fila;
}

void destroiFilaProntos(FilaProntos *fila)
{
    if (fila != NULL)
    {
        pthread_cond_destroy(&fila->naoVazia);
        pthread_mutex_destroy(&fila->mutex);
        free(fila->buffer);
        free(fila);
    }
}

void enfileira(FilaProntos *fila, PCB *processo)
{
    pthread_mutex_lock(&fila->mutex);

    if (fila->qtd < fila->capacidade) // com capacidade n a fila nunca deve encher
    {
        int fim = (fila->inicio + fila->qtd) % fila->capacidade;
        fila->buffer[fim] = processo;
        fila->qtd++;
        pthread_cond_signal(&fila->naoVazia);
    }

    pthread_mutex_unlock(&fila->mutex);
}

// remove o primeiro elemento, so deve ser chamada com o mutex ja travado
static PCB *removeInicio(FilaProntos *fila)
{
    PCB *p = fila->buffer[fila->inicio];
    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->qtd--;
    return p;
}

PCB *desenfileira(FilaProntos *fila)
{
    pthread_mutex_lock(&fila->mutex);

    PCB *p = NULL;
    if (fila->qtd > 0)
    {
        p = removeInicio(fila);
    }

    pthread_mutex_unlock(&fila->mutex);
    return p;
}

PCB *desenfileiraEspera(FilaProntos *fila)
{
    pthread_mutex_lock(&fila->mutex);

    while (fila->qtd == 0 && !fila->encerrada)
    {
        pthread_cond_wait(&fila->naoVazia, &fila->mutex);
    }

    PCB *p = NULL;
    if (fila->qtd > 0) // se vazia e encerrada, retorna NULL
    {
        p = removeInicio(fila);
    }

    pthread_mutex_unlock(&fila->mutex);
    return p;
}

// bloqueia ate ter processo ou encerrar e remove o de maior prioridade
// menor numero, no empate fica com o mais antigo na fila
PCB *desenfileiraPrioridade(FilaProntos *fila)
{
    // não mexer

    pthread_mutex_lock(&fila->mutex);

    while (fila->qtd == 0 && !fila->encerrada)
    {
        pthread_cond_wait(&fila->naoVazia, &fila->mutex);
    }

    PCB *escolhido = NULL;
    if (fila->qtd > 0)
    {
        int melhor = 0;
        for (int i = 1; i < fila->qtd; i++)
        {
            PCB *cand = fila->buffer[(fila->inicio + i) % fila->capacidade];
            PCB *atual = fila->buffer[(fila->inicio + melhor) % fila->capacidade];
            if (getPrioridadeProcesso(cand) < getPrioridadeProcesso(atual))
            {
                melhor = i;
            }
        }

        escolhido = fila->buffer[(fila->inicio + melhor) % fila->capacidade];

        // remove o escolhido deslocando os seguintes uma posicao para tras
        for (int j = melhor; j < fila->qtd - 1; j++)
        {
            int a = (fila->inicio + j) % fila->capacidade;
            int b = (fila->inicio + j + 1) % fila->capacidade;
            fila->buffer[a] = fila->buffer[b];
        }
        fila->qtd--;
    }

    pthread_mutex_unlock(&fila->mutex);
    return escolhido;
}

int removeProcesso(FilaProntos *fila, PCB *processo)
{
    pthread_mutex_lock(&fila->mutex);

    int removeu = 0;
    for (int i = 0; i < fila->qtd; i++)
    {
        int pos = (fila->inicio + i) % fila->capacidade;
        if (fila->buffer[pos] == processo)
        {
            // desloca os elementos seguintes uma posicao para tras mantendo a ordem
            for (int j = i; j < fila->qtd - 1; j++)
            {
                int atual = (fila->inicio + j) % fila->capacidade;
                int prox = (fila->inicio + j + 1) % fila->capacidade;
                fila->buffer[atual] = fila->buffer[prox];
            }
            fila->qtd--;
            removeu = 1;
            break;
        }
    }

    pthread_mutex_unlock(&fila->mutex);
    return removeu;
}

PCB *espiaProcesso(FilaProntos *fila, int posicao)
{
    pthread_mutex_lock(&fila->mutex);

    PCB *p = NULL;
    if (posicao >= 0 && posicao < fila->qtd)
    {
        p = fila->buffer[(fila->inicio + posicao) % fila->capacidade];
    }

    pthread_mutex_unlock(&fila->mutex);
    return p;
}

int filaVazia(FilaProntos *fila)
{
    pthread_mutex_lock(&fila->mutex);
    int vazia = (fila->qtd == 0);
    pthread_mutex_unlock(&fila->mutex);
    return vazia;
}

int tamanhoFila(FilaProntos *fila)
{
    pthread_mutex_lock(&fila->mutex);
    int qtd = fila->qtd;
    pthread_mutex_unlock(&fila->mutex);
    return qtd;
}

void encerraFila(FilaProntos *fila)
{
    pthread_mutex_lock(&fila->mutex);
    fila->encerrada = 1;
    pthread_cond_broadcast(&fila->naoVazia);
    pthread_mutex_unlock(&fila->mutex);
}

// menor numero de prioridade presente na fila, INT_MAX se vazia
int prioridadeMaisAltaNaFila(FilaProntos *fila)
{
    pthread_mutex_lock(&fila->mutex);

    int melhor = INT_MAX;
    for (int i = 0; i < fila->qtd; i++)
    {
        PCB *p = fila->buffer[(fila->inicio + i) % fila->capacidade];
        if (getPrioridadeProcesso(p) < melhor)
        {
            melhor = getPrioridadeProcesso(p);
        }
    }

    pthread_mutex_unlock(&fila->mutex);
    return melhor;
}
