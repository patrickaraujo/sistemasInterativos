# Round Robin Scheduling em C

Este projeto é uma versão em C do código `round_Robin.cpp`.

O programa simula o algoritmo de **Round Robin Scheduling**, ou seja, escalonamento circular por quantum.

Nesse algoritmo, cada processo recebe a CPU por uma fatia fixa de tempo chamada **quantum**. Se o processo terminar antes ou exatamente nesse quantum, ele sai da fila. Se ainda precisar de mais CPU, ele volta para a fila e espera sua próxima vez.

## Arquivos

- `round_robin.c`: código-fonte em C, com comentários explicativos.
- `README_round_robin.md`: explicação do funcionamento do código.
- `log.txt`: arquivo gerado ao executar o programa, contendo a saída da simulação.

## Como compilar

Use:

```bash
gcc -O2 -std=c11 -Wall -Wextra round_robin.c -o round_robin
```

## Como executar

Use:

```bash
./round_robin
```

Ao executar, o programa mostra a saída no terminal e também cria o arquivo:

```bash
log.txt
```

## O que o algoritmo faz

O Round Robin distribui a CPU entre os processos de forma alternada.

A ideia é simples:

1. o primeiro processo executa por no máximo um quantum;
2. se ele terminar, sai da fila;
3. se não terminar, volta para esperar outra rodada;
4. o próximo processo executa pelo mesmo critério;
5. o ciclo continua até todos os processos terminarem.

Esse tipo de escalonamento evita que um único processo monopolize a CPU por muito tempo.

## Dados usados no exemplo

O código usa os mesmos dados do exemplo original:

| Processo | Burst time |
|---:|---:|
| 1 | 10 |
| 2 | 5 |
| 3 | 8 |

O quantum usado é:

```text
2
```

Isso significa que cada processo pode executar por no máximo 2 unidades de tempo por rodada.

## Linha do tempo da execução

Com quantum igual a 2, a execução acontece assim:

| Rodada | Processo | Tempo executado | Tempo atual |
|---:|---:|---:|---:|
| 1 | 1 | 2 | 2 |
| 1 | 2 | 2 | 4 |
| 1 | 3 | 2 | 6 |
| 2 | 1 | 2 | 8 |
| 2 | 2 | 2 | 10 |
| 2 | 3 | 2 | 12 |
| 3 | 1 | 2 | 14 |
| 3 | 2 | 1 | 15 |
| 3 | 3 | 2 | 17 |
| 4 | 1 | 2 | 19 |
| 4 | 3 | 2 | 21 |
| 5 | 1 | 2 | 23 |

O processo 2 termina antes de usar um quantum completo na terceira rodada, porque faltava apenas 1 unidade de tempo.

## Waiting time

O **waiting time** é o tempo total que um processo passa esperando para usar a CPU.

Neste exemplo, todos os processos chegam no tempo 0. Então, quando um processo termina, seu waiting time pode ser calculado como:

```text
waiting time = tempo de término - burst time
```

Resultado:

| Processo | Burst time | Waiting time |
|---:|---:|---:|
| 1 | 10 | 13 |
| 2 | 5 | 10 |
| 3 | 8 | 13 |

## Turnaround time

O **turnaround time** é o tempo total até o processo terminar.

Como todos os processos chegam no tempo 0:

```text
turnaround time = burst time + waiting time
```

Resultado:

| Processo | Burst time | Waiting time | Turnaround time |
|---:|---:|---:|---:|
| 1 | 10 | 13 | 23 |
| 2 | 5 | 10 | 15 |
| 3 | 8 | 13 | 21 |

## Médias

Com os dados acima, o programa calcula:

```text
Tempo medio de espera  = 12.00
Tempo medio de retorno = 19.67
```

## Explicação das principais partes do código

### `print_both`

Imprime a saída no terminal e, ao mesmo tempo, grava no arquivo `log.txt`.

Isso evita duplicação de código e mantém o terminal e o log com o mesmo conteúdo.

### `all_done`

Verifica se todos os processos já terminaram.

Ela percorre o vetor de burst time restante. Se todos os valores forem zero, significa que não há mais processo pendente.

### `find_waiting_time`

É a função mais importante do programa.

Ela simula o Round Robin de fato.

Dentro dela:

- `remaining_burst` guarda quanto tempo ainda falta para cada processo terminar;
- `current_time` guarda o tempo atual da simulação;
- cada processo executa por no máximo `quantum` unidades de tempo;
- quando um processo termina, seu waiting time é calculado.

### `find_turnaround_time`

Calcula o turnaround time de cada processo.

O cálculo é:

```text
turnaround time = burst time + waiting time
```

### `find_average_time`

Organiza a execução do programa.

Ela faz quatro coisas:

1. imprime o estado inicial;
2. chama a simulação Round Robin;
3. calcula o turnaround time;
4. imprime as métricas finais e as médias.

### `main`

Define os processos, os burst times e o quantum.

Também cria o arquivo `log.txt` e chama a função principal da simulação.

## Diferença em relação ao código C++

O código original estava em C++ e usava:

- `vector`;
- `cout`;
- referências de C++.

A versão em C usa:

- arrays fixos;
- funções com ponteiros e vetores;
- `printf`;
- `FILE*` para gerar o `log.txt`.

A lógica do escalonamento foi mantida, mas o código foi adaptado para C puro.
