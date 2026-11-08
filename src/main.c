/* =====================================================================
   src/main.c

   Ponto de entrada do jogo. Abre a janela e roda o laço principal
   (game loop), que a cada quadro faz três coisas:
     1. lê o teclado;
     2. atualiza o estado do jogo (regras, animação, troca de tela);
     3. desenha a tela atual.
   ===================================================================== */

#include <string.h>    /* strlen, strcpy */
#include "raylib.h"
#include "jogo.h"
#include "ranking.h"
#include "tela.h"

/* Estado: em qual tela o jogo está. Um enum dá nomes a números
   inteiros; sem valores escritos à mão, ele numera sozinho:
   JOGANDO vale 0, FASE_CONCLUIDA vale 1, e assim por diante.
   O typedef cria o apelido Estado para não escrever "enum Estado". */
typedef enum {
    JOGANDO,            /* partida em andamento */
    FASE_CONCLUIDA,     /* aviso de fase vencida, esperando ENTER */
    DIGITANDO_NOME,     /* venceu todas as fases, digitando o nome */
    MOSTRANDO_RANKING   /* tela com as melhores pontuações */
} Estado;

/* Tecla apertada agora OU segurada (a raylib repete o aviso enquanto
   a tecla fica pressionada). Usada no desfazer e no apagar do nome. */
static int apertou(int tecla)
{
    return IsKeyPressed(tecla) || IsKeyPressedRepeat(tecla);
}

/* Verifica uma direção, aceitando duas teclas (letra e seta).
   segurando = 0: só conta se a tecla acabou de ser apertada;
   segurando = 1: conta enquanto a tecla estiver pressionada. */
static int tecla(int a, int b, int segurando)
{
    if (segurando)
        return IsKeyDown(a) || IsKeyDown(b);
    return IsKeyPressed(a) || IsKeyPressed(b);
}

/* Converte as teclas de direção em deslocamento (dl, dc). Os valores
   voltam pelos ponteiros, porque uma função só devolve um valor pelo
   return. Devolve 1 se alguma direção foi apertada e 0 se nenhuma. */
static int lerDirecao(int *dl, int *dc, int segurando)
{
    *dl = 0;
    *dc = 0;
    if (tecla(KEY_W, KEY_UP, segurando))    { *dl = -1; return 1; }
    if (tecla(KEY_S, KEY_DOWN, segurando))  { *dl = 1;  return 1; }
    if (tecla(KEY_A, KEY_LEFT, segurando))  { *dc = -1; return 1; }
    if (tecla(KEY_D, KEY_RIGHT, segurando)) { *dc = 1;  return 1; }
    return 0;
}

/* Lê as letras digitadas para o nome do jogador. GetCharPressed
   devolve um caractere por vez da fila de teclas digitadas neste
   quadro, e 0 quando a fila acaba. Só aceita caracteres visíveis
   (32 a 126 na tabela ASCII) e respeita o tamanho máximo. */
static void lerNome(char *nome, int max)
{
    int n = (int)strlen(nome);
    int c = GetCharPressed();

    while (c > 0) {
        if (c >= 32 && c < 127 && n < max - 1) {
            nome[n++] = (char)c;
            nome[n] = '\0';
        }
        c = GetCharPressed();
    }
    if (apertou(KEY_BACKSPACE) && n > 0)
        nome[--n] = '\0';
}

/* Começa uma partida nova desde a fase 1. */
static int novaPartida(Jogo *j)
{
    j->movimentosTotal = 0;
    return carregarFase(j, 0);
}

int main(void)
{
    Jogo jogo = {0};              /* {0} zera todos os campos */
    Pontuacao *ranking = NULL;    /* lista vazia */
    Estado estado = JOGANDO;
    char nome[MAX_NOME] = "";
    int dl, dc;
    int rodando = 1;

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Push Block");
    SetTargetFPS(60);                    /* 60 quadros por segundo */

    /* As fases são procuradas a partir da pasta onde o jogo foi aberto.
       Se o executável foi aberto direto de dentro de build/, sobe para
       a raiz do projeto, onde estão fases/ e dados/. */
    if (!FileExists("fases/fase1.txt")) {
        ChangeDirectory(GetApplicationDirectory());
        ChangeDirectory("..");
    }

    if (!novaPartida(&jogo)) {
        /* Sem fase não tem jogo: mostra o erro até fechar a janela */
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Nao achei fases/fase1.txt", 40, 260, 30, RED);
            DrawText("Rode o jogo a partir da raiz do projeto.", 40, 310, 20, RAYWHITE);
            EndDrawing();
        }
        CloseWindow();
        return 1;
    }

    /* Laço principal: cada volta é um quadro (60 por segundo).
       WindowShouldClose() fica verdadeiro com ESC ou com o X da janela. */
    while (rodando && !WindowShouldClose()) {

        /* ---------- 1 e 2: ler teclado e atualizar o estado ---------- */
        switch (estado) {
        case JOGANDO:
            /* avança a animação conforme o tempo do último quadro */
            atualizarAnimacao(&jogo, GetFrameTime());

            if (IsKeyPressed(KEY_R)) {
                carregarFase(&jogo, jogo.fase);      /* reinicia a fase */
            } else if (apertou(KEY_U)) {
                desfazer(&jogo);
            } else if (!faseVencida(&jogo) && lerDirecao(&dl, &dc, 0)) {
                /* tecla acabou de ser apertada: se ainda estava animando
                   o passo anterior, termina ele na hora e já anda de novo,
                   para nenhum toque rápido ser perdido */
                jogo.anim.progresso = 1.0f;
                mover(&jogo, dl, dc);
            } else if (!animando(&jogo) && !faseVencida(&jogo) && lerDirecao(&dl, &dc, 1)) {
                /* tecla segurada: anda de novo assim que o passo acabar */
                mover(&jogo, dl, dc);
            }

            /* só confere a vitória depois que a última caixa terminou
               de deslizar, para o jogador ver ela chegando no destino */
            if (!animando(&jogo) && faseVencida(&jogo)) {
                jogo.movimentosTotal += jogo.movimentosFase;
                jogo.movimentosFase = 0;
                if (jogo.fase == TOTAL_FASES - 1) {
                    nome[0] = '\0';
                    estado = DIGITANDO_NOME;         /* venceu o jogo */
                } else {
                    estado = FASE_CONCLUIDA;         /* venceu a fase */
                }
            }
            break;

        case FASE_CONCLUIDA:
            if (IsKeyPressed(KEY_ENTER)) {
                if (carregarFase(&jogo, jogo.fase + 1))
                    estado = JOGANDO;
                else
                    rodando = 0;                     /* fase faltando */
            }
            break;

        case DIGITANDO_NOME:
            lerNome(nome, MAX_NOME);
            if (IsKeyPressed(KEY_ENTER)) {
                if (nome[0] == '\0')
                    strcpy(nome, "Anonimo");
                /* lê o arquivo, insere o resultado e grava de novo */
                ranking = carregarRanking(ARQUIVO_RANKING);
                ranking = inserirPontuacao(ranking, nome, jogo.movimentosTotal);
                salvarRanking(ranking, ARQUIVO_RANKING);
                estado = MOSTRANDO_RANKING;
            }
            break;

        case MOSTRANDO_RANKING:
            if (IsKeyPressed(KEY_ENTER)) {
                liberarRanking(ranking);             /* libera a lista */
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

        /* ---------------------- 3: desenhar ---------------------- */
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

    /* libera toda a memória alocada antes de sair */
    liberarRanking(ranking);
    liberarJogo(&jogo);
    CloseWindow();
    return 0;
}