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

### Funcionalidades

- **3 fases** com dificuldade crescente, carregadas de arquivos de texto.
- **Desfazer jogadas** (`U`), quantas vezes quiser, até o início da fase.
- **Reiniciar a fase** (`R`) a qualquer momento.
- **Animação** do jogador e das caixas deslizando de uma casa para a outra.
- **Ranking (top scores)** salvo em arquivo: vence quem terminar as 3 fases com menos movimentos.

---

## 🕹️ Controles

<div align="center">

| Tecla | Ação                     |
|:-----:|--------------------------|
| `W` / `↑` | Mover para cima      |
| `A` / `←` | Mover para esquerda  |
| `S` / `↓` | Mover para baixo     |
| `D` / `→` | Mover para direita   |
| `U`   | Desfazer a última jogada |
| `R`   | Reiniciar a fase         |
| `ESC` | Sair do jogo             |

</div>


---

## 🛠️ Como compilar e jogar

### 1. Requisitos

Você precisa de `git`, `gcc` e `make`:

| Sistema | Como instalar |
|---|---|
| **Windows** | [w64devkit](https://github.com/skeeto/w64devkit/releases) extraído em `C:\w64devkit`, com `C:\w64devkit\bin` no PATH |
| **Linux** (Ubuntu/Debian) | `sudo apt install build-essential git libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev` |
| **macOS** | `xcode-select --install` |

Para conferir, `gcc --version` e `make --version` precisam mostrar uma versão.

### 2. Baixar, compilar e jogar

```bash
git clone --recurse-submodules --shallow-submodules https://github.com/rafaellcosta-s/Projeto_PIF.git
cd Projeto_PIF
make run
```

Pronto: a janela do jogo abre. Na **primeira vez**, o `make` também compila a raylib (biblioteca usada para a janela e o desenho), por isso demora um pouco mais. Nas próximas, compila só o jogo.

> **Já clonou sem `--recurse-submodules`?** A pasta `libs/raylib` fica vazia. Rode `git submodule update --init --depth 1` e depois `make run`.

### 3. Comandos do Makefile

| Comando | O que faz |
|---|---|
| `make run` | Compila (se precisar) e abre o jogo |
| `make` | Só compila: gera `build/pushblock` (`build/pushblock.exe` no Windows) |
| `make clean` | Apaga o executável |

No VS Code, `Ctrl+Shift+B` faz o mesmo que `make run`.

O passo a passo detalhado de cada sistema (instalação do w64devkit, PATH, WSL) está em **[COMPILAR.md](COMPILAR.md)**.
---

## 🧰 Tecnologias

- **Linguagem C** (padrão C11), compilada com `gcc` e `make`
- **[raylib](https://github.com/raysan5/raylib) 6.2** para janela, teclado e desenho, incluída como submódulo Git
- Funciona em **Windows, Linux e macOS**

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
