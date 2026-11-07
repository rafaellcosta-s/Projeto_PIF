/* =====================================================================
   src/tela.c

   Toda a parte visual do jogo, feita com a raylib. Cada função desenha
   uma tela a partir do estado guardado nos tipos do jogo; nenhuma delas muda
   o estado do jogo (por isso recebem ponteiros "const").

   Obs.: a fonte padrão da raylib não tem acento, então os textos que
   aparecem na janela estão sem acento de propósito.
   ===================================================================== */

#include <stdio.h>    /* snprintf, para montar textos com números */
#include "raylib.h"
#include "tela.h"

#define TOPO_HUD 80          /* altura reservada no topo para o placar */
#define RODAPE 50            /* altura reservada embaixo para a ajuda */
#define TAM_MAX_BLOCO 80     /* tamanho máximo de cada casa, em pixels */

/* Centro da janela. As telas de aviso e de ranking são posicionadas a
   partir daqui, então continuam centralizadas se o tamanho da janela
   (LARGURA_TELA e ALTURA_TELA, no tela.h) for mudado. */
#define CENTRO_X (LARGURA_TELA / 2)
#define CENTRO_Y (ALTURA_TELA / 2)

/* Largura das caixas de aviso (fase concluída e vitória). */
#define LARGURA_PAINEL 640

/* Cores no formato {vermelho, verde, azul, opacidade}, de 0 a 255. */
static const Color COR_FUNDO    = {24, 26, 33, 255};
static const Color COR_CHAO     = {52, 56, 68, 255};
static const Color COR_PAREDE   = {120, 85, 60, 255};
static const Color COR_DESTINO  = {90, 200, 120, 255};
static const Color COR_CAIXA    = {225, 170, 60, 255};
static const Color COR_CAIXA_OK = {90, 200, 120, 255};
static const Color COR_JOGADOR  = {90, 160, 240, 255};

/* Calcula o tamanho de cada casa para o mapa inteiro caber na janela:
   pega o menor entre o que cabe na largura e o que cabe na altura. */
static int tamanhoBloco(const Jogo *j)
{
    int porLargura = (LARGURA_TELA - 80) / j->colunas;
    int porAltura = (ALTURA_TELA - TOPO_HUD - RODAPE - 20) / j->linhas;
    int tam = porLargura < porAltura ? porLargura : porAltura;
    return tam > TAM_MAX_BLOCO ? TAM_MAX_BLOCO : tam;
}

/* Escreve um texto centralizado na horizontal. MeasureText devolve a
   largura do texto em pixels, então o x inicial é o que sobra / 2. */
static void textoCentro(const char *texto, int y, int tamanho, Color cor)
{
    int largura = MeasureText(texto, tamanho);
    DrawText(texto, (LARGURA_TELA - largura) / 2, y, tamanho, cor);
}

/* Desenha o "chão" de uma casa: parede, chão vazio ou destino.
   Caixas e jogador são desenhados depois, por cima, porque eles podem
   estar no meio de uma animação (entre duas casas). */
static void desenharFundo(char c, int x, int y, int t)
{
    Rectangle r = {(float)x, (float)y, (float)t, (float)t};

    if (c == PAREDE) {
        DrawRectangleRec(r, COR_PAREDE);
        DrawRectangleLinesEx(r, 2, Fade(BLACK, 0.35f));  /* borda escura */
        return;
    }
    DrawRectangleRec(r, COR_CHAO);
    DrawRectangleLinesEx(r, 1, Fade(BLACK, 0.25f));

    /* destino: bolinha verde no centro da casa */
    if (c == DESTINO || c == CAIXA_NO_DESTINO || c == JOGADOR_NO_DESTINO)
        DrawCircle(x + t / 2, y + t / 2, t * 0.15f, COR_DESTINO);
}

/* Desenha uma caixa com o canto superior esquerdo da casa em (x, y).
   Recebe float porque, durante a animação, a posição fica "quebrada". */
static void desenharCaixa(float x, float y, int t, int noDestino)
{
    Rectangle caixa = {x + t * 0.12f, y + t * 0.12f, t * 0.76f, t * 0.76f};
    DrawRectangleRounded(caixa, 0.2f, 6, noDestino ? COR_CAIXA_OK : COR_CAIXA);
}

