#ifndef TIPOS_H
#define TIPOS_H

typedef struct {
    int linha;
    int coluna;
} Posicao;

typedef struct {
    char **mapa;        /* matriz dinamica: mapa[linha][coluna] */
    int linhas;
    int colunas;
    Posicao jogador;
    int caixas;         /* total de caixas na fase */
    int destinos;       /* total de destinos na fase */
} Fase;

/* No da lista encadeada do historico (usada na etapa 4) */
typedef struct Jogada {
    int dLinha;             /* direcao do movimento: -1, 0 ou 1 */
    int dColuna;
    int empurrouCaixa;      /* 1 se a jogada empurrou uma caixa */
    struct Jogada *proxima;
} Jogada;

/* No da lista encadeada do ranking (usada na etapa 5) */
typedef struct Pontuacao {
    char nome[50];
    int movimentos;
    struct Pontuacao *proxima;
} Pontuacao;

#endif