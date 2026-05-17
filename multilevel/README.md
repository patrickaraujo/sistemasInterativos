# Simulador de Escalonamento em Filas Multinível (C)

Este projeto implementa um simulador de escalonamento de CPU utilizando a estratégia de Filas Multinível (Multilevel Queue Scheduling) na linguagem C. O sistema classifica os processos em três filas distintas com base nas suas prioridades, aplicando um algoritmo de escalonamento específico a cada uma delas.

O código é escrito em C puro, utilizando as bibliotecas padrão `<stdio.h>`, `<stdlib.h>` e `<stdbool.h>`.

---

## Arquitetura do Escalonador

O processador simulado divide o tempo de CPU entre **três filas**, alternando entre elas. Cada fila recebe uma fatia de tempo (`FATIA`) de **10 unidades de tempo** por rodada.

As políticas de escalonamento de cada fila são:

1.  **Queue 1 (Mais prioritária):** Utiliza o algoritmo **Round Robin (RR)** com um quantum interno de 4 unidades de tempo.
2.  **Queue 2 (Prioridade média):** Utiliza o algoritmo **Priority Scheduling (PS)**, onde o processo com o menor número de prioridade é executado primeiro.
3.  **Queue 3 (Menos prioritária):** Utiliza o algoritmo **First Come First Served (FCFS)**, executando os processos na ordem em que foram inseridos na fila.

---

## Como Compilar e Executar

Para compilar o código fonte, utilize o compilador `gcc` com a flag `-Wall` para exibir todos os avisos de compilação:

```bash
gcc -Wall -o multilevel multiLevel.c

```

Para executar o programa gerado:

```bash
./multilevel

```

---

## Fluxo de Utilização

O simulador é interativo e requer a introdução de dados via terminal. O programa suporta até um máximo de **50 processos** simultâneos (`MAX_PROC`).

1. **Definição das Faixas de Prioridade:** O programa começa por pedir o limite inferior e superior de prioridade para cada uma das três filas. O utilizador deve introduzir os valores no formato `min max`.
2. **Quantidade de Processos:** O utilizador define quantos processos vão participar na simulação.
3. **Detalhes dos Processos:** Para cada processo, o programa solicita dois valores: a **Prioridade** e o tempo necessário de CPU (**Burst Time**).
4. **Distribuição:** O simulador verifica a prioridade de cada processo e aloca-o automaticamente na primeira fila cuja faixa de prioridade corresponda ao valor introduzido. Caso a prioridade do processo não se encaixe em nenhuma das faixas definidas, o programa emite um erro e é encerrado.

---

## Resultados da Simulação

O loop principal da simulação alterna a execução do tempo de CPU (`run_RR` -> `run_PS` -> `run_FCFS`) garantindo as fatias de 10 unidades de tempo a cada fila ativa, até que o tempo restante (`rem_burst_time`) de todos os processos chegue a zero.

No final, o sistema imprime uma tabela consolidada com os seguintes resultados para cada processo:

* **Waiting time (wt):** O tempo total que o processo passou à espera para ser totalmente executado.
* **Turnaround time:** O tempo total decorrido desde a chegada do processo até à sua conclusão (Waiting Time + Burst Time).

A simulação também calcula e apresenta o **tempo médio de espera** e o **tempo médio de retorno** (turnaround) do sistema como um todo.
