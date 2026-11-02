# 📦 Sokoban - Push Block

Jogo casual de quebra-cabeça desenvolvido em **C** para a disciplina de **PIF — Programação Imperativa Funcional**.

---

## 🎮 Sobre o jogo

**Sokoban** é um clássico jogo de lógica. O jogador controla um personagem dentro de um armazém e precisa empurrar todas as caixas até os locais marcados como destino.

### Regras

- O jogador pode se mover para cima, baixo, esquerda e direita.
- O jogador **só pode empurrar** uma caixa por vez (nunca puxar).
- Uma caixa não pode ser empurrada contra uma parede ou contra outra caixa.
- A fase termina quando **todas as caixas** estiverem sobre os destinos.

---

## 🕹️ Controles

<div align="center">
  
| Tecla | Ação                |
|:-----:|---------------------|
| `W` / `↑` | Mover para cima     |
| `A` / `←` | Mover para esquerda |
| `S` / `↓` | Mover para baixo    |
| `D` / `→` | Mover para direita  |
| `U`   | Desfazer a última jogada |
| `R`   | Reiniciar a fase    |
| `ESC` | Sair do jogo        |

</div>

---

## 🛠️ Como compilar e jogar

O jogo é escrito em C e usa a biblioteca [raylib](https://github.com/raysan5/raylib) (versão 6.2) para abrir a janela e desenhar. A raylib está no projeto como **submódulo Git** em `libs/raylib`: na primeira compilação, o Makefile compila a raylib junto (só nessa vez) e depois o jogo.

### Requisitos

- `git`, `gcc` e `make`
- **Windows:** [w64devkit](https://github.com/skeeto/w64devkit/releases) com a pasta `bin` no PATH
- **Linux (Ubuntu/Debian):** `sudo apt install build-essential git libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`
- **macOS:** `xcode-select --install`

### Clonar o repositório

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

### Compilar e rodar

```bash
make run     # compila a raylib (só na primeira vez) e o jogo, e abre a janela
make         # só compila: gera build/pushblock (build/pushblock.exe no Windows)
make clean   # apaga o executável
```

> A primeira compilação demora um pouco mais por causa da raylib; nas próximas, só o jogo é recompilado.
> No VS Code, `Ctrl+Shift+B` faz o mesmo que `make run`.

---

## 👥 Integrantes do Projeto

<div align="center">

|Membro | E-mail |
| :---: | :---: |
|Felipe Tavares de Oliveira Tenório Borborema Henriques | **ftotbh@cesar.school** |
|Lucas Cabral Lapa | **lcl4@cesar.school** |
|Rafael Costa e Silva | **rcs9@cesar.school** |

</div>

---

### 📄 Licença

Projeto acadêmico, desenvolvido para fins educacionais ©. 
