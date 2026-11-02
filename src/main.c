/* =====================================================================
   src/main.c (versão de teste da etapa 3)

   Só confere se o gcc e a raylib funcionam juntos: abre uma janela e
   escreve um texto. Na etapa 8 este arquivo é substituído pelo main.c
   do jogo, que usa exatamente a mesma estrutura:
   abrir janela -> laço (ler teclado, atualizar, desenhar) -> fechar.
   ===================================================================== */

#include "raylib.h"   /* funções da raylib: janela, desenho, teclado */

int main(void)
{
    /* Cria a janela: largura 800 e altura 600 (em pixels), e o texto
       que aparece na barra de título. */
    InitWindow(800, 600, "Push Block - teste");

    /* Limita o jogo a 60 quadros por segundo. Sem isso o laço abaixo
       rodaria o mais rápido possível e usaria 100% do processador. */
    SetTargetFPS(60);

    /* Laço principal (game loop). Cada volta desenha um quadro (frame)
       da tela; a 60 FPS são 60 voltas por segundo.
       WindowShouldClose() devolve verdadeiro quando o usuário aperta
       ESC ou clica no X da janela; aí o laço termina. */
    while (!WindowShouldClose()) {

        /* Começa o desenho deste quadro. Tudo que for desenhado entre
           BeginDrawing() e EndDrawing() aparece junto na tela. */
        BeginDrawing();

        /* Pinta a janela inteira de cinza escuro, apagando o quadro
           anterior. Sem isso, os desenhos se acumulariam. */
        ClearBackground(DARKGRAY);

        /* DrawText(texto, x, y, tamanho da fonte, cor).
           x e y são a posição em pixels, contando do canto superior
           esquerdo da janela (0, 0); y cresce para baixo. */
        DrawText("A raylib esta funcionando!", 190, 270, 30, RAYWHITE);
        DrawText("Aperte ESC para fechar", 280, 320, 20, LIGHTGRAY);

        /* Termina o quadro e mostra ele na tela. Aqui também a raylib
           espera o tempo necessário para manter os 60 FPS. */
        EndDrawing();
    }

    /* Fecha a janela e libera os recursos que a raylib usou. */
    CloseWindow();

    return 0;   /* 0 = o programa terminou sem erro */
}