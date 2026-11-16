#ifndef JOGO_H
#define JOGO_H

#define TOTAL_FASES 3

#define DURACAO_PASSO 0.12f

typedef enum Celula
{
   PAREDE = '#',
   CHAO = ' ',
   DESTINO = '.',
   CAIXA = '$',
   CAIXA_NO_DESTINO = '*',
   JOGADOR = '@',
   JOGADOR_NO_DESTINO = '+'
} Celula;

typedef struct Jogada
{
   int dl;
   int dc;
   int empurrou;
   struct Jogada *proxima;
} Jogada;

typedef struct
{
   int dl;
   int dc;
   int empurrou;
   float progresso;
} Animacao;

typedef struct
{
   char **mapa;
   int linhas;
   int colunas;
   int jogadorL;
   int jogadorC;
   int fase;
   int movimentosFase;
   int movimentosTotal;
   Jogada *historico;
   Animacao anim;
} Jogo;

int carregarFase(Jogo *j, int fase);

void liberarJogo(Jogo *j);

int mover(Jogo *j, int dl, int dc);

int desfazer(Jogo *j);

int faseVencida(const Jogo *j);

void atualizarAnimacao(Jogo *j, float dt);

int animando(const Jogo *j);

#endif