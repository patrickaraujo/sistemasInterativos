# Fair-Share Scheduler em C

Este projeto é uma versão em C do simulador de escalonamento Fair-Share. Ele simula uma CPU executando processos de diferentes grupos e compara dois modos de escalonamento:

1. `--fss`: Fair-Share Scheduler, que tenta respeitar a porcentagem de CPU definida para cada grupo.
2. `--system`: prioridade simples por processo, sem considerar a porcentagem de CPU dos grupos.

O programa não cria threads reais. Ele é um simulador em loop: cada iteração representa um pequeno avanço no tempo lógico da CPU.

## Arquivos

- `fair_share.c`: código-fonte em C, com comentários explicando as principais estruturas e funções.
- `README.md`: explicação do projeto, comandos de compilação e execução.
- `log.txt`: arquivo gerado após a execução, contendo a evolução do uso de CPU dos grupos ao longo do tempo.

## Como compilar

Use `gcc` com suporte ao padrão C11:

```bash
gcc -O2 -std=c11 -Wall -Wextra fair_share.c -lm -o fair_share
```

O parâmetro `-lm` é necessário porque o código usa funções matemáticas, como `floor` e `fabs`.

## Como executar

Para rodar com Fair-Share Scheduler:

```bash
./fair_share --fss
```

Para rodar com prioridade normal do sistema:

```bash
./fair_share --system
```

Também é possível fixar a seed da simulação. Isso é útil para repetir exatamente o mesmo experimento:

```bash
./fair_share --fss --seed 42
```

## O que o programa faz

O simulador cria dois grupos de processos:

| Grupo | Share desejado | Quantidade de processos |
|---|---:|---:|
| `users` | 75% | 1500 |
| `admin` | 25% | 3000 |

Cada processo recebe um tempo de execução aleatório, chamado de `burst`. Durante a simulação, os processos chegam gradualmente à fila de prontos. A CPU executa um processo por vez e troca de processo quando o quantum termina ou quando o processo atual finaliza.

O quantum usado no código é:

```c
#define QUANTUM 50
```

Isso significa que um processo pode executar por até 50 ticks antes de voltar para a fila.

## Estruturas principais do código

### `Process`

Representa um processo simulado.

Campos importantes:

- `count`: quanto tempo de CPU o processo já consumiu.
- `burst`: quanto tempo total de CPU o processo precisa para terminar.
- `last_run`: último instante em que o processo foi colocado de volta na fila.
- `group`: grupo ao qual o processo pertence.
- `idle`: indica se o processo é o processo ocioso da CPU.

### `ShareGroup`

Representa um grupo de processos com uma porcentagem desejada de CPU.

Exemplo:

```c
ShareGroup *users = share_group_create(75);
ShareGroup *admin = share_group_create(25);
```

Nesse caso, o grupo `users` deveria receber aproximadamente 75% da CPU enquanto houver processos suficientes para executar, e o grupo `admin` deveria receber aproximadamente 25%.

### `ReadyQueue`

Representa a fila de processos prontos.

A fila é implementada como um `min-heap`. Isso significa que o processo com menor valor de prioridade fica no topo e é escolhido primeiro.

O campo mais importante é:

```c
bool fair_share;
```

Quando `fair_share` é `true`, o escalonador considera o grupo do processo. Quando é `false`, ele usa apenas a prioridade individual do processo.

### `Processor`

Representa a CPU simulada.

Campos importantes:

- `time`: tempo lógico total da simulação.
- `cycle`: quantidade de ticks desde a última troca de contexto.
- `current_task`: processo atualmente em execução.
- `queue`: fila de processos prontos.

## Como a prioridade é calculada

No modo `--system`, a prioridade considera apenas o processo individual:

```c
return (double) p->count / (double) p->last_run;
```

A ideia é dar preferência a processos que consumiram menos CPU em relação ao tempo em que ficaram esperando.

No modo `--fss`, a prioridade também considera o grupo:

```c
return share_group_priority(p->group) + fractional;
```

A função `share_group_priority` compara o uso real do grupo com o uso esperado. Se um grupo recebeu menos CPU do que deveria, seus processos tendem a ganhar prioridade.

## Processo idle

O código possui um processo especial chamado `idle_process`.

Ele é executado quando a fila de prontos está vazia. Esse processo não representa trabalho real. Ele apenas permite medir quanto tempo a CPU ficou ociosa.

O tempo ocioso é acumulado em:

```c
q->idle_time
```

## Arquivo `log.txt`

Durante a execução, o simulador escreve no arquivo `log.txt` a evolução do uso de CPU dos grupos.

O formato é:

```text
tempo    taxa_users    taxa_admin
```

Exemplo:

```text
1000    74.5000    25.5000
2000    75.1000    24.9000
```

Esses valores ajudam a observar se o Fair-Share Scheduler está aproximando o uso real da CPU dos shares definidos para os grupos.

## Saída no terminal

Ao final da execução, o programa mostra um resumo parecido com este:

```text
Mode: fair-share
Busy time: 698325, idle time: 49
Admin group total: 464502
User group total: 229423
Admin actual share: 66.52%
User actual share: 32.85%
Total number of processes: 4500
CPU simulation time: 10.240s
Finished. Rates were written to log.txt
```

Importante: a porcentagem final pode não ficar exatamente em 75% e 25%, porque o total de trabalho disponível em cada grupo também influencia o resultado. Neste exemplo, o grupo `admin` tem mais processos do que `users`. O Fair-Share Scheduler tenta controlar a divisão enquanto existem processos prontos nos dois grupos, mas ele não consegue dar 75% de CPU para um grupo que já não tem mais trabalho disponível.

## Fluxo geral da simulação

O fluxo principal acontece dentro da função `main`:

1. O programa lê os argumentos de linha de comando.
2. A CPU simulada é inicializada.
3. Os grupos `users` e `admin` são criados.
4. Os processos são criados e colocados em pools.
5. Aos poucos, processos dos dois grupos chegam à fila de prontos.
6. A CPU executa ticks de simulação.
7. Quando o quantum termina, ocorre uma troca de contexto.
8. Quando todos os processos terminam, o programa imprime o resumo final.

## Diferença entre os modos

### `--system`

Usa apenas a prioridade individual de cada processo. O grupo do processo não influencia diretamente na escolha.

### `--fss`

Usa a prioridade do grupo mais a prioridade individual. Isso permite que o escalonador favoreça temporariamente o grupo que recebeu menos CPU do que deveria receber.

## Observação sobre a versão em C

Esta versão mantém a lógica principal do projeto original, mas foi adaptada para C como um simulador sequencial. Em vez de criar uma `Thread` como no Java, o programa executa uma função de tick várias vezes, controlando manualmente tempo, quantum, fila, troca de contexto e término dos processos.
