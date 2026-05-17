# Escalonamento Garantido em C

Este projeto é uma versão em C do simulador Java enviado no arquivo `garantido.zip`.

O projeto original usa uma interface gráfica em Java Swing. Esta versão foi adaptada para rodar pelo terminal, mantendo a lógica principal do escalonador: criação de processos, fila de entrada, fila de prontos, uso de memória, quantum, execução na CPU e atualização de prioridades.

## Arquivos

- `escalonamento_garantido.c`: código-fonte em C com comentários explicativos.
- `README_escalonamento_garantido.md`: explicação do projeto, compilação, execução e funcionamento.
- `log.txt`: arquivo gerado durante a execução com o passo a passo do escalonamento.

## Como compilar

Use `gcc` com o padrão C11:

```bash
gcc -O2 -std=c11 -Wall -Wextra escalonamento_garantido.c -lm -o escalonamento_garantido
```

O `-lm` é usado porque o código utiliza funções da biblioteca matemática de C.

## Como executar

Para rodar com o conjunto padrão de processos:

```bash
./escalonamento_garantido
```

Para informar manualmente os surtos dos processos:

```bash
./escalonamento_garantido 10 4 7 13 6
```

Nesse exemplo, o programa cria cinco processos:

| Processo | Surto |
|---|---:|
| P1 | 10 |
| P2 | 4 |
| P3 | 7 |
| P4 | 13 |
| P5 | 6 |

Cada número passado na linha de comando representa o tempo de CPU necessário para um processo terminar.

## O que o programa faz

O programa simula um escalonador de processos usando a ideia de escalonamento garantido.

A execução segue este fluxo:

1. Cria processos com estado `NOVO`.
2. Coloca os processos na fila de entrada.
3. Move processos para a fila de prontos quando há espaço na memória.
4. Escolhe um processo pronto para executar.
5. Executa o processo na CPU até ele terminar ou até consumir seu quantum.
6. Se o processo terminou, libera sua memória.
7. Se o processo ainda não terminou, ele volta para a fila de prontos.
8. Atualiza as prioridades dos processos prontos.
9. Repete até todos os processos terminarem.

## Relação com o projeto Java original

A versão Java tinha estas classes principais:

| Java | Versão em C |
|---|---|
| `Processo` | `Process` |
| `FilaEntrada` | `ProcessQueue` com modo FIFO |
| `FilaProcessos` | `ProcessQueue` ordenada por prioridade |
| `Memoria` | `Memory` |
| `Processador` | `Processor` |
| `Escalonador` | `Scheduler` |
| `TelaJFrame` | Saída no terminal e `log.txt` |

A principal diferença é que a versão Java tinha interface gráfica. Em C, a simulação é executada automaticamente no terminal.

## Estruturas principais do código

### `Process`

Representa um processo simulado.

Campos importantes:

- `id`: identificador do processo.
- `name`: nome do processo, como `P1`, `P2`, `P3`.
- `mb`: quantidade de memória usada pelo processo.
- `burst_remaining`: tempo restante para o processo terminar.
- `burst_initial`: tempo inicial do processo.
- `processed_time`: quanto tempo de CPU o processo já recebeu.
- `quantum_used`: quanto do quantum atual o processo já usou.
- `quantum_ratio`: valor usado para atualizar a prioridade.
- `state`: estado atual do processo.
- `priority`: prioridade atual do processo.

Estados possíveis:

```c
typedef enum {
    STATE_NOVO,
    STATE_PRONTO,
    STATE_SUSPENSO,
    STATE_EXECUTANDO,
    STATE_FINALIZADO
} State;
```

Prioridades possíveis:

```c
typedef enum {
    PRIORITY_ALTA = 0,
    PRIORITY_MEDIA = 1,
    PRIORITY_BAIXA = 2
} Priority;
```

A ordem é importante. Como `ALTA` tem valor menor, ela é escolhida antes na fila de prontos.

## `ProcessQueue`

Representa uma fila de processos.

Ela pode funcionar de duas formas:

1. Como fila simples, para a fila de entrada.
2. Como fila ordenada por prioridade, para a fila de prontos.

Campo principal:

```c
bool ordered_by_priority;
```

Quando esse campo é `false`, a fila remove o primeiro processo inserido.

Quando esse campo é `true`, a fila remove o processo com maior prioridade.

## `Memory`

Representa a memória simulada.

O projeto original usa 32 blocos de 4 MB. Esta versão mantém a mesma ideia:

