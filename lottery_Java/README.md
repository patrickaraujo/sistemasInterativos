# Simulador de Escalonamento por Lotaria (Java)

Este projeto implementa um simulador simples do algoritmo de **Escalonamento por Lotaria** (Lottery Scheduling) na linguagem Java. Neste modelo estatístico, o processador é partilhado através de um sistema de sorteio: cada processo recebe um determinado número de "bilhetes" (tickets) proporcional à sua necessidade de CPU. A cada ciclo, um bilhete é sorteado aleatoriamente e o processo vencedor ganha o direito de executar durante um determinado intervalo de tempo (*quantum*).

---

## Como Compilar e Executar

O código fonte está inserido no pacote `lottery`. Para executar o programa a partir do terminal, utilize os seguintes comandos:

**Para compilar:**
```bash
javac -d . Lottery.java

```

**Para executar:**

```bash
java lottery.Lottery

```

---

## Estrutura e Funcionamento do Simulador

### 1. Inicialização e Configuração

Ao iniciar, o programa gera automaticamente o cenário de simulação para **10 processos**, identificados sequencialmente pelas letras minúsculas de `a` até `j`.
As seguintes propriedades são geradas de forma aleatória:

* **Tempo de Chegada (`arrival`):** Sorteado entre 0 e 29 unidades de tempo.
* **Demanda de CPU (`request`):** Sorteada entre 1 e 19 unidades de tempo (se o valor gerado for 0, é ajustado para 5, evitando processos vazios).
* **Quantum:** Sorteado até um máximo de 8 unidades de tempo (se for sorteado o valor 0, é ajustado por defeito para 5).

Após a geração dos dados, os processos são ordenados cronologicamente pelo seu tempo de chegada utilizando um algoritmo de *Bubble Sort* estável.

### 2. Distribuição de Bilhetes (Tickets)

A principal característica deste algoritmo é a atribuição de bilhetes dinâmicos. Em cada ciclo do processador, apenas os processos que já chegaram à fila (`arrival <= time`) e que ainda não terminaram o seu trabalho (`request > 0`) entram no sorteio.

A quantidade de bilhetes atribuída a cada processo baseia-se na sua demanda restante:

```java
tickets[i] = Math.max(1, request[i] / 2);

```

Isto significa que os processos que necessitam de mais tempo de CPU recebem mais bilhetes, aumentando a sua probabilidade de ganhar, mas todo o processo ativo recebe, no mínimo, 1 bilhete, garantindo que não ocorre *starvation* absoluto.

### 3. Sorteio e Execução

Se existirem processos na fila, o simulador avança da seguinte forma:

1. Calcula a soma total dos bilhetes em jogo (`totalTickets`).
2. Sorteia um valor aleatório entre `0` e `totalTickets - 1`.
3. Itera pelas parcelas de bilhetes de cada processo para encontrar o "vencedor" do sorteio.
4. O processo vencedor é executado pelo tempo limite do seu *quantum* ou até terminar o seu `request` total, caso necessite de menos tempo que o *quantum* definido (`Math.min(quantum, request[winner])`).
5. Avança o tempo global com base no tempo efetivamente consumido e reduz o `request` do vencedor.

Caso o processador fique ocioso (nenhum processo com demanda > 0 chegou ainda), o sistema avança automaticamente o tempo global para o instante de chegada do próximo processo agendado.

---

## Análise da Saída no Terminal

Durante a execução, o simulador imprime primeiro o **Estado Inicial** de todos os processos para referência.

De seguida, exibe uma tabela de progressão em que cada linha representa um sorteio e a respetiva execução. As colunas apresentadas são:

* **`time`**: O instante de tempo após a execução do processo vencedor.
* **`winner`**: O processo (letra) sorteado.
* **`request`**: O tempo que o processo ainda precisa processar após essa ronda.
* **`tickets`**: O número de bilhetes que este processo detinha na altura do sorteio.
* **`total`**: O número global de bilhetes de todos os processos disponíveis nesse exato momento.

O ciclo (`while`) termina quando a demanda somada de todos os processos atinge zero, exibindo uma mensagem de encerramento com o tempo total dispendido pela CPU.
