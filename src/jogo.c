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

/* Devolve o ENDEREÇO da célula (l, c) do mapa, ou NULL se a posição
   estiver fora do mapa. Devolver um ponteiro permite que quem chamou
   leia e também altere a célula. */
static char *celula(const Jogo *j, int l, int c)
{
    if (l < 0 || l >= j->linhas || c < 0 || c >= j->colunas)
        return NULL;
    return &j->mapa[l][c];
}

/* A célula tem destino? (vazio, com caixa ou com o jogador em cima) */
static int temDestino(char c)
{
    return c == DESTINO || c == CAIXA_NO_DESTINO || c == JOGADOR_NO_DESTINO;
}

/* A célula tem caixa? (fora do destino ou em cima de um destino) */
static int temCaixa(char c)
{
    return c == CAIXA || c == CAIXA_NO_DESTINO;
}

/* Coloca um objeto (CAIXA, JOGADOR ou CHAO para esvaziar) na célula,
   sem perder a informação de destino que já estava nela. Exemplo:
   colocar CAIXA numa célula DESTINO resulta em CAIXA_NO_DESTINO. */
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

/* Tenta mover o jogador uma casa na direção (dl, dc).
   Tratamento de colisão:
     - parede (ou fora do mapa) na frente: não anda;
     - caixa na frente: só anda se a casa DEPOIS da caixa estiver livre
       (sem parede e sem outra caixa); aí a caixa é empurrada junto.
   Cada movimento feito vira um nó novo no início da lista do histórico. */
int mover(Jogo *j, int dl, int dc){
    char *origem = celula(j, j->jogadorL, j->jogadorC);
    char *dest = celula(j, j->jogadorL + dl, j->jogadorC + dc);
    char *alem = NULL;              /* casa depois da caixa */
    Jogada *nova;
    int empurrou = 0;

    /* colisão com parede ou borda */
    if (dest == NULL || *dest == PAREDE)
        return 0;

    /* colisão com caixa: verifica se dá para empurrar */
    if (temCaixa(*dest)) {
        alem = celula(j, j->jogadorL + 2 * dl, j->jogadorC + 2 * dc);
        if (alem == NULL || *alem == PAREDE || temCaixa(*alem))
            return 0;               /* caixa travada: ninguém se mexe */
        empurrou = 1;
    }
       /* cria o nó da jogada antes de mexer no mapa: se faltar memória,
       nada muda e o jogo continua consistente */
    nova = malloc(sizeof *nova);
    if (nova == NULL)
        return 0;

    /* atualiza o mapa: caixa vai para frente, jogador sai e entra */
    if (empurrou)
        colocar(alem, CAIXA);
    colocar(origem, CHAO);
    colocar(dest, JOGADOR);
    j->jogadorL += dl;
    j->jogadorC += dc;
    j->movimentosFase++;

    /* empilha a jogada: o novo nó aponta para o antigo primeiro */
    nova->dl = dl;
    nova->dc = dc;
    nova->empurrou = empurrou;
    nova->proxima = j->historico;
    j->historico = nova;

    /* começa a animação deste passo do zero */
    j->anim.dl = dl;
    j->anim.dc = dc;
    j->anim.empurrou = empurrou;
    j->anim.progresso = 0.0f;
    return 1;
}

/* Desfaz a última jogada: tira o primeiro nó da lista e faz o
   movimento ao contrário (jogador volta e, se tinha empurrado,
   a caixa volta para onde o jogador estava). */
int desfazer(Jogo *j)
{
    Jogada *ultima = j->historico;
    int l = j->jogadorL;
    int c = j->jogadorC;

    if (ultima == NULL)
        return 0;                   /* nada para desfazer */

    colocar(&j->mapa[l][c], CHAO);  /* jogador sai da posição atual */
    if (ultima->empurrou) {
        /* a caixa está na casa à frente do jogador: volta uma casa */
        colocar(&j->mapa[l + ultima->dl][c + ultima->dc], CHAO);
        colocar(&j->mapa[l][c], CAIXA);
    }
    j->jogadorL = l - ultima->dl;   /* jogador volta uma casa */
    j->jogadorC = c - ultima->dc;
    colocar(&j->mapa[j->jogadorL][j->jogadorC], JOGADOR);
    j->movimentosFase--;

    /* desempilha: o início da lista passa a ser o nó seguinte */
    j->historico = ultima->proxima;
    free(ultima);

    j->anim.progresso = 1.0f;       /* desfazer não anima */
    return 1;
}

/* A fase termina quando nenhuma linha tem CAIXA (caixa fora do destino).
   strchr procura um caractere numa string e devolve NULL se não achar. */
int faseVencida(const Jogo *j)
{
    for (int l = 0; l < j->linhas; l++)
        if (strchr(j->mapa[l], CAIXA) != NULL)
            return 0;
    return 1;
}

/* Avança a animação proporcionalmente ao tempo que passou. Com
   DURACAO_PASSO = 0.12 s, o progresso vai de 0 a 1 em 0,12 segundo,
   não importa quantos quadros por segundo o computador consiga. */
void atualizarAnimacao(Jogo *j, float dt)
{
    if (j->anim.progresso < 1.0f) {
        j->anim.progresso += dt / DURACAO_PASSO;
        if (j->anim.progresso > 1.0f)
            j->anim.progresso = 1.0f;
    }
}

int animando(const Jogo *j)
{
    return j->anim.progresso < 1.0f;
}