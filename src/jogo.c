#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jogo.h"

static void liberarHistorico(Jogo *j)
{
    while (j->historico != NULL)
    {
        Jogada *proxima = j->historico->proxima;
        free(j->historico);
        j->historico = proxima;
    }
}

void liberarJogo(Jogo *j)
{
    if (j->mapa != NULL)
    {

        for (int l = 0; l < j->linhas; l++)
            free(j->mapa[l]);
        free(j->mapa);
    }
    j->mapa = NULL;
    j->linhas = 0;
    j->colunas = 0;
    liberarHistorico(j);
}

int carregarFase(Jogo *j, int fase)
{
    char caminho[64];
    char linha[256];
    int linhas = 0, colunas = 0, jogadores = 0;
    FILE *arq;

    snprintf(caminho, sizeof caminho, "fases/fase%d.txt", fase + 1);
    arq = fopen(caminho, "r");
    if (arq == NULL)
        return 0;

    while (fgets(linha, sizeof linha, arq) != NULL)
    {

        int tam = (int)strcspn(linha, "\r\n");
        if (tam == 0)
            continue;
        linhas++;
        if (tam > colunas)
            colunas = tam;
    }
    if (linhas == 0)
    {
        fclose(arq);
        return 0;
    }
    rewind(arq);

    liberarJogo(j);

    j->mapa = malloc(linhas * sizeof(char *));
    if (j->mapa == NULL)
    {
        fclose(arq);
        return 0;
    }
    j->colunas = colunas;

    while (j->linhas < linhas && fgets(linha, sizeof linha, arq) != NULL)
    {
        int tam = (int)strcspn(linha, "\r\n");
        char *copia;
        if (tam == 0)
            continue;

        copia = malloc(colunas + 1);
        if (copia == NULL)
        {
            fclose(arq);
            liberarJogo(j);
            return 0;
        }
        for (int c = 0; c < colunas; c++)
        {

            char ch = c < tam ? linha[c] : CHAO;
            if (ch == '-')
                ch = CHAO;

            if (ch == JOGADOR || ch == JOGADOR_NO_DESTINO)
            {
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

    if (jogadores != 1)
    {
        liberarJogo(j);
        return 0;
    }
    j->fase = fase;
    j->movimentosFase = 0;
    j->anim.progresso = 1.0f;
    return 1;
}

static char *celula(const Jogo *j, int l, int c)
{
    if (l < 0 || l >= j->linhas || c < 0 || c >= j->colunas)
        return NULL;
    return &j->mapa[l][c];
}

static int temDestino(char c)
{
    return c == DESTINO || c == CAIXA_NO_DESTINO || c == JOGADOR_NO_DESTINO;
}

static int temCaixa(char c)
{
    return c == CAIXA || c == CAIXA_NO_DESTINO;
}

static void colocar(char *cel, Celula objeto)
{
    int dest = temDestino(*cel);
    if (objeto == CAIXA)
        *cel = dest ? CAIXA_NO_DESTINO : CAIXA;
    else if (objeto == JOGADOR)
        *cel = dest ? JOGADOR_NO_DESTINO : JOGADOR;
    else
        *cel = dest ? DESTINO : CHAO;
}

int mover(Jogo *j, int dl, int dc)
{
    char *origem = celula(j, j->jogadorL, j->jogadorC);
    char *dest = celula(j, j->jogadorL + dl, j->jogadorC + dc);
    char *alem = NULL;
    Jogada *nova;
    int empurrou = 0;

    if (dest == NULL || *dest == PAREDE)
        return 0;

    if (temCaixa(*dest))
    {
        alem = celula(j, j->jogadorL + 2 * dl, j->jogadorC + 2 * dc);
        if (alem == NULL || *alem == PAREDE || temCaixa(*alem))
            return 0;
        empurrou = 1;
    }

    nova = malloc(sizeof *nova);
    if (nova == NULL)
        return 0;

    if (empurrou)
        colocar(alem, CAIXA);
    colocar(origem, CHAO);
    colocar(dest, JOGADOR);
    j->jogadorL += dl;
    j->jogadorC += dc;
    j->movimentosFase++;

    nova->dl = dl;
    nova->dc = dc;
    nova->empurrou = empurrou;
    nova->proxima = j->historico;
    j->historico = nova;

    j->anim.dl = dl;
    j->anim.dc = dc;
    j->anim.empurrou = empurrou;
    j->anim.progresso = 0.0f;
    return 1;
}

int desfazer(Jogo *j)
{
    Jogada *ultima = j->historico;
    int l = j->jogadorL;
    int c = j->jogadorC;

    if (ultima == NULL)
        return 0;

    colocar(&j->mapa[l][c], CHAO);
    if (ultima->empurrou)
    {

        colocar(&j->mapa[l + ultima->dl][c + ultima->dc], CHAO);
        colocar(&j->mapa[l][c], CAIXA);
    }
    j->jogadorL = l - ultima->dl;
    j->jogadorC = c - ultima->dc;
    colocar(&j->mapa[j->jogadorL][j->jogadorC], JOGADOR);
    j->movimentosFase--;

    j->historico = ultima->proxima;
    free(ultima);

    j->anim.progresso = 1.0f;
    return 1;
}

int faseVencida(const Jogo *j)
{
    for (int l = 0; l < j->linhas; l++)
        if (strchr(j->mapa[l], CAIXA) != NULL)
            return 0;
    return 1;
}

void atualizarAnimacao(Jogo *j, float dt)
{
    if (j->anim.progresso < 1.0f)
    {
        j->anim.progresso += dt / DURACAO_PASSO;
        if (j->anim.progresso > 1.0f)
            j->anim.progresso = 1.0f;
    }
}

int animando(const Jogo *j)
{
    return j->anim.progresso < 1.0f;
}