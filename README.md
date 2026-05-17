# Sistemas Interativos - Algoritmos de Escalonamento

Coletânea de implementações de algoritmos clássicos de **escalonamento de processos** para sistemas interativos, em C, C++ e Java. O repositório agrupa sete famílias de algoritmos, algumas com mais de uma implementação, totalizando versões em C, C++ e Java com foco didático.

---

## Sumário

1. [Estrutura do repositório](#estrutura-do-repositório)
2. [Como compilar e executar](#como-compilar-e-executar)
3. [Algoritmos implementados](#algoritmos-implementados)
   - [SJF / SRTN](#1-sjf--srtn-shortest-remaining-time-next)
   - [Round Robin em C e C++](#2-round-robin-em-c-e-c)
   - [Priority Scheduling em C e C++](#3-priority-scheduling-em-c-e-c)
   - [Multilevel Queue](#4-multilevel-queue)
   - [Lottery Scheduling em C e Java](#5-lottery-scheduling-em-c-e-java)
   - [Fair Share Scheduling em C e Java](#6-fair-share-scheduling-em-c-e-java)
   - [Escalonamento Garantido em C e Java](#7-escalonamento-garantido-em-c-e-java)
4. [Comparativo dos algoritmos](#comparativo-dos-algoritmos)
5. [Correções aplicadas](#correções-aplicadas)
6. [Limitações conhecidas](#limitações-conhecidas)

---

## Estrutura do repositório

```
sistemasInterativos-master/
├── README.md
├── sjf/
│   ├── README.md
│   └── srtn.c                         # Shortest Remaining Time Next (SJF preemptivo)
├── round-robin_cpp/
│   ├── README.md
│   └── round_Robin.cpp                # Round Robin em C++
├── round_robin_c/
│   ├── README.md
│   └── round_robin.c                  # Round Robin em C
├── priority_cpp/
│   ├── README.md
│   └── priority.cpp                   # Priority Scheduling em C++
├── priority_c/
│   ├── README.md
│   └── priority.c                     # Priority Scheduling em C
├── multilevel/
│   ├── README.md
│   └── multiLevel.c                   # Multilevel Queue (RR + Priority + FCFS)
├── lottery_Java/
│   ├── README.md
│   └── Lottery.java                   # Lottery Scheduling em Java
├── lottery_c/
│   ├── README.md
│   └── lottery.c                      # Lottery Scheduling em C
├── fair-share_Java/
│   ├── README.md
│   └── src/
│       ├── scheduler/                 # Núcleo do escalonador Fair Share
│       │   ├── FSSLogger.java
│       │   ├── IdleProcess.java
│       │   ├── Process.java
│       │   ├── Processor.java
│       │   ├── ReadyQueue.java
│       │   └── ShareGroup.java
│       └── tester/
│           └── ScheduleTest.java      # Programa de teste
├── fair_share_c/
│   ├── README.md
│   └── fair_share.c                   # Fair Share Scheduler em C
├── garantido_Java/
│   ├── README.md
│   └── src/
│       ├── controller/                # MVC: lógica de escalonamento
│       ├── model/                     # MVC: entidades
│       └── view/                      # MVC: interface gráfica Swing
└── escalonamento_garantido_c/
    ├── README.md
    └── escalonamento_garantido.c      # Escalonamento Garantido em C
```

---

## Como compilar e executar

### Pré-requisitos

| Linguagem | Compilador / Runtime       | Versão mínima |
|-----------|----------------------------|---------------|
| C         | `gcc`                      | C11           |
| C++       | `g++`                      | C++17         |
| Java      | `javac` / `java` (OpenJDK) | 11+           |

### SJF / SRTN

```bash
cd sjf
gcc -std=c11 -Wall -Wextra -o srtn srtn.c
./srtn
```

Entrada: número de processos, tempos de chegada e durações.

### Round Robin em C++

```bash
cd round-robin_cpp
g++ -std=c++17 -Wall -Wextra -o round_robin round_Robin.cpp
./round_robin
```

Não requer entrada. Os processos estão pré-definidos no `main`.

### Round Robin em C

```bash
cd round_robin_c
gcc -std=c11 -Wall -Wextra -o round_robin round_robin.c
./round_robin
```

Gera `log.txt` com a saída da simulação.

### Priority Scheduling em C++

```bash
cd priority_cpp
g++ -std=c++17 -Wall -Wextra -o priority priority.cpp
./priority
```

### Priority Scheduling em C

```bash
cd priority_c
gcc -std=c11 -Wall -Wextra -o priority priority.c
./priority
```

Gera `log.txt` com a saída da simulação.

### Multilevel Queue

```bash
cd multilevel
gcc -std=c11 -Wall -Wextra -o multilevel multiLevel.c
./multilevel
```

Entrada: faixas de prioridade das três filas, quantidade de processos, prioridade e burst time de cada processo.

### Lottery Scheduling em Java

```bash
cd lottery_Java
javac -d . Lottery.java
java lottery.Lottery
```

### Lottery Scheduling em C

```bash
cd lottery_c
gcc -std=c11 -Wall -Wextra -o lottery lottery.c
./lottery
```

Também aceita seed fixa:

```bash
./lottery --seed 42
```

### Fair Share Scheduling em Java

```bash
cd fair-share_Java/src
javac -d ../build $(find . -name "*.java")
cd ../build
java tester.ScheduleTest
```

Gera `log.txt` com a evolução das taxas de uso de CPU por grupo.

### Fair Share Scheduling em C

```bash
cd fair_share_c
gcc -std=c11 -Wall -Wextra fair_share.c -lm -o fair_share
./fair_share --fss
```

Também é possível comparar com prioridade simples do sistema:

```bash
./fair_share --system
```

### Escalonamento Garantido em Java (interface gráfica Swing)

```bash
cd garantido_Java/src
javac -d ../build $(find . -name "*.java")
cd ../build
java view.TesteView
```

Requer ambiente gráfico (X11 / Wayland no Linux, Aqua no macOS, Desktop no Windows).

### Escalonamento Garantido em C

```bash
cd escalonamento_garantido_c
gcc -std=c11 -Wall -Wextra escalonamento_garantido.c -lm -o escalonamento_garantido
./escalonamento_garantido
```

Também é possível informar os surtos manualmente:

```bash
./escalonamento_garantido 10 4 7 13 6
```

---

## Algoritmos implementados

### 1. SJF / SRTN (Shortest Remaining Time Next)

Versão **preemptiva** do *Shortest Job First*. A cada unidade de tempo, o escalonador escolhe entre os processos que já chegaram aquele com o **menor tempo restante de execução**.

```
Tempo →   0   1   2   3   4   5   6   7   8   9  10
Pronto: [P1] [P1,P2] [P2] [P2] [P2] [P1] [P1] ...
            ↑ P2 chega com burst=4 < 7 restante de P1, preempção
```

**Vantagem**: ótimo para *waiting time* médio.
**Desvantagem**: *starvation* (processos longos podem esperar indefinidamente). Requer conhecer o burst time a priori (na prática, é estimado).

**Métricas calculadas**:
- Tempo de espera (waiting time)
- Tempo de retorno (turnaround time)
- Tempo de conclusão (completion time)

---

### 2. Round Robin em C e C++

Cada processo recebe a CPU por um **quantum** fixo. Quando o quantum expira, o processo volta para o fim da fila circular.

**Implementações disponíveis:** `round-robin_cpp/round_Robin.cpp` e `round_robin_c/round_robin.c`.

```
Quantum = 2
Fila: [P1, P2, P3, P1, P2, P3, P1, P2, P1, P1, P1, ...]

      ┌──────┐       ┌──────┐       ┌──────┐
P1 →  │ 2u   │ ───── │ 2u   │ ───── │ 2u   │ ─ ...
      └──────┘       └──────┘       └──────┘
         ↓ fim quantum  ↓ fim quantum
      ┌──────┐       ┌──────┐
P2 →  │ 2u   │ ───── │ 2u   │ ...
      └──────┘       └──────┘
```

**Vantagem**: justo, resposta rápida - ideal para sistemas interativos.
**Desvantagem**: *overhead* alto se o quantum for muito pequeno; comporta-se como FCFS se o quantum for muito grande.

---

### 3. Priority Scheduling em C e C++

Cada processo tem uma prioridade; o de **maior prioridade** executa primeiro. As implementações deste repositório são **não-preemptivas** e usam a convenção *"maior número = maior prioridade"*.

**Implementações disponíveis:** `priority_cpp/priority.cpp` e `priority_c/priority.c`.

```
Entrada (pid, burst, prioridade):
  (1, 10, 2)   ← prioridade 2
  (2,  5, 0)   ← prioridade 0
  (3,  8, 1)   ← prioridade 1

Ordem de execução: P1 → P3 → P2
Gantt: [P1: 0..10] [P3: 10..18] [P2: 18..23]
```

**Vantagem**: flexível; permite políticas de QoS.
**Desvantagem**: *starvation* dos processos de baixa prioridade (mitigado com *aging*, não implementado aqui).

---

### 4. Multilevel Queue

Combina **três filas** com políticas diferentes, cada uma recebendo uma fatia de 10 unidades de tempo, em rodízio:

```
                ┌─────────────────────────────────────────┐
                │  FILA 1: Round Robin (quantum=4)        │ ← alta prioridade
                ├─────────────────────────────────────────┤
                │  FILA 2: Priority Scheduling            │
                ├─────────────────────────────────────────┤
                │  FILA 3: FCFS                           │ ← baixa prioridade
                └─────────────────────────────────────────┘
                    ↓             ↓              ↓
                  10u           10u            10u
                    └─────────────┴──────────────┘
                            rodízio entre filas
```

Cada processo é colocado em **uma única fila** com base em sua prioridade e nas faixas configuradas pelo usuário.

**Vantagem**: diferentes tipos de processo (interativos, batch) podem ser tratados com políticas adequadas.
**Desvantagem**: rigidez - um processo nasce em uma fila e nunca migra (diferentemente do *Multilevel Feedback Queue*).

---

### 5. Lottery Scheduling em C e Java

Algoritmo **probabilístico**: cada processo recebe um número de "tickets" proporcional à sua demanda restante de CPU. A cada rodada, um ticket é sorteado e o processo vencedor executa por um quantum.

**Implementações disponíveis:** `lottery_Java/Lottery.java` e `lottery_c/lottery.c`.

```
Tickets por processo (proporcional a request restante):
  P_a → ▓                    (1 ticket)
  P_b → ▓▓▓▓▓▓▓▓             (8 tickets)
  P_c → ▓▓▓▓                 (4 tickets)
  P_d → ▓▓                   (2 tickets)
                 ──────────
  total          15 tickets

  Sorteio: número aleatório em [0, 15) → determina o vencedor.
```

**Vantagem**: probabilisticamente justo; sem *starvation* (todo processo com tickets > 0 tem chance > 0); fácil ajustar a "prioridade" alterando o número de tickets.
**Desvantagem**: variância - em curtos períodos, a distribuição real pode divergir bastante da proporcional.

---

### 6. Fair Share Scheduling em C e Java

Distribui a CPU entre **grupos** de processos (não entre processos individuais). Cada grupo recebe uma fatia percentual configurada da CPU; dentro do grupo, distribui-se igualmente entre os membros.

**Implementações disponíveis:** `fair-share_Java/src/` e `fair_share_c/fair_share.c`.

```
Configuração:
  Grupo "users"  → 75% da CPU (1500 processos)
  Grupo "admin"  → 25% da CPU (3000 processos)

Tempo →
  ════════════════════════════════════════
  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓░░░░░░░░ ← users (75%)
  ▓▓▓▓▓▓▓░░░░░░░░░░░░░░░░░░░░░░░░ ← admin (25%)
  ════════════════════════════════════════
```

**Arquitetura da versão Java** (`fair-share_Java/src/`):
- `Process` - representa um processo com burst time aleatório
- `ShareGroup` - agrupa processos e calcula a "dívida" do grupo (uso real vs. cota)
- `Processor` - thread que simula a CPU, com taskswitch por quantum
- `ReadyQueue` - `PriorityBlockingQueue` que ordena por prioridade (sistema ou grupo)
- `FSSLogger` - registra em `log.txt` a evolução das taxas de uso por grupo

A prioridade efetiva de um processo é `prioridade_do_grupo + (prioridade_individual % 1)`, onde a prioridade do grupo é a diferença entre o uso real do grupo e sua cota. Quanto mais o grupo já consumiu além da cota, menor sua prioridade.

**Versão em C** (`fair_share_c/fair_share.c`): simula a CPU em um loop lógico, sem criar threads reais, e permite executar em dois modos: `--fss`, considerando a cota dos grupos, e `--system`, usando prioridade simples por processo.

**Vantagem**: garante isolamento entre usuários/departamentos.
**Desvantagem**: implementação mais complexa; pode prejudicar throughput total.

---

### 7. Escalonamento Garantido em C e Java

Promete dividir o tempo de CPU de forma proporcional entre os processos ativos. Ajusta a prioridade dinamicamente conforme cada processo recebe mais ou menos do que seu quinhão.

**Implementações disponíveis:** `garantido_Java/src/` e `escalonamento_garantido_c/escalonamento_garantido.c`.

**Arquitetura MVC da versão Java** (`garantido_Java/src/`):

```
┌────────────────────────────────────────────────────────────┐
│                         VIEW                                │
│  TelaJFrame ──── JTable + botões (Criar/Escalonar/...)     │
│  TesteView  ──── ponto de entrada (main)                   │
└──────────────────────────┬─────────────────────────────────┘
                           │ chamadas
                           ▼
┌────────────────────────────────────────────────────────────┐
│                       CONTROLLER                            │
│  AbstractEscalonador  (interface)                          │
│  Escalonador          (singleton - orquestra fila e CPU)   │
└──────────────────────────┬─────────────────────────────────┘
                           │ usa
                           ▼
┌────────────────────────────────────────────────────────────┐
│                         MODEL                               │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  Processo  ←  ProcessoFactory                       │   │
│  │     ↑                                                │   │
│  │  FilaProcessos / FilaEntrada                        │   │
│  │  Processador (singleton)                            │   │
│  │  Memoria + BlocoMemoria                             │   │
│  │  ModeloTabelaProcessos (AbstractTableModel)         │   │
│  └─────────────────────────────────────────────────────┘   │
│  Interfaces: AbstractFactory, AbstractFilaProcesso,        │
│              AbstractMemoria, AbstractProcessador          │
└────────────────────────────────────────────────────────────┘
```

**Fluxo de execução**:

1. O usuário clica em **Criar** repetidamente para adicionar processos à `FilaEntrada` (entrando com o "surto" - tempo de CPU desejado).
2. Clica em **Escalonar** → `Escalonador.inicializarFilaProcessos()` move processos da entrada para a `FilaProcessos`, alocando memória.
3. A *thread* principal já está em `Escalonador.escalona()` (loop infinito).
4. A cada iteração, o próximo processo é retirado da `FilaProcessos`, recebido pelo `Processador` e processado por `quantum` unidades de tempo.
5. Ao terminar o quantum, `alteraPrioridade()` recalcula a prioridade de todos os processos com base na razão `tempoProcessamento / quantumParcial`:
   - `razão > 1` → recebeu mais que sua cota → **prioridade BAIXA**
   - `razão = 1` → na cota → **prioridade MÉDIA**
   - `razão < 1` → recebeu menos que sua cota → **prioridade ALTA**
6. Se o processo ainda tem surto restante, volta para a fila; se não, é finalizado e libera memória.
7. Os botões **Suspender**, **Prosseguir** e **Finalizar** atuam sobre o processo em execução.

**Versão em C** (`escalonamento_garantido_c/escalonamento_garantido.c`): adapta a lógica da versão Java para execução em terminal. Ela mantém processos, fila de entrada, fila de prontos, memória simulada, processador, quantum, atualização de prioridades e geração de `log.txt`, mas remove a dependência da interface gráfica Swing.

**Características adicionais**:
- Gerência de memória própria com `Memoria` (32 blocos de 4 MB cada).
- Modelo de tabela customizado para reflexão em tempo real do estado dos processos.
- Singletons sincronizados para `Escalonador` e `Processador`.

---

## Comparativo dos algoritmos

| Algoritmo | Implementações | Preemptivo | Starvation? | Conhece burst? | Adequado a |
|-----------|----------------|:----------:|:-----------:|:--------------:|------------|
| **SJF / SRTN** | C | Sim | Sim | Sim | Batch, quando se busca menor waiting time médio |
| **Round Robin** | C e C++ | Sim | Não | Não | Sistemas interativos |
| **Priority Scheduling** | C e C++ | Não, aqui | Sim | Não | QoS e tempo real soft |
| **Multilevel Queue** | C | Parcial | Possível | Não | Ambientes mistos, com filas por tipo de processo |
| **Lottery Scheduling** | C e Java | Sim | Não, probabilisticamente | Não | Justiça probabilística e políticas ajustáveis por tickets |
| **Fair Share Scheduling** | C e Java | Sim | Não, por grupo | Não | Ambientes multiusuário ou multi-grupo |
| **Escalonamento Garantido** | C e Java | Sim | Não | Não | Tempo compartilhado justo |

---

## Correções aplicadas

A versão original do repositório continha diversos bugs - alguns que impediam a compilação, outros sutis que só apareceriam em casos específicos. Abaixo, a lista completa do que foi corrigido.

### `sjf/srtn.c` (anteriormente `srtn.c` na raiz)

| # | Bug original | Correção | Justificativa |
|---|--------------|----------|---------------|
| 1 | `#include <conio.h>` | Removido | Header não-padrão (Turbo C/DOS); não compila em Linux/macOS. |
| 2 | `clrscr()` e `getch()` | Removidos | Funções de `conio.h`. Para "esperar tecla" o padrão é `getchar()`, mas no fluxo deste programa nem é necessário. |
| 3 | `printf("\Entre com...")` | `printf("\nEntre com...")` | `\E` não é uma sequência de escape válida. |
| 4 | `scanf("d", &b[i])` | `scanf("%d", &b[i])` | Faltava o `%`. Sem ele, `scanf` não lê nada e `b[i]` fica indefinido. |
| 5 | `b[9] = 9999;` (sentinela hardcoded) | Removido | Assume sempre exatamente 10 processos. Quebra se `n != 10`. Substituído pela checagem `if (smallest == -1) continue;`. |
| 6 | `smallest = 0;` no início do loop | `smallest = -1;` + checagem | Se o processo 0 ainda não chegou (`arrival[0] > time`), o código antigo o escolhia mesmo assim. Agora `-1` marca "nenhum processo elegível" e a CPU fica ociosa nesse caso. |
| 7 | `printf("%If %If", avg, tt)` | `printf` removido | Especificador `%If` não existe (o original era um `%lf` corrompido). Era uma linha de debug. |
| 8 | `printf("\n\Tempo medio...")` | `printf("\nTempo medio...")` | `\T` não é escape válido. |
| 9 | Arrays fixos `[10]` sem validação | `MAX = 100` + checagem de `n` | Buffer overflow garantido se `n > 10`. |
| 10 | Erros de digitação (`turnaroun`, etc.) | Padronizados | Legibilidade. |

### `round-robin_cpp/round_Robin.cpp`

| # | Bug original | Correção | Justificativa |
|---|--------------|----------|---------------|
| 1 | `int rem_bt[n]; int wt[n]; int tat[n];` | `std::vector<int>` | VLAs **não fazem parte do C++ padrão** (são extensão GCC). Não compilam com `clang++ -pedantic` ou MSVC. |
| 2 | Parâmetro `int processes[]` sem uso interno | Mantido apenas onde é exibido | Limpeza. |

### `priority_cpp/priority.cpp`

| # | Bug original | Correção | Justificativa |
|---|--------------|----------|---------------|
| 1 | `#include <bits/stdc++.h>` | `#include <iostream>`, `<algorithm>`, `<vector>` | Header não-padrão (libstdc++ apenas); inflaciona tempo de compilação. |
| 2 | VLAs `int wt[n], tat[n]` | `std::vector<int>` | Idem Round Robin. |
| 3 | Passagem por valor de `Process proc[]` | `const std::vector<Process>&` | Evita cópias e dá const-correctness. |

### `multilevel/multiLevel.c` (anteriormente `multiLevel.cpp`)

Este foi o arquivo com mais problemas estruturais.

| # | Bug original | Correção | Justificativa |
|---|--------------|----------|---------------|
| 1 | Extensão `.cpp` mas código C puro | Renomeado para `.c` | Usa `stdio.h`, `stdbool.h`, não usa features C++. Convenção. |
| 2 | `RR()`, `PS()`, `FCFS()` se chamam mutuamente sem **forward declaration** | Reestruturado | Em C, função não pode ser chamada antes de declarada. O código original não compilava nem com GCC permissivo. |
| 3 | **Recursão mútua infinita**: `RR → PS → FCFS → RR → ...` | Loop principal único | Cada chamada empilha um frame. Em workloads longos → **stack overflow**. Substituído por um `while (!todos_terminaram())` que invoca `run_RR()`, `run_PS()`, `run_FCFS()` em sequência. |
| 4 | `if (q == 10 || prc[queue1[0]].rem_burst_time == 0 && prc[queue1[1]].rem_burst_time == 0)` | Lógica reescrita | (a) Precedência: `&&` liga mais forte que `||`, então a expressão era `q==10 \|\| (a && b)` - provavelmente o intento. (b) `queue1[1]` é **segfault se só houver 1 processo na fila**. |
| 5 | Variável global `int queue` declarada mas nunca usada | Removida | Lixo. |
| 6 | `i = 0; break;` dentro do `for` (gambiarra para reiniciar) | Loops reestruturados com `gasto < FATIA` | Difícil de auditar e ineficiente. |
| 7 | `exit(0)` em meio à recursão | Retorno limpo pelo `main` | `exit(0)` no meio de funções recursivas impede liberação de recursos e dificulta uso como biblioteca. |
| 8 | `if (q1 == 0)` levaria a loop infinito em `RR()` | Checagem `if (q1 > 0) run_RR();` no `main` | Defensivo. |
| 9 | Arrays globais sem `static` | Marcados `static` | Encapsulamento (escopo de arquivo). |
| 10 | Sem validação de entrada | `scanf` retorna verificado | Robustez. |

### `lottery_Java/Lottery.java`

| # | Bug original | Correção | Justificativa |
|---|--------------|----------|---------------|
| 1 | Arquivo `lottery.java`, classe `Lottery` | Renomeado para `Lottery.java` | Java exige `nome do arquivo == nome da classe pública`. O original não compila com `javac` estrito. |
| 2 | Bubble sort com `>=` | Trocado por `>` | `>=` torna o sort instável e pode causar trocas desnecessárias. |
| 3 | **Dead code** no `else if`: `(request[i] > 0) && (time < arrival[i])` | Removido | A condição anterior já era `request[i] > 0`, então este `else if` é inalcançável. |
| 4 | `temp[i] = process[p]` onde `p` é contador de tickets (não índice de processo) | Reestruturado | Podia causar `ArrayIndexOutOfBoundsException` quando `p ≥ 10`. |
| 5 | **Sorteio de tickets defeituoso**: `max_tick` atualizado a cada iteração mas com `p` global, sem reset entre rodadas | Reescrito: tickets recalculados a cada rodada, soma cumulativa, `rand.nextInt(total)` | A versão original podia sortear um número que não correspondia a nenhum ticket existente. |
| 6 | `arrival` gerado mas ignorado no sorteio | Agora respeitado (processos só competem se já chegaram) | Sem isso, processos com `arrival > 0` recebem CPU antes de existirem. |
| 7 | Mistura de identação tabs/espaços e indentação confusa | Padronizado (4 espaços) | Legibilidade. |
| 8 | Avanço de `time` em vários locais com lógica conflitante | Centralizado em `time += consumido` | Tornava difícil rastrear o "tempo simulado". |

### `garantido_Java/` - Java MVC

| # | Bug original | Correção | Justificativa |
|---|--------------|----------|---------------|
| 1 | 3 arquivos em **ISO-8859-1** (`Memoria.java`, `TelaJFrame.java`, `Escalonador.java`) | Convertidos para UTF-8 | `javac` moderno (Java 11+) usa UTF-8 por padrão. Os arquivos não compilavam - erros `unmappable character`. |
| 2 | `AbstractEscalonador.getProximoProcesso()` retorna `Thread` | Alterado para `Processo` | Inconsistência semântica. Funcionava por covariância (`Processo extends Thread`), mas a interface deveria refletir o domínio. |
| 3 | `definirQuantum()`: `filaProcessos.tamanho() * 2 / filaProcessos.tamanho()` | Reescrito como `Math.max(2, Math.min(10, 20/qtd))` | O cálculo original **sempre retorna 2** (n*2/n). Era um bug; substituído por uma fórmula proporcional real. |
| 4 | `escalona()` com `while(true)` **sem nada para fazer quando a fila esvazia** | Adicionado `Thread.sleep(200)` no loop externo | Busy-wait → 100% de CPU em um core sempre que a fila ficava vazia. |
| 5 | Campo `filaEntrada` nunca inicializado em `Escalonador` | Inicializado no construtor + armazenado em `inicializarFilaProcessos()` | `getFilaEntrada()` sempre retornava `null`. |
| 6 | `Escalonador.getInstancia()` e `Processador.getInstancia()` não thread-safe | Marcados `synchronized` | Padrão singleton ingênuo: duas threads podiam criar duas instâncias. |
| 7 | `Memoria.desfragmenta()` corpo vazio | Implementado: compacta blocos ocupados no início | Método declarado pela interface mas não funcional. |
| 8 | `Processo.incrementaQuantum()` com `System.out.println(++quantum)` | `println` removido | Saída de debug poluindo stdout. |
| 9 | `calcularQuantumParcial()` divide por `filaProcessos.tamanho()` sem verificar zero | Adicionada checagem `if (tamanho == 0) return 0` | `ArithmeticException` se a fila ficasse vazia. |
| 10 | `alteraPrioridade()` divide por `quantumParcial` que pode ser 0 | Adicionada checagem | Idem. |
| 11 | `Processador.retira()` faz `setQuantum(0)` em referência possivelmente `null` | Adicionado guard `if (null) return null` | NPE se `finalizar()` for clicado sem nada executando. |
| 12 | `Escalonador.finalizar()` não checava `null` do retorno de `retira()` | Adicionado early-return | NPE. |
| 13 | Cast redundante `(Processo) processo` (já era `Processo`) | Removido | Warning `[cast]` no `javac`. |
| 14 | Cast redundante `(String[]) i.next()` (iterator já era genérico) | Removido | Warning `[cast]`. |

### `fair-share_Java/`

| # | Bug original | Correção | Justificativa |
|---|--------------|----------|---------------|
| 1 | (Mantido) `catch(NullPointerException)` vazio em `ReadyQueue.schedule()` | Mantido com comentário | Defensivo contra race condition entre `isEmpty()` e `poll()` em `PriorityBlockingQueue`. Não é elegante, mas é funcional. |

---

## Limitações conhecidas

Alguns aspectos foram **deliberadamente não corrigidos** por exigirem refatoração arquitetural mais profunda que mudaria o caráter didático original:

### Garantido

- **`Processo extends Thread` mas nunca usa thread**: a herança é semanticamente confusa, mas refatorar exigiria mudar todas as APIs do projeto. O código funciona - `Processo` é apenas tratado como objeto de dados.
- **`Processador.processa()` chama `Thread.sleep(1000)`**: quando disparado pela UI (botão "Escalonar"), o sleep acontece na **thread principal do escalonador** (não na EDT do Swing), então a UI continua responsiva. Mas a atualização da tabela é feita fora da EDT via `TelaJFrame.atualizarLinha()`, o que **viola o contrato do Swing**. Em produção, deveria usar `SwingUtilities.invokeLater()` ou `SwingWorker`.
- **Não há mecanismo de parar o escalonador**: o `while(true)` em `escalona()` só pode ser interrompido fechando a JVM.
- **`Memoria` não trata fragmentação automaticamente**: `desfragmenta()` foi implementado mas nunca é chamado automaticamente.

### Fair Share

- O programa de teste roda por **alguns minutos** (1500+3000 processos com burst time aleatório de até 290 unidades). Não há flag para reduzir a carga.
- O `log.txt` é sobrescrito a cada execução.

### Multilevel

- Os processos não migram entre filas (esta é uma *Multilevel Queue*, não uma *Multilevel Feedback Queue*).
- Faixas de prioridade entre filas podem se sobrepor; nesse caso, o processo entra na **primeira fila cujo intervalo aceita** sua prioridade (decisão de design).

---

## Licença e créditos

Material didático para a disciplina de **Sistemas Operacionais - Sistemas Interativos**.
Autoria das versões originais creditada nos comentários dos arquivos Java (`@author Menno.VanDiermen` no fair-share, `By Pimpolhos` no garantido).

Reorganização, correção de bugs e documentação: versão revisada.
