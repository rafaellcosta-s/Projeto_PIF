/* =====================================================================
   include/tela.h

   "Contrato" da parte visual: as funções que o tela.c implementa para
   desenhar cada tela do jogo na janela da raylib. É o único módulo
   (junto com o main.c) que conhece a raylib; jogo.c e ranking.c não.
   ===================================================================== */

#ifndef TELA_H
#define TELA_H

#include "jogo.h"      /* tipo Jogo, usado para desenhar a partida */
#include "ranking.h"   /* tipo Pontuacao, usado na tela de ranking */

/* Tamanho da janela, em pixels. */
#define LARGURA_TELA 1280
#define ALTURA_TELA 720

/* Desenha o mapa, o placar e a ajuda de teclas. */
void desenharJogo(const Jogo *j);

/* Desenha o jogo com um aviso de "fase concluída" por cima. */
void desenharFaseConcluida(const Jogo *j);

/* Desenha a tela de vitória, com o campo para digitar o nome. */
void desenharDigitandoNome(const Jogo *j, const char *nome);

/* Desenha a tela de ranking com as melhores pontuações. */
void desenharRanking(const Pontuacao *lista, int movimentos);

#endif