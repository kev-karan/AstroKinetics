# AstroKinetics

Um jogo arcade de tiro espacial "Casual Game" desenvolvido inteiramente em **C** utilizando a biblioteca **Raylib**. O jogo apresenta progressão de níveis, batalhas contra chefes, sistema de partículas e uma tabela de líderes!

Este projeto foi desenvolvido como requisito de avaliação para a disciplina de **Programação Imperativa e Funcional** da **CESAR School**.

## Funcionalidades
- **Física e Inércia:** Movimentação fluida da nave com rotação de 360º e sistema de inércia espacial.
- **Progressão Infinita:** Níveis procedurais onde os asteroides aumentam de velocidade e se multiplicam.
- **Inimigos Dinâmicos:** UFOs que perseguem e disparam contra o jogador.
- **Boss Fights:** A cada 5 Waves, enfrente um Chefe com mecânicas únicas.
- **Juice & Efeitos Visuais:** Fundo de estrelas com efeito *Warp Jump* dinâmico e sistema de partículas para explosões.
- **Top 5 High Scores:** Sistema de recordes no estilo clássico de fliperama, permitindo registar as suas 3 iniciais e salvando diretamente num ficheiro de texto.

## Arquitetura
A arquitetura do código foi pensada para demonstrar o domínio dos seguintes conceitos em linguagem C:
* **Estruturas (Structs) e Matrizes:** Gestão de entidades do jogo e renderização em múltiplas camadas (Parallax Starfield).
* **Ponteiros e Passagem por Referência:** Comunicação eficiente entre os módulos lógicos e gráficos.
* **Alocação Dinâmica de Memória:** Criação dinâmica (`malloc`) e destruição segura (`free`) de projéteis em tempo real.
* **Listas Encadeadas (Linked Lists):** Gestão otimizada do fluxo de disparos na tela sem memory leaks.
* **Leitura e Escrita em Ficheiro:** Persistência de dados para manter a Tabela de Recordes atualizada.

## Pré-requisitos
Para compilar e jogar o **AstroKinetics**, precisará ter instalado no seu ambiente Linux, MacOS ou WSL:
- Compilador GCC.
- Ferramenta GNU Make.
- Biblioteca [Raylib](https://www.raylib.com/) configurada.

## Como Compilar e Jogar
O projeto inclui um `Makefile` universal multiplataforma (suporta Linux, WSL, MacOS e Windows via MinGW).

1. Clone o repositório:
```bash
  git clone https://github.com/kev-karan/AstroKinetics.git
```
2. Entre no arquivo baixado
```bash
  cd AstroKinetics
```
3. Compile e execute automaticamente usando o comando:
```bash
  make run
```
*(Nota: Caso pretenda apenas compilar o executável sem rodar o jogo de imediato, utilize apenas o comando `make`)*

## Controles
- **SETAS (Esq/Dir):** Rotacionar a nave
- **SETA (Cima):** Acelerar os propulsores
- **ESPAÇO:** Disparar Laser
- **ENTER:** Confirmar opções de menu / Pausar o jogo / Inserir iniciais na Tabela de Recordes

## Licença e Autoria
Este projeto é licenciado sob a **MIT License**. Copyright (c) 2026 Kelwin Karan. 
Sinta-se livre para jogar, estudar, modificar e contribuir!
