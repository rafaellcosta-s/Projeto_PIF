# =====================================================================
# Push Block - Makefile
#
# O Makefile é a "receita" de compilação do projeto. Em vez de digitar
# o comando gcc enorme toda vez, cada um do grupo só roda:
#
#   make        -> compila o jogo (gera o executável na pasta build/)
#   make run    -> compila (se precisar) e abre o jogo
#   make clean  -> apaga o executável, para compilar do zero
#
# Tudo que começa com # é comentário e o make ignora.
# =====================================================================


# Normalmente cada linha de comando de uma regra precisa começar com
# TAB. Aqui trocamos o TAB pelo caractere ">", porque ao copiar e colar
# o TAB costuma virar espaço e o make dá o erro "missing separator".
.RECIPEPREFIX = >


# ---------------------------------------------------------------------
# Variáveis: nomes curtos para não repetir texto ao longo do arquivo.
# Depois são usadas assim: $(CC), $(CFLAGS), $(SRC)...
# ---------------------------------------------------------------------

# Compilador usado (o gcc que veio com o w64devkit).
CC = gcc

# Opções passadas ao gcc:
#   -std=c11   usa o padrão C11 da linguagem C
#   -Wall      liga os avisos mais comuns (ajuda a achar erros)
#   -Wextra    liga avisos extras
#   -Iinclude  diz ao gcc para procurar os nossos .h na pasta include/
CFLAGS = -std=c11 -Wall -Wextra -Iinclude

# Lista de arquivos .c do jogo. O wildcard pega sozinho todos os .c da
# pasta src/, então criar um .c novo não exige mexer no Makefile.
SRC = $(wildcard src/*.c)


# ---------------------------------------------------------------------
# Configuração por sistema operacional. O Windows define a variável
# OS = Windows_NT; se não for Windows, o "uname -s" diz se é Linux ou
# macOS (Darwin). Cada sistema guarda a raylib num lugar diferente.
# ---------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    # Windows: o executável termina em .exe
    BIN = build/pushblock.exe
    # A raylib está dentro do próprio projeto, em libs/raylib.
    # -I aponta onde está o raylib.h
    CFLAGS += -Ilibs/raylib/include
    # -L aponta onde está o libraylib.a e cada -l liga uma biblioteca:
    #   raylib   a biblioteca do jogo (janela, desenho, teclado)
    #   opengl32 desenho na placa de vídeo (a raylib usa por baixo)
    #   gdi32    criação da janela no Windows
    #   winmm    funções de tempo do Windows (controle de FPS)
    LIBS = -Llibs/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
else
    UNAME = $(shell uname -s)
    BIN = build/pushblock
    ifeq ($(UNAME),Darwin)
        # macOS: raylib instalada com "brew install raylib";
        # o pkg-config descobre sozinho onde ela foi instalada.
        CFLAGS += $(shell pkg-config --cflags raylib)
        LIBS = $(shell pkg-config --libs raylib) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        # Linux: raylib instalada no sistema; as outras bibliotecas
        # são as de vídeo (GL, X11), matemática (m) e threads.
        LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif


# ---------------------------------------------------------------------
# Regras. Formato de cada uma:
#
#   alvo: dependências
#   > comando
#
# O make só executa o comando se o alvo não existir ou se alguma
# dependência foi modificada depois dele. Por isso, rodar "make" duas
# vezes seguidas sem mudar nada não recompila.
# ---------------------------------------------------------------------

# "all" é a regra padrão: é o que roda quando se digita só "make".
# Ela depende do executável, então manda gerar o executável.
all: $(BIN)

# Como gerar o executável: ele depende de todos os .c e .h, então
# qualquer arquivo alterado faz o jogo ser compilado de novo.
#   mkdir -p build   cria a pasta build/ (se já existir, não dá erro)
#   $(CC) ...        o comando gcc completo, montado com as variáveis:
#                    gcc [opções] [todos os .c] -o [executável] [bibliotecas]
$(BIN): $(SRC) $(wildcard include/*.h)
> mkdir -p build
> $(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LIBS)

# "make run": garante que o executável está atualizado e executa ele.
run: $(BIN)
> ./$(BIN)

# "make clean": apaga o executável (rm -f não reclama se ele não existir).
clean:
> rm -f build/pushblock build/pushblock.exe

# Avisa ao make que all, run e clean são nomes de comandos, e não de
# arquivos. Sem isso, um arquivo chamado "clean" na pasta faria o
# "make clean" achar que não tem nada a fazer.
.PHONY: all run clean