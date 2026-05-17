# Lottery Scheduling em C

Este projeto é uma versão em C do arquivo `Lottery.java`. O programa simula o algoritmo de escalonamento chamado **Lottery Scheduling**.

No Lottery Scheduling, cada processo recebe uma quantidade de **tickets**. A cada rodada, o escalonador sorteia um ticket. O processo dono do ticket sorteado ganha o direito de executar por um intervalo de tempo chamado **quantum**.

Quanto mais tickets um processo possui, maior é sua chance de ser escolhido.

## Arquivos

- `lottery.c`: implementação em C do simulador.
- `README_lottery.md`: explicação do código, compilação e execução.
- `Lottery.java`: versão Java original usada como base.

## Como compilar

Use o comando abaixo:

```bash
gcc -O2 -std=c11 -Wall -Wextra lottery.c -o lottery
```

## Como executar

```bash
./lottery
```

A execução imprime a simulação no terminal e também cria um arquivo chamado:

```bash
log.txt
```

Esse arquivo contém o mesmo resultado mostrado na tela.

## Como executar com seed fixa

Por padrão, o programa usa uma seed baseada no horário atual. Por isso, cada execução pode gerar processos diferentes.

Para repetir sempre o mesmo resultado, use uma seed fixa:

```bash
./lottery --seed 42
```

## O que o programa faz

O programa cria 10 processos, identificados pelas letras:

```text
a, b, c, d, e, f, g, h, i, j
```

Para cada processo, são gerados aleatoriamente:

- tempo de chegada ao sistema;
- quantidade de CPU solicitada.

Depois disso, os processos são ordenados pelo tempo de chegada.

Em seguida, o algoritmo de escalonamento começa.

## Principais conceitos

### Processo

Cada processo possui três informações principais:

```c
typedef struct {
    char name;
    int arrival;
    int request;
} Process;
```

- `name`: nome do processo, como `a`, `b` ou `c`.
- `arrival`: tempo em que o processo chega ao sistema.
- `request`: quantidade restante de CPU que o processo ainda precisa consumir.

Quando `request` chega a zero, o processo terminou.

### Quantum

O quantum é o máximo de tempo que um processo pode executar em uma rodada.

Exemplo: se o quantum for 5 e o processo ainda precisar de 12 unidades de CPU, ele executa 5 unidades e depois volta a disputar o sorteio.

Se o processo precisar de apenas 3 unidades, ele executa 3 e termina.

### Tickets

Os tickets representam a chance de um processo ser escolhido.

A regra usada no código é:

```text
tickets = max(1, request / 2)
```

Isso significa que processos com maior demanda restante de CPU recebem mais tickets. Mesmo assim, todo processo pronto recebe pelo menos 1 ticket.

### Processo pronto

Um processo só participa do sorteio se:

- já chegou ao sistema;
- ainda possui `request > 0`.

Ou seja, processos que ainda não chegaram não podem ser escolhidos.

## Fluxo do algoritmo

O loop principal funciona assim:

1. Verifica quais processos já chegaram.
2. Calcula os tickets dos processos prontos.
3. Soma o total de tickets.
4. Sorteia um ticket aleatório.
5. Descobre qual processo é dono desse ticket.
6. Executa o processo vencedor por um quantum ou até ele terminar.
7. Atualiza o tempo atual da simulação.
8. Repete até todos os processos terminarem.

## Exemplo da saída

A saída possui uma primeira parte com o estado inicial:

```text
Estado inicial:
    process: a    arrival:  3    request: 10
    process: b    arrival:  7    request:  5
quantum: 4
```

Depois, cada linha mostra uma rodada do escalonamento:

```text
  time    winner   request   tickets     total
     7         a         6         5         5
    11         b         1         2         5
```

Significado das colunas:

- `time`: tempo atual após a execução da rodada.
- `winner`: processo vencedor do sorteio.
- `request`: demanda restante do processo vencedor após executar.
- `tickets`: quantidade de tickets do processo vencedor naquela rodada.
- `total`: total de tickets disponíveis no sorteio.

## Diferença em relação ao Java original

A lógica principal foi preservada:

- geração de processos aleatórios;
- ordenação por tempo de chegada;
- cálculo de tickets proporcional ao `request` restante;
- sorteio do processo vencedor;
- execução por quantum;
- finalização quando todos os processos chegam a `request = 0`.

A versão em C adiciona alguns recursos didáticos:

- comentários explicativos no código;
- estrutura `Process` para organizar melhor os dados;
- opção `--seed` para repetir a mesma simulação;
- gravação automática da saída em `log.txt`.
