#ifndef TELA_H
#define TELA_H

#include "jogo.h"
#include "ranking.h"

#define LARGURA_TELA 1280
#define ALTURA_TELA 720

void desenharJogo(const Jogo *j);

void desenharFaseConcluida(const Jogo *j);

void desenharDigitandoNome(const Jogo *j, const char *nome);

void desenharRanking(const Pontuacao *lista, int movimentos);

#endif