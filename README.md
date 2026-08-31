# AstroKinetics

AstroKinetics é um shooter de arcade 2D compacto implementado em C utilizando Raylib. Ele combina um modelo de controle de espaçonave top-down, progressão determinística baseada em ondas, física de asteróides, IA de inimigos, encontros com chefes, efeitos de partículas e rastreamento de pontuação persistente. A base de código é estruturada como um projeto de systems programming em nível didático, enfatizando modularidade, manipulação de memória de baixo nível, gerenciamento de estado de jogo e compilação multiplataforma.

O projeto foi desenvolvido como trabalho final da materia de Programação Imperativa e Funcional para demonstrar engenharia C prática em um contexto de jogo: estado de entidades via structs, propriedade direta de memória através de listas encadeadas, detecção de colisão em tempo real, persistência baseada em arquivo e uma clara separação entre lógica de gameplay e renderização. É adequado para revisão acadêmica, avaliação técnica e avaliação de desenvolvedores.

## 1. Visão Geral

AstroKinetics é um jogo de ação em tempo real no qual o jogador pilota uma nave através de ondas procedimentalmente geradas de asteróides e UFOs hostis. O jogo utiliza um campo espacial com wraparound e um modelo de voo baseado em impulso rotacional e inércia para criar um sistema de movimento responsivo mas com estética arcade. O objetivo principal é sobreviver e acumular pontos enquanto limpa campos de asteróides, evita fogo inimigo e derrota encontros com chefes acionados a cada cinco níveis.

Do ponto de vista de arquitetura de software, o projeto é deliberadamente simples mas tecnicamente rigoroso:

- É implementado como um único programa C com um header central definindo o modelo de dados do jogo.
- O estado do jogo é codificado usando structs ao invés de abstrações orientadas a objetos.
- O loop de execução é acionado pelo timing de frames do Raylib e entrada de teclado.
- Os sistemas de física e combate estão contidos em rotinas de lógica dedicadas.
- A renderização é isolada do gameplay, permitindo uma clara separação de responsabilidades.
- A persistência de pontuação máxima é implementada como I/O direto de arquivo para um arquivo de texto simples.

O código enfatiza práticas clássicas de engenharia em C: arrays de tamanho fixo para pools de entidades, alocação dinâmica para nós de projéteis, gerenciamento direto de ponteiros e limpeza explícita de recursos. Isto faz do AstroKinetics um forte exemplo de como a lógica de jogo pode ser modelada efetivamente em uma linguagem de baixo nível sem sacrificar legibilidade ou manutenibilidade.

## 2. Destaques de Arquitetura

### Modelo de dados central e gerenciamento de estado orientado por `struct`

As definições centrais do jogo residem em `src/game.h`, que consolida todas as constantes, tipos de entidade, estados de tela e protótipos de função. O projeto usa `struct`s como o mecanismo principal para gerenciar entidades de jogo e seu estado de execução.

Os modelos de entidades principais incluem:

- `Player`: armazena posição, velocidade, rotação, raio de colisão (`size`), cor, vidas restantes e `invulnerableTimer` para breve invencibilidade após dano.
- `Asteroid`: armazena posição, velocidade, raio, flag de atividade, offsets de vértices irregulares e parâmetros rotacionais. Isto é essencial para gerar geometria de asteróide e simular movimento angular.
- `Enemy`: representa oponentes UFO com posição, velocidade, raio, estado ativo, intervalo de disparo e comportamento controlado pela lógica de perseguição.
- `Boss`: inclui saúde, saúde máxima, timer de introdução, contagem de bounces, estado de travessia e estado de movimento. É um adversário com estado que faz transições entre fases de bounce vertical e travessia horizontal.
- `Particle`: armazena efeitos visuais transitórios com cor, velocidade, lifetime e flag ativa para explosões e efeitos de propulsor.
- `Bullet`: inclui posição de projétil, velocidade, lifetime, flag de inimigo e um ponteiro `next` utilizado na lista encadeada de gameplay.

Este design mantém o engine orientado a dados: cada entidade existe em um array fixo ou estrutura encadeada, e as funções de lógica operam nesses arrays e ponteiros diretamente. Este é um padrão clássico e eficiente para jogos baseados em C, especialmente quando atualizações de estado devem permanecer determinísticas e leves.

### Gerenciamento de projéteis com lista encadeada e propriedade de memória

O gerenciamento de projéteis em `src/logic.c` é implementado com uma lista encadeada simples. Cada nó `Bullet` é alocado com `malloc`, inserido no início da lista e eventualmente removido e liberado quando expira ou colide. Este design é importante porque permite que um número variável de projéteis seja disparado dinamicamente sem pré-definir um limite máximo rígido.

