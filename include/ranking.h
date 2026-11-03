/* =====================================================================
   include/ranking.h

   "Contrato" do ranking (top scores): o tipo de cada pontuação e as
   funções que o ranking.c implementa. O ranking é uma lista encadeada
   mantida em ordem (menos movimentos primeiro) e salva em arquivo.

   Este arquivo não usa raylib.
   ===================================================================== */

#ifndef RANKING_H
#define RANKING_H

#define MAX_NOME 20        /* tamanho máximo do nome (com o '\0' do fim) */
#define MAX_RANKING 10     /* quantas pontuações ficam salvas no arquivo */
#define MAX_EXIBIDOS 5     /* quantas aparecem na tela de ranking */

/* Caminho do arquivo onde o ranking é lido e gravado. */
#define ARQUIVO_RANKING "dados/ranking.txt"

/* ---------------------------------------------------------------------
   Pontuacao: um nó da lista encadeada do ranking.
   A lista fica sempre ordenada: o primeiro nó é o melhor resultado
   (o que venceu as fases com menos movimentos).

   Como em Jogada, dentro da struct se escreve "struct Pontuacao *",
   porque o apelido só existe depois do fim do typedef.
   --------------------------------------------------------------------- */
typedef struct Pontuacao {
    char nome[MAX_NOME];        /* nome digitado pelo jogador */
    int movimentos;             /* total de movimentos na partida */
    struct Pontuacao *proxima;  /* próximo da lista (pior ou igual), ou NULL */
} Pontuacao;

/* Lê o arquivo e monta a lista. Se o arquivo não existir, devolve NULL
   (lista vazia), o que é normal na primeira vez que o jogo roda. */
Pontuacao *carregarRanking(const char *caminho);

/* Insere uma pontuação na posição certa da lista e devolve o início da
   lista (que muda quando a nova pontuação é a melhor de todas). */
Pontuacao *inserirPontuacao(Pontuacao *lista, const char *nome, int movimentos);

/* Grava as MAX_RANKING melhores pontuações no arquivo.
   Devolve 1 se conseguiu e 0 se não conseguiu abrir o arquivo. */
int salvarRanking(const Pontuacao *lista, const char *caminho);

/* Libera todos os nós da lista. */
void liberarRanking(Pontuacao *lista);

#endif