```c
#define MEMORY_BLOCKS 32
#define BLOCK_SIZE_MB 4
#define MEMORY_TOTAL_MB (MEMORY_BLOCKS * BLOCK_SIZE_MB)
```

Cada processo usa 8 MB por padrão:

```c
#define DEFAULT_PROCESS_MB 8
```

Então cada processo ocupa dois blocos de memória.

A memória é representada assim:

```c
int blocks[MEMORY_BLOCKS];
```

Cada posição guarda:

- `0`, quando o bloco está livre;
- ID do processo, quando o bloco está ocupado.

## `Processor`

Representa a CPU simulada.

Campos principais:

- `running`: processo atualmente em execução.
- `memory`: memória associada à CPU.
- `total_time`: tempo total de CPU já executado.
- `quantum`: quantum atual.
- `partial_time`: contador auxiliar de tempo.

A CPU executa um processo por vez.

## `Scheduler`

Representa o escalonador.

Ele agrupa:

- fila de entrada;
- fila de prontos;
- fila de suspensos;
- CPU;
- estado de início da simulação.

A função principal é:

```c
static void scheduler_run(Scheduler *scheduler, FILE *log)
```

Ela executa o escalonamento até todos os processos terminarem.

## Como o quantum é definido

O quantum é definido com base na quantidade de processos prontos.

A ideia é simples: quando há muitos processos prontos, cada processo recebe um quantum menor. Quando há poucos processos, o quantum pode ser maior.

No código:

```c
int quantum = 20 / ready_count;
```

Depois o valor é limitado entre 2 e 10:

```c
if (quantum < 2) {
    quantum = 2;
}
if (quantum > 10) {
    quantum = 10;
}
```

Isso segue a lógica já presente no Java original.

## Como a prioridade é atualizada

Depois que um processo executa e volta para a fila, o escalonador recalcula as prioridades dos processos prontos.

A lógica é:

- quem recebeu mais CPU que a média fica com prioridade `BAIXA`;
- quem está próximo da média fica com prioridade `MEDIA`;
- quem recebeu menos CPU que a média fica com prioridade `ALTA`.

Essa parte está na função:

```c
static void scheduler_update_priorities(Scheduler *scheduler)
```

Isso faz com que processos que receberam menos CPU tenham mais chance de executar em seguida.

## Como a memória funciona

Antes de um processo ir para a fila de prontos, o escalonador verifica se há espaço contíguo na memória.

Função usada:

```c
static bool memory_has_space(const Memory *memory, int megabytes)
```

Se houver espaço, o processo é armazenado:

```c
static bool memory_store(Memory *memory, const Process *process)
```

Quando o processo termina, sua memória é liberada:

```c
static void memory_free_process(Memory *memory, const Process *process)
```

## Arquivo `log.txt`

Durante a execução, o programa cria um arquivo chamado `log.txt`.

Esse arquivo mostra:

- processos admitidos na memória;
- processo escolhido para executar;
- quantum usado;
- tempo restante do processo;
- preempções;
- finalizações;
- estado resumido da memória.

Exemplo de linha do log:

```text
tempo=006 | executando=P2 | restante=3 | processado=5 | quantum_usado=2
```

## Saída no terminal

Ao final, o programa imprime um resumo como este:

```text
Escalonamento finalizado.
Tempo total de CPU: 23 ticks
Memoria utilizada no fim: 0MB/128MB
Log gerado em: log.txt

ID   Nome   Priorid.   Estado       Rest.    Processado   Razao
1    P1     MEDIA      FINALIZADO   0        5            0.00
2    P2     MEDIA      FINALIZADO   0        8            1.00
```

## Observação sobre suspensão e retomada

O Java original tinha botões para suspender, retomar e finalizar processos manualmente.

Esta versão em C inclui funções equivalentes:

```c
scheduler_suspend_running_process(...)
scheduler_resume_one_process(...)
scheduler_finish_running_process(...)
```

No modo automático, elas não são chamadas. Elas ficaram no código para mostrar como essas ações poderiam ser adaptadas caso você queira transformar o simulador em um programa interativo depois.

## Resumo

Esta versão em C mantém o núcleo do projeto original:

- processos com surto;
- estados de processo;
- prioridades;
- fila de entrada;
- fila de prontos;
- memória em blocos;
- CPU com quantum;
- preempção;
- finalização;
- log da execução.

A parte visual foi substituída por saída no terminal e pelo arquivo `log.txt`.
