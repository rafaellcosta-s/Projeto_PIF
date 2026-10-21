#ifndef JOGO_H
#define JOGO_H

#define MAX_LINHAS 10
#define MAX_COLUNAS 12
#define TOTAL_FASES 3

struct Jogo {
    char mapa[MAX_LINHAS][MAX_COLUNAS];
    int linhas;
    int jogadorL;
    int jogadorC;
    int fase;
};

void carregarFase(struct Jogo *j, int fase);
int mover(struct Jogo *j, int dl, int dc);
int faseVencida(const struct Jogo *j);
void desenhar(const struct Jogo *j);

#endif