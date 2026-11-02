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
# A raylib entra no projeto como submódulo Git, na pasta libs/raylib:
# é o código-fonte original da raylib (versão 6.2). Na primeira vez que
# se roda "make", o Makefile compila a raylib e gera o libraylib.a;
# nas próximas vezes ele já existe e o make pula essa parte.
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

# Pasta do código-fonte da raylib dentro do submódulo. É lá que estão
# o raylib.h e onde o libraylib.a é gerado.
RAYLIB_DIR = libs/raylib/src

# A biblioteca compilada da raylib (criada pela regra mais abaixo).
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

# Opções passadas ao gcc:
#   -std=c11          usa o padrão C11 da linguagem C
#   -Wall             liga os avisos mais comuns (ajuda a achar erros)
#   -Wextra           liga avisos extras
#   -Iinclude         procura os nossos .h na pasta include/
#   -I$(RAYLIB_DIR)   procura o raylib.h dentro do submódulo
CFLAGS = -std=c11 -Wall -Wextra -Iinclude -I$(RAYLIB_DIR)

# Lista de arquivos .c do jogo. O wildcard pega sozinho todos os .c da
# pasta src/, então criar um .c novo não exige mexer no Makefile.
SRC = $(wildcard src/*.c)


# ---------------------------------------------------------------------
# Configuração por sistema operacional. O Windows define a variável
# OS = Windows_NT; se não for Windows, o "uname -s" diz se é Linux ou
# macOS (Darwin). Em todos, -L aponta onde está o libraylib.a e cada
# -l liga uma biblioteca; o que muda são as bibliotecas do sistema
# que a raylib usa por baixo para criar a janela e desenhar.
# ---------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    # Windows: o executável termina em .exe
    #   opengl32  desenho na placa de vídeo
    #   gdi32     criação da janela no Windows
    #   winmm     funções de tempo do Windows (controle de FPS)
    BIN = build/pushblock.exe
    LIBS = -L$(RAYLIB_DIR) -lraylib -lopengl32 -lgdi32 -lwinmm
else
    UNAME = $(shell uname -s)
    BIN = build/pushblock
    ifeq ($(UNAME),Darwin)
        # macOS: bibliotecas de vídeo e janela da Apple (frameworks)
        LIBS = -L$(RAYLIB_DIR) -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    else
        # Linux: vídeo (GL, X11), matemática (m) e threads
        LIBS = -L$(RAYLIB_DIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
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

# Como gerar o libraylib.a: entra na pasta da raylib (-C) e roda o
# Makefile que vem com ela, pedindo a versão para computador
# (PLATFORM_DESKTOP). $(MAKE) é o próprio make que está rodando.
# Só acontece uma vez, porque depois o arquivo já existe.
$(RAYLIB_LIB):
> $(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP

# Como gerar o executável: ele depende dos nossos .c e .h e da raylib
# compilada, então qualquer arquivo alterado faz o jogo ser compilado
# de novo, e a raylib é compilada antes se ainda não existir.
#   mkdir -p build   cria a pasta build/ (se já existir, não dá erro)
#   $(CC) ...        o comando gcc completo, montado com as variáveis:
#                    gcc [opções] [todos os .c] -o [executável] [bibliotecas]
$(BIN): $(SRC) $(wildcard include/*.h) $(RAYLIB_LIB)
> mkdir -p build
> $(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LIBS)

# "make run": garante que o executável está atualizado e executa ele.
run: $(BIN)
> ./$(BIN)

# "make clean": apaga o executável (rm -f não reclama se ele não existir).
# A raylib compilada fica, para não precisar compilar ela de novo.
clean:
> rm -f build/pushblock build/pushblock.exe

# Avisa ao make que all, run e clean são nomes de comandos, e não de
# arquivos. Sem isso, um arquivo chamado "clean" na pasta faria o
# "make clean" achar que não tem nada a fazer.
.PHONY: all run clean