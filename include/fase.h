#ifndef FASE_H
#define FASE_H

#include "tipos.h"

/* Le fases/faseN.txt para dentro de *fase. Retorna 1 se deu certo, 0 se falhou. */
int carregarFase(Fase *fase, int numero);

/* Libera a memoria do mapa. */
void liberarFase(Fase *fase);

/* Desenha o mapa na tela. */
void imprimirFase(const Fase *fase);

#endif