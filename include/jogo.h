/* =====================================================================
   include/jogo.h

   "Contrato" da parte de regras do jogo: os tipos que guardam o estado
   de uma partida e as funções que o jogo.c implementa. Quem quiser usar
   essas funções (main.c, tela.c) só precisa dar #include "jogo.h".

   Este arquivo não usa raylib: as regras não sabem nada de janela.
   ===================================================================== */

/* Proteção contra inclusão dupla: se JOGO_H já foi definido (o arquivo
   já foi incluído antes), o compilador pula tudo até o #endif. */
#ifndef JOGO_H
#define JOGO_H

/* Quantidade de fases (arquivos fases/fase1.txt até fases/faseN.txt). */
#define TOTAL_FASES 3

/* Tempo, em segundos, que o jogador leva para deslizar de uma casa para
   a outra. É o que faz o movimento aparecer animado na tela. */
#define DURACAO_PASSO 0.12f

/* ---------------------------------------------------------------------
   enum Celula: os símbolos que podem aparecer em cada casa do mapa.

   Um enum dá NOMES a valores inteiros. Aqui cada nome recebe o próprio
   caractere usado nos arquivos de fase (padrão do Sokoban), então o
   código pode escrever PAREDE em vez de '#', o que deixa as regras
   mais fáceis de ler, e o mapa continua sendo uma matriz de char.

   typedef cria um apelido: em vez de escrever "enum Celula" toda vez,
   basta escrever "Celula".
   --------------------------------------------------------------------- */
typedef enum Celula {
    PAREDE             = '#',
    CHAO               = ' ',
    DESTINO            = '.',
    CAIXA              = '$',
    CAIXA_NO_DESTINO   = '*',
    JOGADOR            = '@',
    JOGADOR_NO_DESTINO = '+'
} Celula;

/* ---------------------------------------------------------------------
   Jogada: um nó da lista encadeada do histórico.
   Cada movimento feito vira um nó novo, colocado no INÍCIO da lista.
   Assim o primeiro nó é sempre a jogada mais recente, e a lista funciona
   como uma pilha: para desfazer, basta tirar o primeiro nó.

   Dentro da struct ainda é preciso escrever "struct Jogada *", porque o
   apelido Jogada só passa a existir depois do fim do typedef.
   --------------------------------------------------------------------- */
typedef struct Jogada {
    int dl;                  /* direção em linhas: -1 cima, 1 baixo, 0 nada */
    int dc;                  /* direção em colunas: -1 esquerda, 1 direita, 0 nada */
    int empurrou;            /* 1 se essa jogada empurrou uma caixa */
    struct Jogada *proxima;  /* ponteiro para a jogada anterior (ou NULL) */
} Jogada;

/* ---------------------------------------------------------------------
   Animacao: guarda o último passo para a tela desenhar o jogador
   (e a caixa empurrada) deslizando, em vez de pular de casa em casa.
   progresso vai de 0 (acabou de sair da casa antiga) até 1 (chegou).
   --------------------------------------------------------------------- */
typedef struct {
    int dl;            /* direção do último passo (igual à da Jogada) */
    int dc;
    int empurrou;      /* 1 se uma caixa está deslizando junto */
    float progresso;   /* 0.0 a 1.0; quando chega em 1, a animação acabou */
} Animacao;

/* ---------------------------------------------------------------------
   Jogo: todo o estado de uma partida.

   O mapa é uma MATRIZ de caracteres alocada dinamicamente (char **):
   mapa é um vetor de ponteiros, um por linha, e cada linha é um vetor
   de char criado com malloc. Acessa-se como mapa[linha][coluna], e cada
   casa guarda um dos valores do enum Celula.
   --------------------------------------------------------------------- */
typedef struct {
    char **mapa;          /* matriz dinâmica: mapa[linha][coluna] */
    int linhas;           /* quantidade de linhas do mapa */
    int colunas;          /* quantidade de colunas do mapa */
    int jogadorL;         /* linha onde o jogador está */
    int jogadorC;         /* coluna onde o jogador está */
    int fase;             /* fase atual, começando em 0 */
    int movimentosFase;   /* passos dados na fase atual */
    int movimentosTotal;  /* passos somados das fases já vencidas */
    Jogada *historico;    /* início da lista encadeada de jogadas */
    Animacao anim;        /* animação do último passo */
} Jogo;

/* Lê fases/fase<N>.txt e monta o mapa. fase começa em 0.
   Devolve 1 se deu certo e 0 se o arquivo não existe ou é inválido. */
int carregarFase(Jogo *j, int fase);

/* Libera toda a memória da partida (mapa e histórico). */
void liberarJogo(Jogo *j);

/* Tenta mover o jogador na direção (dl, dc). Trata as colisões com
   parede e caixa. Devolve 1 se o jogador andou e 0 se foi bloqueado. */
int mover(Jogo *j, int dl, int dc);

/* Desfaz a última jogada. Devolve 1 se desfez e 0 se não havia jogada. */
int desfazer(Jogo *j);

/* Devolve 1 se não sobrou nenhuma caixa fora de um destino. */
int faseVencida(const Jogo *j);

/* Avança a animação do último passo. dt é o tempo, em segundos, desde
   o quadro anterior (a raylib informa com GetFrameTime()). */
void atualizarAnimacao(Jogo *j, float dt);

/* Devolve 1 enquanto o jogador ainda está deslizando entre duas casas. */
int animando(const Jogo *j);

#endif