O padrão central é o seguinte:

- No disparo do jogador ou inimigo, um novo nó é criado com `malloc(sizeof(Bullet))`.
- O projétil é inicializado com posição, velocidade, lifetime e flags de propriedade.
- O nó é inserido na lista usando `newBullet->next = *bulletsHead; *bulletsHead = newBullet;`.
- A cada frame, a lista é percorrida e os projéteis são atualizados de acordo com sua velocidade e lifetime.
- Se um projétil colide com um asteróide, UFO ou chefe, ou se seu lifetime expira, ele é desvinculado da lista e `free()` é chamado.

Isto garante que o engine não sofra memory leaks enquanto dispara projéteis durante combate de alta intensidade. A mesma disciplina também é aplicada durante resets de jogo e transições de nível, onde os projéteis restantes são explicitamente desalocados antes da próxima fase começar. O resultado é um robusto ciclo de vida de memória que corresponde ao comportamento de systems programming esperado de C.

### Sistema de colisão e mecânica de bounce de Asteróide/Chefe

A detecção de colisão é primariamente baseada em sobreposição de círculos usando utilitários do Raylib como `CheckCollisionCircles`. Isto proporciona um método rápido e confiável para avaliar colisões entre entidades com raios nominais.

A lógica de interação de asteróide em `UpdateAsteroids` é fundamentada matematicamente em resposta similar a corpo rígido. Para cada par de asteróides que se sobrepõem:

- O vetor de deslocamento relativo é computado como `dx`, `dy`.
- A distância é normalizada para produzir um eixo de colisão, `nx` e `ny`.
- A magnitude de sobreposição é calculada como `sumRadii - distance`.
- Ambos os asteróides são separados por metade da sobreposição ao longo da normal para evitar interpenetração.
- A velocidade relativa ao longo da normal de colisão é computada com o produto escalar:

  `dotProduct = (dvx * nx) + (dvy * ny)`

- Se a velocidade está se movendo para dentro (`dotProduct < 0`), um impulso é aplicado a ambos os corpos baseado em suas massas efetivas, aproximadas por seus raios. Isto produz um comportamento de bounce consistente sem exigir um engine de física completo.

Isto mantém a simulação leve enquanto ainda fornece aos asteróides uma resposta convincente quando colidem um com o outro.

O encontro com chefe utiliza uma máquina de estados para implementar um padrão de movimento mais deliberado. Em `UpdateBoss`:

- O chefe inicia em uma fase de oscilação vertical onde `position.y` é limitada entre o teto e o piso da tela.
- Quando o chefe atinge o limite, `velocity.y` é invertida, produzindo um efeito de bounce.
- Após um número configurado de bounces (`bounceCount >= 5`), o chefe muda para `isCrossing`, que altera o eixo de movimento de travessia vertical para horizontal.
- O chefe então se move através da tela, limitado em `x = 100` ou `x = screenWidth - 100`, e retoma uma nova fase quando a travessia é concluída.

Esta abordagem cria comportamento de chefe previsível mas dinâmico e é um bom exemplo de lógica de jogo orientada por estado implementada sem abstração excessiva.

### Separação de responsabilidades entre módulos

O projeto demonstra uma clara separação entre orquestração de loop de jogo, lógica de gameplay e renderização.

- `src/main.c`: manipula o loop de aplicação principal, polling de entrada, transições de tela (`SPLASH`, `MENU`, `GAMEPLAY`, `NAME_ENTRY`, `TOP_SCORES`, `ENDING`), reprodução de áudio e ciclo de vida do engine. É a camada de orquestração.
- `src/logic.c`: contém todos os sistemas de gameplay: movimento de nave, spawning e atualizações de projéteis, comportamento de asteróide, IA de inimigos, IA de chefe, resolução de colisão, progressão de power-up e lógica de transição entre níveis. Esta camada possui o estado da simulação.
- `src/graphics.c`: é responsável por desenhar o starfield, entidades, efeitos, texto de interface, overlays, barra de saúde do chefe e UI específica de tela. Realiza apenas renderização, deixando as regras de simulação na camada de lógica.

Esta estrutura modular é importante para manutenibilidade: o comportamento de gameplay pode ser modificado sem tocar o código de renderização e vice-versa. O projeto é, portanto, não apenas um jogo mas um bom exemplo de design em camadas em C.

### Persistência de dados e sistema de Pontuação Máxima

