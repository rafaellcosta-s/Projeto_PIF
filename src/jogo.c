/* =====================================================================
   src/jogo.c

   Regras do Push Block (Sokoban): carregar a fase de um arquivo, mover
   o jogador tratando as colisões, empurrar caixas, desfazer jogadas e
   verificar a vitória. Não usa raylib.
   ===================================================================== */

#include <stdio.h>    /* fopen, fgets, fclose, snprintf */
#include <stdlib.h>   /* malloc, free */
#include <string.h>   /* strcspn, strchr */
#include "jogo.h"

/* "static" numa função quer dizer que ela só existe dentro deste
   arquivo: é uma função auxiliar, que o main.c não enxerga. */

/* Apaga a lista de jogadas inteira, nó por nó. */
static void liberarHistorico(Jogo *j)
{
    while (j->historico != NULL) {
        /* guarda o próximo ANTES de liberar, senão perderíamos o ponteiro */
        Jogada *proxima = j->historico->proxima;
        free(j->historico);
        j->historico = proxima;
    }
}

/* Libera o mapa e o histórico. Pode ser chamada mais de uma vez sem
   problema, porque depois de liberar tudo os ponteiros viram NULL. */
void liberarJogo(Jogo *j)
{
    if (j->mapa != NULL) {
        /* primeiro libera cada linha, depois o vetor de linhas */
        for (int l = 0; l < j->linhas; l++)
            free(j->mapa[l]);
        free(j->mapa);
    }
    j->mapa = NULL;
    j->linhas = 0;
    j->colunas = 0;
    liberarHistorico(j);
}

/* Lê fases/fase<N>.txt e monta o mapa na memória.
   O arquivo é lido duas vezes: na primeira, só para descobrir quantas
   linhas e colunas o mapa tem (e saber quanta memória pedir); na
   segunda, para copiar os caracteres. No arquivo, '-' vale chão vazio
   (alguns editores apagam espaços no fim da linha, o '-' evita isso). */
int carregarFase(Jogo *j, int fase)
{
    char caminho[64];
    char linha[256];
    int linhas = 0, colunas = 0, jogadores = 0;
    FILE *arq;

    /* monta o nome do arquivo: fase 0 -> "fases/fase1.txt" */
    snprintf(caminho, sizeof caminho, "fases/fase%d.txt", fase + 1);
    arq = fopen(caminho, "r");
    if (arq == NULL)
        return 0;

    /* 1ª leitura: conta as linhas e acha a linha mais comprida */
    while (fgets(linha, sizeof linha, arq) != NULL) {
        /* strcspn devolve quantos caracteres vêm antes do \r ou \n,
           ou seja, o tamanho da linha sem a quebra de linha */
        int tam = (int)strcspn(linha, "\r\n");
        if (tam == 0)
            continue;               /* ignora linhas vazias */
        linhas++;
        if (tam > colunas)
            colunas = tam;
    }
    if (linhas == 0) {
        fclose(arq);
        return 0;
    }
    rewind(arq);                    /* volta para o começo do arquivo */

    /* apaga o mapa da fase anterior (se houver) e o histórico */
    liberarJogo(j);

    /* aloca o vetor de linhas: um ponteiro (char *) para cada linha */
    j->mapa = malloc(linhas * sizeof(char *));
    if (j->mapa == NULL) {
        fclose(arq);
        return 0;
    }
    j->colunas = colunas;

    /* 2ª leitura: cada linha do arquivo vira uma linha da matriz */
    while (j->linhas < linhas && fgets(linha, sizeof linha, arq) != NULL) {
        int tam = (int)strcspn(linha, "\r\n");
        char *copia;
        if (tam == 0)
            continue;

        /* colunas + 1 por causa do '\0' que termina a string */
        copia = malloc(colunas + 1);
        if (copia == NULL) {
            fclose(arq);
            liberarJogo(j);
            return 0;
        }
        for (int c = 0; c < colunas; c++) {
            /* linhas mais curtas são completadas com chão */
            char ch = c < tam ? linha[c] : CHAO;
            if (ch == '-')
                ch = CHAO;
            /* achou o jogador: guarda a posição dele */
            if (ch == JOGADOR || ch == JOGADOR_NO_DESTINO) {
                jogadores++;
                j->jogadorL = j->linhas;
                j->jogadorC = c;
            }
            copia[c] = ch;
        }
        copia[colunas] = '\0';
        j->mapa[j->linhas++] = copia;
    }
    fclose(arq);

    /* uma fase válida tem exatamente um jogador */
    if (jogadores != 1) {
        liberarJogo(j);
        return 0;
    }
    j->fase = fase;
    j->movimentosFase = 0;
    j->anim.progresso = 1.0f;       /* começa parado, sem animação */
    return 1;
}
