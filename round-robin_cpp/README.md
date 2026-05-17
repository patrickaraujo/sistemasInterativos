# Simulador de Escalonamento Round Robin (C++)

Este projeto implementa um simulador simples do algoritmo de **Escalonamento Round Robin** na linguagem C++. O Round Robin é um dos algoritmos de escalonamento preemptivo mais antigos e justos, desenhado especialmente para sistemas de tempo partilhado (*time-sharing*). 

A premissa principal do algoritmo é que cada processo recebe uma fatia fixa de tempo da CPU, denominada **quantum**. Quando o *quantum* de um processo expira, se este ainda não tiver concluído a sua execução, é interrompido (preempção) e colocado no fim da fila para aguardar o seu próximo turno.

---

## Como Compilar e Executar

O código foi escrito utilizando a norma C++17 e tira partido de bibliotecas padrão como `<iostream>` e `<vector>`.

Para compilar o código fonte no seu terminal, utilize o compilador `g++`:

```bash
g++ -std=c++17 -Wall -o round_robin round_Robin.cpp

```

Para executar o ficheiro compilado:

```bash
./round_robin

```

---

## Estrutura e Funcionamento do Código

O simulador é composto por um conjunto de funções que calculam as métricas de tempo de cada processo iterativamente:

### 1. Cálculo do Tempo de Espera (`findWaitingTime`)

O código cria uma cópia das necessidades de CPU de cada processo no vetor `rem_bt` (tempos de *burst* restantes) e inicializa um relógio global `t` a `0`.
Utiliza um ciclo `while (true)` que percorre repetidamente todos os processos na ordem em que foram apresentados:

* **Se o processo precisa de mais tempo que o *quantum*:** O relógio global avança o valor do *quantum* e a necessidade restante do processo é decrementada na mesma proporção.
* **Se o processo precisa de um tempo menor ou igual ao *quantum*:** O relógio global avança apenas a quantidade restante exata que o processo necessita. O processo conclui a sua execução, o seu tempo de espera (`wt`) é calculado como `(tempo_atual - burst_time_original)` e o seu tempo restante passa a `0`.
O ciclo apenas termina quando nenhum processo tiver tempo restante pendente (`done = true`).

### 2. Cálculo do Tempo de Retorno (`findTurnAroundTime`)

O tempo de retorno (*Turnaround Time*) de cada processo é calculado de forma direta, bastando somar o tempo total que o processo precisou para executar (*burst time*) ao tempo que passou à espera na fila (*waiting time*).

---

## Cenário de Teste Incluído

A função `main` contém um ambiente de testes já configurado com dados predefinidos para validar o simulador:

* **Processos:** 3 processos identificados como `1`, `2` e `3`.
* **Burst Time (Tempo de CPU):** `10` unidades para o Processo 1, `5` unidades para o Processo 2, e `8` unidades para o Processo 3.
* **Quantum:** `2` unidades de tempo por ciclo.

Ao executar a aplicação, o programa imprimirá no ecrã:

1. Uma tabela com as métricas individuais de cada processo (Identificador, *Burst Time*, Tempo de Espera e Tempo de Retorno).
2. O **Tempo médio de espera** e o **Tempo médio de retorno** totais do lote de processos.
