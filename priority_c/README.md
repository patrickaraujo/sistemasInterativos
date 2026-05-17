# Priority Scheduling em C

Este projeto é uma versão em C do código `priority.cpp`.

O programa simula o algoritmo de **Priority Scheduling**, ou seja, escalonamento por prioridade.

Neste exemplo, o algoritmo é **não-preemptivo**. Isso significa que, depois que um processo começa a executar, ele continua até terminar. Nenhum outro processo interrompe sua execução no meio.

## Arquivos

- `priority.c`: código-fonte em C, com comentários explicativos.
- `README_priority.md`: explicação do funcionamento do código.
- `log.txt`: arquivo gerado ao executar o programa, contendo a saída da simulação.

## Como compilar

Use:

```bash
gcc -O2 -std=c11 -Wall -Wextra priority.c -o priority
```

## Como executar

Use:

```bash
./priority
```

Ao executar, o programa mostra a saída no terminal e também cria o arquivo:

```bash
log.txt
```

## O que o algoritmo faz

O algoritmo recebe uma lista de processos. Cada processo possui:

- `pid`: identificador do processo;
- `burst_time`: tempo de CPU necessário para terminar;
- `priority`: prioridade do processo.

A convenção usada neste código é:

```text
maior número = maior prioridade
```

Assim, um processo com prioridade `2` executa antes de um processo com prioridade `1`, que executa antes de um processo com prioridade `0`.

## Processos usados no exemplo

O código usa estes processos:

| Processo | Burst time | Prioridade |
|---:|---:|---:|
| 1 | 10 | 2 |
| 2 | 5 | 0 |
| 3 | 8 | 1 |

Como a maior prioridade executa primeiro, a ordem esperada é:

```text
1 3 2
```

## Waiting time

O **waiting time** é o tempo que um processo espera antes de começar a executar.

Como o escalonamento é não-preemptivo:

- o primeiro processo espera `0`;
- o segundo espera o tempo de execução do primeiro;
- o terceiro espera o tempo de execução do primeiro mais o tempo do segundo.

Com a ordem `1 3 2`:

| Processo | Burst time | Waiting time |
|---:|---:|---:|
| 1 | 10 | 0 |
| 3 | 8 | 10 |
| 2 | 5 | 18 |

## Turnaround time

O **turnaround time** é o tempo total até o processo terminar.

Neste exemplo, todos os processos estão disponíveis desde o início. Então:

```text
turnaround time = waiting time + burst time
```

Resultado:

| Processo | Burst time | Waiting time | Turnaround time |
|---:|---:|---:|---:|
| 1 | 10 | 0 | 10 |
| 3 | 8 | 10 | 18 |
| 2 | 5 | 18 | 23 |

## Explicação das principais partes do código

### `struct Process`

Representa um processo:

```c
typedef struct {
    int pid;
    int burst_time;
    int priority;
} Process;
```

Cada processo tem um identificador, um tempo de execução e uma prioridade.

### `sort_by_priority`

Ordena os processos em ordem decrescente de prioridade.

Ou seja, os processos com maior prioridade vão para o começo da fila.

Em caso de empate, o código usa o menor `pid` primeiro.

### `find_waiting_time`

Calcula quanto tempo cada processo espera antes de executar.

Como o algoritmo é não-preemptivo, o waiting time de um processo é a soma dos burst times dos processos que vieram antes dele.

### `find_turnaround_time`

Calcula o tempo total de retorno de cada processo.

O turnaround time é calculado somando o waiting time com o burst time.

### `find_average_time`

Calcula e imprime:

- waiting time de cada processo;
- turnaround time de cada processo;
- tempo médio de espera;
- tempo médio de retorno.

### `priority_scheduling`

É a função principal do escalonador.

Ela faz três coisas:

1. ordena os processos por prioridade;
2. imprime a ordem de execução;
3. calcula e mostra as métricas.

## Diferença em relação ao código C++

O código original estava em C++ e usava:

- `vector`;
- `sort`;
- `cout`.

A versão em C usa:

- arrays fixos;
- `struct`;
- função própria de ordenação;
- `printf`;
- `FILE*` para gerar o `log.txt`.

A lógica do escalonamento foi mantida.
