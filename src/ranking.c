/* =====================================================================
   src/ranking.c

   Ranking (top scores) com lista encadeada ordenada e arquivo texto.
   Formato do arquivo dados/ranking.txt, uma pontuação por linha:
       <movimentos> <nome>
   Exemplo:
       14 Borba
       20 Lucas
   Não usa raylib.
   ===================================================================== */

#include <stdio.h>    /* fopen, fgets, fprintf, sscanf, fclose */
#include <stdlib.h>   /* malloc, free */
#include <string.h>   /* strncpy */
#include "ranking.h"

/* Insere um nó novo mantendo a lista em ordem crescente de movimentos
   (menos movimentos = melhor = mais perto do início).
   Devolve o início da lista, que muda se o novo nó entrar em primeiro. */
Pontuacao *inserirPontuacao(Pontuacao *lista, const char *nome, int movimentos)
{
    Pontuacao *novo = malloc(sizeof *novo);
    Pontuacao *atual;

    if (novo == NULL)
        return lista;               /* sem memória: lista fica como estava */

    /* copia o nome com limite de tamanho e garante o '\0' no final */
    strncpy(novo->nome, nome, MAX_NOME - 1);
    novo->nome[MAX_NOME - 1] = '\0';
    novo->movimentos = movimentos;

    /* caso 1: lista vazia ou novo resultado melhor que o primeiro:
       o novo nó vira o início da lista */
    if (lista == NULL || movimentos < lista->movimentos) {
        novo->proxima = lista;
        return novo;
    }

    /* caso 2: anda pela lista até achar o último nó que não é pior
       que o novo, e encaixa o novo logo depois dele */
    atual = lista;
    while (atual->proxima != NULL && atual->proxima->movimentos <= movimentos)
        atual = atual->proxima;
    novo->proxima = atual->proxima;
    atual->proxima = novo;
    return lista;
}

/* Lê o arquivo linha por linha e insere cada pontuação na lista.
   Sem arquivo, devolve NULL (ranking vazio, normal na primeira vez). */
Pontuacao *carregarRanking(const char *caminho)
{
    Pontuacao *lista = NULL;
    char linha[128];
    char nome[MAX_NOME];
    int movimentos;
    FILE *arq = fopen(caminho, "r");

    if (arq == NULL)
        return NULL;

    while (fgets(linha, sizeof linha, arq) != NULL) {
        /* %d lê o número; %19[^\r\n] lê até 19 caracteres que não sejam
           quebra de linha (o nome pode ter espaços). sscanf devolve
           quantos campos conseguiu ler: só aceita a linha se leu os 2. */
        if (sscanf(linha, "%d %19[^\r\n]", &movimentos, nome) == 2)
            lista = inserirPontuacao(lista, nome, movimentos);
    }
    fclose(arq);
    return lista;
}

/* Grava no arquivo as MAX_RANKING primeiras pontuações da lista.
   Abrir com "w" apaga o conteúdo antigo e escreve tudo de novo. */
int salvarRanking(const Pontuacao *lista, const char *caminho)
{
    int quantas = 0;
    FILE *arq = fopen(caminho, "w");

    if (arq == NULL)
        return 0;

    while (lista != NULL && quantas < MAX_RANKING) {
        fprintf(arq, "%d %s\n", lista->movimentos, lista->nome);
        lista = lista->proxima;
        quantas++;
    }
    fclose(arq);
    return 1;
}

/* Libera todos os nós, guardando o próximo antes de cada free. */
void liberarRanking(Pontuacao *lista)
{
    while (lista != NULL) {
        Pontuacao *proxima = lista->proxima;
        free(lista);
        lista = proxima;
    }
}