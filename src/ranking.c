#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ranking.h"

Pontuacao *inserirPontuacao(Pontuacao *lista, const char *nome, int movimentos)
{
    Pontuacao *novo = malloc(sizeof *novo);
    Pontuacao *atual;

    if (novo == NULL)
        return lista;

    strncpy(novo->nome, nome, MAX_NOME - 1);
    novo->nome[MAX_NOME - 1] = '\0';
    novo->movimentos = movimentos;

    if (lista == NULL || movimentos < lista->movimentos)
    {
        novo->proxima = lista;
        return novo;
    }

    atual = lista;
    while (atual->proxima != NULL && atual->proxima->movimentos <= movimentos)
        atual = atual->proxima;
    novo->proxima = atual->proxima;
    atual->proxima = novo;
    return lista;
}

Pontuacao *carregarRanking(const char *caminho)
{
    Pontuacao *lista = NULL;
    char linha[128];
    char nome[MAX_NOME];
    int movimentos;
    FILE *arq = fopen(caminho, "r");

    if (arq == NULL)
        return NULL;

    while (fgets(linha, sizeof linha, arq) != NULL)
    {

        if (sscanf(linha, "%d %19[^\r\n]", &movimentos, nome) == 2)
            lista = inserirPontuacao(lista, nome, movimentos);
    }
    fclose(arq);
    return lista;
}

int salvarRanking(const Pontuacao *lista, const char *caminho)
{
    int quantas = 0;
    FILE *arq = fopen(caminho, "w");

    if (arq == NULL)
        return 0;

    while (lista != NULL && quantas < MAX_RANKING)
    {
        fprintf(arq, "%d %s\n", lista->movimentos, lista->nome);
        lista = lista->proxima;
        quantas++;
    }
    fclose(arq);
    return 1;
}

void liberarRanking(Pontuacao *lista)
{
    while (lista != NULL)
    {
        Pontuacao *proxima = lista->proxima;
        free(lista);
        lista = proxima;
    }
}