# 🛠️ Como compilar e jogar

O jogo é escrito em C e usa a biblioteca [raylib](https://github.com/raysan5/raylib) (versão 6.2) para abrir a janela e desenhar. A raylib está no projeto como **submódulo Git** em `libs/raylib`: na primeira compilação, o Makefile compila a raylib junto (só nessa vez) e depois o jogo.

## Requisitos

- `git`, `gcc` e `make`
- **Windows:** [w64devkit](#windows-passo-a-passo)
- **Linux:** [pacotes de desenvolvimento](#linux-passo-a-passo)
- **macOS:** [Xcode Command Line Tools](#macos-passo-a-passo)

## Windows: passo a passo

O Makefile usa comandos como `mkdir -p` e `rm -f`, que o Windows não tem. O **w64devkit** traz o `gcc`, o `make` e essas ferramentas, então ele é **obrigatório** no Windows (o `mingw32-make` de outras instalações do MinGW não serve).

1. Baixe o `w64devkit-x64-...exe` em [github.com/skeeto/w64devkit/releases](https://github.com/skeeto/w64devkit/releases) e extraia em `C:\w64devkit`.
2. Adicione `C:\w64devkit\bin` ao PATH. No PowerShell:

   ```powershell
   [Environment]::SetEnvironmentVariable("Path", "C:\w64devkit\bin;" + [Environment]::GetEnvironmentVariable("Path", "User"), "User")
   ```

   O comando coloca a pasta no **início** do PATH, na frente de qualquer outro `gcc` já instalado.
3. **Feche e abra o PowerShell de novo** (o PATH novo só vale em janelas abertas depois da mudança).
4. Confira se deu certo:

   ```powershell
   gcc --version     # deve mostrar "gcc.exe (GCC) ..."
   make --version
   ```

> Se preferir não mexer no PATH, abra o `w64devkit.exe` (um terminal pronto que vem na pasta) e rode os comandos abaixo dentro dele.

## Linux: passo a passo

No Linux, a raylib é compilada a partir do código-fonte e precisa dos arquivos de desenvolvimento do sistema de janelas (X11) e do OpenGL.

1. Instale o compilador e as bibliotecas. No **Ubuntu/Debian**:

   ```bash
   sudo apt update
   sudo apt install build-essential git libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
   ```

   - `build-essential` traz o `gcc` e o `make`
   - os pacotes `libx...-dev` são o sistema de janelas e de teclado/mouse
   - `libgl1-mesa-dev` é o OpenGL, usado para desenhar

   Em outras distribuições, instale os pacotes equivalentes (X11, Xrandr, Xinerama, Xcursor, Xi e OpenGL com os arquivos de desenvolvimento).

2. Confira se deu certo:

   ```bash
   gcc --version
   make --version
   ```

3. Siga para [Clonar o repositório](#clonar-o-repositório) e [Compilar e rodar](#compilar-e-rodar).

> **WSL (Windows):** no Windows 11, o WSL com Ubuntu também funciona: rode os mesmos comandos dentro do terminal do Ubuntu e a janela do jogo abre no Windows (pelo WSLg).

## macOS: passo a passo

1. Instale as ferramentas de linha de comando da Apple (traz `git`, `make` e o compilador):

   ```bash
   xcode-select --install
   ```

   Vai abrir uma janela pedindo para instalar: clique em **Instalar** e espere terminar.

2. Confira se deu certo:

   ```bash
   gcc --version
   make --version
   ```

   No macOS, o comando `gcc` na verdade chama o **clang** (o compilador da Apple), então aparecer "Apple clang" é normal e funciona igual.

3. Siga para [Clonar o repositório](#clonar-o-repositório) e [Compilar e rodar](#compilar-e-rodar).

## Clonar o repositório

O `git clone` comum **não baixa submódulos**, por isso use as opções abaixo:

```bash
git clone --recurse-submodules --shallow-submodules https://github.com/rafaellcosta-s/Projeto_PIF.git
cd Projeto_PIF
```

Se você já clonou sem elas (a pasta `libs/raylib` fica vazia), baixe a raylib com:

```bash
git submodule update --init --depth 1
```

Ao dar `git pull` depois de alguma atualização da raylib, rode esse mesmo comando de novo.

## Compilar e rodar

```bash
make run     # compila a raylib (só na primeira vez) e o jogo, e abre a janela
make         # só compila: gera build/pushblock (build/pushblock.exe no Windows)
make clean   # apaga o executável
```

> A primeira compilação demora um pouco mais por causa da raylib; nas próximas, só o jogo é recompilado.
> No VS Code, `Ctrl+Shift+B` faz o mesmo que `make run`.

---

[← Voltar para o README](README.md)
