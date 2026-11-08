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

O passo a passo completo para **Windows, Linux e macOS** (instalação das ferramentas, como clonar com o submódulo da raylib e como compilar) está em **[COMPILAR.md](COMPILAR.md)**.

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