O sistema de pontuação máxima é implementado em `src/utils.c` e depende de simples I/O de arquivo com `fopen`, `fscanf` e `fprintf`. O arquivo `topscores.txt` armazena cada entrada como um nome de três caracteres e uma pontuação inteira em formato de texto simples.

`LoadHighScores` tenta ler o arquivo existente e popular o array `HighScoreEntry`. Se o arquivo está faltando ou incompleto, ele recua para um conjunto padrão de pontuações sintéticas. Isto proporciona resiliência e garante que o jogo sempre tenha dados válidos de leaderboard.

`SaveHighScores` escreve os rankings atuais de volta ao disco em um formato consistente. Durante o gameplay, a tela de entrada de nome permite que o usuário defina três iniciais, após o que o leaderboard é deslocado e salvo. Isto cria um modelo clássico de persistência de pontuação arcade sem exigir um banco de dados ou dependência externa.

## 3. Instruções de Build

O projeto inclui um `Makefile` multiplataforma que suporta builds de desktop nativo e um alvo WebAssembly para implantação em navegador.

### Build de desktop nativo

A partir da raiz do projeto:

```bash
make
```

Isto compila todos os arquivos de origem de `src/` no alvo executável. O `Makefile` define `-Wall -std=c99 -I./src` e faz link contra Raylib com bibliotecas de sistema específicas da plataforma.

Comportamento da plataforma:

- Linux: faz link com `-lraylib -lGL -lm -lpthread -ldl -lrt -lX11`
- macOS: faz link com os frameworks requeridos para suporte OpenGL e Cocoa
- Windows: faz link com as bibliotecas Windows OpenGL e GDI

Para compilar e executar imediatamente o jogo:

```bash
make run
```

Isto chama o alvo de execução específico da plataforma, que executa o executável gerado. Para remover objetos compilados e artefatos binários:

```bash
make clean
```

### Build WebAssembly com Emscripten

O repositório também inclui um alvo `web` para implantação em navegador:

```bash
make web
```

Isto invoca `emcc` com os arquivos de origem do projeto e produz `index.html`, `index.js` e um módulo WebAssembly. O comando inclui:

- `-I./src -I./lib_web`
- `-L./lib_web -lraylib.web`
- `-DPLATFORM_WEB`
- `-s USE_GLFW=3 -s ASYNCIFY`
- `-s INITIAL_MEMORY=67108864`
- `-s ALLOW_MEMORY_GROWTH=1`
- `--preload-file assets`

Esta configuração é necessária porque o projeto depende de um build Raylib compatível com navegador e assets que devem ser agrupados com o site gerado. O alvo `web` é destinado para ambientes habilitados para Emscripten e é uma importante demonstração de portabilidade além de execução de desktop nativa.

### Pré-requisitos

Antes de compilar, certifique-se de que o seguinte está disponível:

- GCC ou um compilador C equivalente
- GNU Make
- Raylib instalado e detectável pelo compilador/linker
- Para o alvo `web`: Emscripten (`emcc`) em PATH, mais a configuração apropriada da biblioteca Raylib web

## 4. Estrutura de Diretórios

```text
AstroKinetics/
├── LICENSE
├── Makefile                     # Sistema de build multiplataforma para alvos desktop e web
├── README.md                    # Documentação do projeto
├── topscores.txt                # Dados de leaderboard persistente
├── lib_web/                     # Suporte de build web e bibliotecas Raylib web
├── src/
│   ├── game.h                  # Constantes centrais, structs, enums e declarações de função
│   ├── main.c                  # Loop de aplicação principal, máquina de estados, áudio e ciclo de vida
│   ├── logic.c                 # Simulação de gameplay: movimento, IA de inimigo, colisões, chefes, projéteis
│   ├── graphics.c              # Pipeline de renderização e desenho de UI
│   └── utils.c                 # Carregamento/salvamento de pontuação máxima e utilitários de reset
├── assets/                     # Assets de áudio e visuais utilizados pelo jogo
│   ├── audio/
│   └── images/
└── .gitignore
```

## Resumo

AstroKinetics é um projeto de jogo C tecnicamente coerente que demonstra como implementar uma experiência arcade polida utilizando uma linguagem de baixo nível e uma biblioteca de gráficos. Ele integra lógica de gameplay, persistência de arquivo, gerenciamento de memória dinâmica, matemática de colisão e renderização modular de uma maneira que é tanto educacional quanto voltada para produção. Para recrutadores técnicos e avaliadores acadêmicos, o repositório fornece um exemplo claro de design de API, práticas de segurança de memória, lógica de simulação e arquitetura de software estruturada.
