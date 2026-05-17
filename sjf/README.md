# Simulador de Escalonamento SRTN (C)

Este projeto implementa um simulador do algoritmo de **Escalonamento SRTN** (*Shortest Remaining Time Next*) na linguagem C. O SRTN é essencialmente a versão **preemptiva** do algoritmo *Shortest Job First* (SJF).

A lógica central deste modelo define que, a cada unidade de tempo (tick), o escalonador avalia todos os processos que já chegaram à fila e seleciona aquele que tem o **menor tempo de execução restante** para ocupar a CPU. Se um novo processo chegar com um tempo necessário menor do que o tempo que resta ao processo atualmente em execução, este último é interrompido (preempção) para dar lugar ao novo.

---

## Como Compilar e Executar

O código foi desenvolvido em C e utiliza apenas bibliotecas padrão (`<stdio.h>`, `<stdlib.h>`, `<limits.h>`).

Para compilar o código fonte no terminal, utilize o compilador `gcc`:

```bash
gcc -o srtn srtn.c

```

Para executar a aplicação compilada:

```bash
./srtn

```

---

## Estrutura e Funcionamento do Simulador

### 1. Entrada de Dados

O programa é interativo e pede ao utilizador que forneça os dados do cenário de simulação. Suporta até um máximo de **100 processos** simultâneos.
O utilizador deve introduzir passo a passo:

1. O número total de processos.
2. O **Tempo de Chegada** (*Arrival Time*) de cada processo.
3. O **Tempo de Duração** (*Burst Time* inicial) de cada processo.

### 2. Simulação Tick a Tick

A simulação principal ocorre num ciclo iterativo que avança unidade a unidade de tempo (`for (time = 0; count != n; time++)`):

* Em cada instante (`time`), o algoritmo verifica todos os processos onde o tempo de chegada seja menor ou igual ao tempo atual (`arrival[i] <= time`) e que ainda não tenham terminado (`burst[i] > 0`).
* De entre esses, seleciona o processo com a menor duração pendente (`burst[smallest]`) e decrementa o seu tempo restante em 1 unidade.
* Se nenhum processo tiver chegado ainda, o sistema regista que a CPU está ociosa nesse instante e avança o relógio.
* Quando o tempo restante de um processo chega a zero (`burst[smallest] == 0`), o processo é considerado concluído. O simulador regista o **Tempo de Conclusão** e efetua os cálculos das métricas finais desse processo.

### 3. Cálculo das Métricas

Após a conclusão de cada tarefa, o sistema calcula:

* **Tempo de Resposta / Retorno (*Turnaround*):** A diferença entre o momento em que o processo terminou e o momento em que chegou (`completion - arrival`).
* **Tempo de Espera (*Waiting*):** O tempo total que o processo passou à espera, calculado subtraindo a sua duração original ao seu tempo de resposta (`turnaround - original_burst`).

---

## Resultados da Simulação

No final da execução, quando todos os processos tiverem sido processados, o simulador imprime no ecrã uma tabela consolidada contendo:

* O Identificador do Processo (pID).
* O Tempo de Duração original.
* O Tempo de Chegada.
* O Tempo de Espera e Tempo de Resposta (calculados).
* O Tempo de Conclusão exato no relógio do sistema.

Adicionalmente, apresenta as métricas globais de eficiência do sistema: o **Tempo médio de espera** e o **Tempo médio de resposta** globais.
