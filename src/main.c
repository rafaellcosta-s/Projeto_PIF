#include <string.h>
#include "raylib.h"
#include "jogo.h"
#include "ranking.h"
#include "tela.h"

typedef enum
{
    JOGANDO,
    FASE_CONCLUIDA,
    DIGITANDO_NOME,
    MOSTRANDO_RANKING
} Estado;

static int apertou(int tecla)
{
    return IsKeyPressed(tecla) || IsKeyPressedRepeat(tecla);
}

static int tecla(int a, int b, int segurando)
{
    if (segurando)
        return IsKeyDown(a) || IsKeyDown(b);
    return IsKeyPressed(a) || IsKeyPressed(b);
}

static int lerDirecao(int *dl, int *dc, int segurando)
{
    *dl = 0;
    *dc = 0;
    if (tecla(KEY_W, KEY_UP, segurando))
    {
        *dl = -1;
        return 1;
    }
    if (tecla(KEY_S, KEY_DOWN, segurando))
    {
        *dl = 1;
        return 1;
    }
    if (tecla(KEY_A, KEY_LEFT, segurando))
    {
        *dc = -1;
        return 1;
    }
    if (tecla(KEY_D, KEY_RIGHT, segurando))
    {
        *dc = 1;
        return 1;
    }
    return 0;
}

static void lerNome(char *nome, int max)
{
    int n = (int)strlen(nome);
    int c = GetCharPressed();

    while (c > 0)
    {
        if (c >= 32 && c < 127 && n < max - 1)
        {
            nome[n++] = (char)c;
            nome[n] = '\0';
        }
        c = GetCharPressed();
    }
    if (apertou(KEY_BACKSPACE) && n > 0)
        nome[--n] = '\0';
}

static int novaPartida(Jogo *j)
{
    j->movimentosTotal = 0;
    return carregarFase(j, 0);
}

int main(void)
{
    Jogo jogo = {0};
    Pontuacao *ranking = NULL;
    Estado estado = JOGANDO;
    char nome[MAX_NOME] = "";
    int dl, dc;
    int rodando = 1;

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Push Block");
    SetTargetFPS(60);

    if (!FileExists("fases/fase1.txt"))
    {
        ChangeDirectory(GetApplicationDirectory());
        ChangeDirectory("..");
    }

    if (!novaPartida(&jogo))
    {

        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Nao achei fases/fase1.txt", 40, 260, 30, RED);
            DrawText("Rode o jogo a partir da raiz do projeto.", 40, 310, 20, RAYWHITE);
            EndDrawing();
        }
        CloseWindow();
        return 1;
    }

    while (rodando && !WindowShouldClose())
    {

        switch (estado)
        {
        case JOGANDO:

            atualizarAnimacao(&jogo, GetFrameTime());

            if (IsKeyPressed(KEY_R))
            {
                carregarFase(&jogo, jogo.fase);
            }
            else if (apertou(KEY_U))
            {
                desfazer(&jogo);
            }
            else if (!faseVencida(&jogo) && lerDirecao(&dl, &dc, 0))
            {

                jogo.anim.progresso = 1.0f;
                mover(&jogo, dl, dc);
            }
            else if (!animando(&jogo) && !faseVencida(&jogo) && lerDirecao(&dl, &dc, 1))
            {
                /* tecla segurada: anda de novo assim que o passo acabar */
                mover(&jogo, dl, dc);
            }

            if (!animando(&jogo) && faseVencida(&jogo))
            {
                jogo.movimentosTotal += jogo.movimentosFase;
                jogo.movimentosFase = 0;
                if (jogo.fase == TOTAL_FASES - 1)
                {
                    nome[0] = '\0';
                    estado = DIGITANDO_NOME;
                }
                else
                {
                    estado = FASE_CONCLUIDA;
                }
            }
            break;

        case FASE_CONCLUIDA:
            if (IsKeyPressed(KEY_ENTER))
            {
                if (carregarFase(&jogo, jogo.fase + 1))
                    estado = JOGANDO;
                else
                    rodando = 0;
            }
            break;

        case DIGITANDO_NOME:
            lerNome(nome, MAX_NOME);
            if (IsKeyPressed(KEY_ENTER))
            {
                if (nome[0] == '\0')
                    strcpy(nome, "Anonimo");

                ranking = carregarRanking(ARQUIVO_RANKING);
                ranking = inserirPontuacao(ranking, nome, jogo.movimentosTotal);
                salvarRanking(ranking, ARQUIVO_RANKING);
                estado = MOSTRANDO_RANKING;
            }
            break;

        case MOSTRANDO_RANKING:
            if (IsKeyPressed(KEY_ENTER))
            {
                liberarRanking(ranking);
                ranking = NULL;
                if (novaPartida(&jogo))
                    estado = JOGANDO;
                else
                    rodando = 0;
            }
            break;
        }
        if (!rodando)
            break;

        BeginDrawing();
        if (estado == JOGANDO)
            desenharJogo(&jogo);
        else if (estado == FASE_CONCLUIDA)
            desenharFaseConcluida(&jogo);
        else if (estado == DIGITANDO_NOME)
            desenharDigitandoNome(&jogo, nome);
        else
            desenharRanking(ranking, jogo.movimentosTotal);
        EndDrawing();
    }

    liberarRanking(ranking);
    liberarJogo(&jogo);
    CloseWindow();
    return 0;
}