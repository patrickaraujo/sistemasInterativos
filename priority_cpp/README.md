# Simulador de Escalonamento por Prioridade (C++)

Este projeto implementa um simulador do algoritmo de **Escalonamento por Prioridade** (Priority Scheduling) na linguagem C++. Esta versão específica do algoritmo é **não-preemptiva**, o que significa que, uma vez que um processo ganha acesso à CPU, este executa até à sua conclusão, não sendo interrompido por outros processos.

A convenção adotada neste código estabelece que **números maiores representam uma prioridade mais alta**.

---

## Como Compilar e Executar

O código foi desenvolvido utilizando a norma C++17 e a biblioteca padrão (`<iostream>`, `<algorithm>`, `<vector>`). 

Para compilar o código fonte no terminal, utilize o compilador `g++`:

```bash
g++ -std=c++17 -Wall -o priority priority.cpp

```

Para executar o programa gerado:

```bash
./priority

```

---

## Estrutura e Funcionamento do Simulador

### 1. Representação do Processo

Cada processo é gerido através de uma estrutura (`struct Process`) que armazena as seguintes propriedades fundamentais:

* **`pid`:** O identificador único do processo (ID).
* **`bt`:** O *Burst Time*, ou seja, o tempo total de CPU que o processo necessita para concluir a sua execução.
* **`priority`:** O nível de prioridade do processo (onde o valor mais alto indica a maior prioridade).

### 2. Lógica de Ordenação

A função principal do algoritmo, `priorityScheduling`, começa por ordenar o vetor de processos. A ordenação é feita utilizando a função `std::sort` combinada com uma função de comparação customizada (`comparison`).
Esta ordenação garante que o vetor fica organizado por ordem decrescente de prioridade, definindo assim a **ordem exata de execução**.

### 3. Cálculo de Tempos

Como o algoritmo é não-preemptivo e os processos já se encontram ordenados, o simulador calcula os tempos da seguinte forma:

* **Waiting Time (Tempo de Espera):** O tempo de espera do primeiro processo é sempre `0`. Para os processos seguintes, o tempo de espera é calculado somando o *burst time* do processo anterior ao tempo de espera do processo anterior (`wt[i] = proc[i - 1].bt + wt[i - 1]`).
* **Turnaround Time (Tempo de Retorno):** É o tempo total desde a chegada (considerada `0` para todos neste exemplo base) até à finalização. Calcula-se somando o *burst time* do processo ao seu *waiting time* (`tat[i] = proc[i].bt + wt[i]`).

---

## Cenário de Teste Incluído

O simulador inclui na função `main` um cenário base predefinido para demonstração do algoritmo. Estão instanciados três processos com os seguintes atributos:

1. **Processo 1:** Burst Time = 10, Prioridade = 2
2. **Processo 2:** Burst Time = 5, Prioridade = 0
3. **Processo 3:** Burst Time = 8, Prioridade = 1

Ao executar, o programa imprimirá no ecrã:

1. A ordem final de execução dos processos após a ordenação por prioridade.
2. Uma tabela detalhada com o *Burst Time*, *Waiting Time* e *Turnaround Time* para cada processo.
3. O cálculo final do **tempo médio de espera** e do **tempo médio de retorno** de todo o conjunto.
