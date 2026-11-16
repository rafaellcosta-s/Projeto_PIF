#ifndef RANKING_H
#define RANKING_H

#define MAX_NOME 20
#define MAX_RANKING 10
#define MAX_EXIBIDOS 5

#define ARQUIVO_RANKING "dados/ranking.txt"

typedef struct Pontuacao
{
   char nome[MAX_NOME];
   int movimentos;
   struct Pontuacao *proxima;
} Pontuacao;

Pontuacao *carregarRanking(const char *caminho);

Pontuacao *inserirPontuacao(Pontuacao *lista, const char *nome, int movimentos);

int salvarRanking(const Pontuacao *lista, const char *caminho);

void liberarRanking(Pontuacao *lista);

#endif