void desenharJogo(const Jogo *j)
{
    int t = tamanhoBloco(j);
    /* (x0, y0) é onde a casa [0][0] começa, para o mapa ficar centralizado */
    int x0 = (LARGURA_TELA - j->colunas * t) / 2;
    int y0 = TOPO_HUD + (ALTURA_TELA - TOPO_HUD - RODAPE - j->linhas * t) / 2;
    char placar[64];

    /* ---- Animação --------------------------------------------------
       O mapa já guarda o jogador (e a caixa empurrada) na casa NOVA.
       Para o movimento parecer contínuo, eles são desenhados um pouco
       ATRÁS da casa nova, na direção contrária ao passo, e esse recuo
       diminui até zero conforme anim.progresso vai de 0 a 1.
       A conta 1 - (1 - p)^2 faz o movimento começar rápido e frear
       no final, o que fica mais natural que velocidade constante. */
    float p = j->anim.progresso;
    float suave = 1.0f - (1.0f - p) * (1.0f - p);
    float recuo = (1.0f - suave) * t;            /* em pixels */
    float rx = -j->anim.dc * recuo;              /* recuo horizontal */
    float ry = -j->anim.dl * recuo;              /* recuo vertical */

    /* casa onde está a caixa que acabou de ser empurrada (se houver) */
    int caixaL = j->jogadorL + j->anim.dl;
    int caixaC = j->jogadorC + j->anim.dc;
    int caixaAnimando = animando(j) && j->anim.empurrou;

    ClearBackground(COR_FUNDO);

    /* placar no topo */
    snprintf(placar, sizeof placar, "Fase %d/%d   Movimentos: %d",
             j->fase + 1, TOTAL_FASES, j->movimentosTotal + j->movimentosFase);
    DrawText("PUSH BLOCK", 30, 25, 36, RAYWHITE);
    DrawText(placar, LARGURA_TELA - MeasureText(placar, 24) - 30, 32, 24, LIGHTGRAY);

    /* 1ª camada: percorre a matriz e desenha o fundo de cada casa */
    for (int l = 0; l < j->linhas; l++)
        for (int c = 0; c < j->colunas; c++)
            desenharFundo(j->mapa[l][c], x0 + c * t, y0 + l * t, t);

    /* 2ª camada: as caixas. A que está sendo empurrada leva o recuo. */
    for (int l = 0; l < j->linhas; l++) {
        for (int c = 0; c < j->colunas; c++) {
            char cel = j->mapa[l][c];
            float x = (float)(x0 + c * t);
            float y = (float)(y0 + l * t);
            if (cel != CAIXA && cel != CAIXA_NO_DESTINO)
                continue;
            if (caixaAnimando && l == caixaL && c == caixaC) {
                x += rx;
                y += ry;
            }
            desenharCaixa(x, y, t, cel == CAIXA_NO_DESTINO);
        }
    }

    /* 3ª camada: o jogador, também com o recuo da animação */
    DrawCircle((int)(x0 + j->jogadorC * t + t / 2 + rx),
               (int)(y0 + j->jogadorL * t + t / 2 + ry),
               t * 0.35f, COR_JOGADOR);

    /* ajuda de teclas no rodapé */
    textoCentro("WASD/Setas: mover   U: desfazer   R: reiniciar   ESC: sair",
                ALTURA_TELA - 38, 20, GRAY);
}

/* Escurece a tela inteira e desenha uma caixa centralizada, usada nos
   avisos de fase concluída e de vitória. Fade deixa a cor semitransparente.
   Devolve o y do topo da caixa, para quem chamou posicionar os textos. */
static int painel(int altura)
{
    int x = CENTRO_X - LARGURA_PAINEL / 2;
    int y = CENTRO_Y - altura / 2;

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, 0.6f));
    DrawRectangle(x, y, LARGURA_PAINEL, altura, COR_FUNDO);
    DrawRectangleLines(x, y, LARGURA_PAINEL, altura, LIGHTGRAY);
    return y;
}

void desenharFaseConcluida(const Jogo *j)
{
    char texto[64];
    int topo;

    desenharJogo(j);                /* o mapa continua aparecendo atrás */
    topo = painel(180);
    snprintf(texto, sizeof texto, "Fase %d concluida!", j->fase + 1);
    textoCentro(texto, topo + 45, 40, COR_DESTINO);
    textoCentro("Aperte ENTER para a proxima fase", topo + 115, 24, RAYWHITE);
}

void desenharDigitandoNome(const Jogo *j, const char *nome)
{
    char texto[64];
    int topo, campoX, campoY;

    desenharJogo(j);
    topo = painel(280);
    textoCentro("Voce venceu!", topo + 30, 44, COR_DESTINO);
    snprintf(texto, sizeof texto, "Total: %d movimentos", j->movimentosTotal);
    textoCentro(texto, topo + 90, 26, RAYWHITE);
    textoCentro("Digite seu nome e aperte ENTER:", topo + 135, 22, LIGHTGRAY);

    /* campo de texto com o nome digitado até agora, centralizado */
    campoX = CENTRO_X - 220;
    campoY = topo + 180;
    DrawRectangle(campoX, campoY, 440, 50, COR_CHAO);
    DrawText(nome, campoX + 14, campoY + 12, 28, RAYWHITE);

    /* cursor piscando: GetTime() é o tempo desde que o jogo abriu;
       (int)(tempo * 2) muda a cada meio segundo, e % 2 alterna 0 e 1 */
    if (((int)(GetTime() * 2)) % 2 == 0)
        DrawRectangle(campoX + 16 + MeasureText(nome, 28), campoY + 12, 3, 28, RAYWHITE);
}

void desenharRanking(const Pontuacao *lista, int movimentos)
{
    char linha[64];
    int posicao = 1;
    int y = 210;
    int colunaNome = CENTRO_X - 250;      /* onde começam os nomes */
    int colunaNumero = CENTRO_X + 250;    /* onde terminam os números */

    ClearBackground(COR_FUNDO);
    textoCentro("RANKING", 80, 52, RAYWHITE);
    textoCentro("(menos movimentos e melhor)", 142, 22, GRAY);

    if (lista == NULL)
        textoCentro("Ainda nao ha pontuacoes.", y, 26, LIGHTGRAY);

    /* percorre a lista encadeada, no máximo MAX_EXIBIDOS nós */
    while (lista != NULL && posicao <= MAX_EXIBIDOS) {
        Color cor = posicao == 1 ? COR_CAIXA : RAYWHITE;   /* 1º lugar em destaque */

        snprintf(linha, sizeof linha, "%d. %s", posicao, lista->nome);
        DrawText(linha, colunaNome, y, 30, cor);

        /* número alinhado à direita, terminando na colunaNumero */
        snprintf(linha, sizeof linha, "%d", lista->movimentos);
        DrawText(linha, colunaNumero - MeasureText(linha, 30), y, 30, cor);

        y += 52;
        lista = lista->proxima;
        posicao++;
    }

    snprintf(linha, sizeof linha, "Sua partida: %d movimentos", movimentos);
    textoCentro(linha, ALTURA_TELA - 150, 24, LIGHTGRAY);
    textoCentro("ENTER: jogar de novo   ESC: sair", ALTURA_TELA - 80, 22, GRAY);
}