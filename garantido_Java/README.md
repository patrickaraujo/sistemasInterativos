# Escalonamento Garantido em Java

Este projeto implementa um simulador de escalonamento de processos em Java, com interface gráfica feita em Swing. O objetivo é demonstrar o funcionamento de um escalonador inspirado no conceito de **Escalonamento Garantido**, em que cada processo deve receber uma fração justa do tempo de CPU ao longo da execução.

A simulação representa uma CPU executando um processo por vez. O usuário cria processos pela interface, informa o tempo de surto de cada processo e inicia o escalonamento. Durante a execução, a aplicação atualiza a tabela de processos, o tempo total, o uso de memória, os estados dos processos e os valores de quantum.

## Funcionalidades

- Criação manual de processos pela interface gráfica.
- Execução de processos com tempo de surto definido pelo usuário.
- Simulação de CPU com quantum de execução.
- Controle dos estados dos processos:
  - `NOVO`
  - `PRONTO`
  - `SUSPENSO`
  - `EXECUTANDO`
  - `FINALIZADO`
- Fila de processos prontos.
- Fila de processos suspensos.
- Alocação e liberação de memória.
- Atualização visual da execução em uma tabela Swing.
- Suspensão, retomada e finalização manual de processos.

## Estrutura do Projeto

```text
src/
├── controller/
│   ├── AbstractEscalonador.java
│   └── Escalonador.java
│
├── model/
│   ├── AbstractFactory.java
│   ├── AbstractFilaProcesso.java
│   ├── AbstractMemoria.java
│   ├── AbstractProcessador.java
│   ├── BlocoMemoria.java
│   ├── FilaEntrada.java
│   ├── FilaProcessos.java
│   ├── Memoria.java
│   ├── ModeloTabelaProcessos.java
│   ├── Processador.java
│   ├── Processo.java
│   └── ProcessoFactory.java
│
└── view/
    ├── TelaJFrame.java
    └── TesteView.java
```

## Principais Classes

### `TesteView`

Classe principal da aplicação. Ela cria a janela gráfica, exibe a interface Swing e inicia o escalonador.

### `TelaJFrame`

Responsável pela interface gráfica. A tela permite criar processos, iniciar o escalonamento, suspender processos, retomar processos suspensos e finalizar o processo em execução.

### `Escalonador`

Controla a lógica principal do escalonamento. Ele gerencia a fila de entrada, a fila de processos prontos, a fila de processos suspensos, a CPU e a memória.

### `Processador`

Simula a CPU. A classe executa um processo por vez, incrementando o tempo de processamento e decrementando o surto do processo até que o quantum termine ou o processo finalize.

### `Processo`

Representa um processo da simulação. Cada processo possui identificador, nome, tamanho em memória, prioridade, estado, surto, quantum e tempo de processamento.

### `Memoria`

Simula a memória principal. A memória possui 32 blocos, cada um com 4 MB, totalizando 128 MB. Cada processo criado ocupa 8 MB.

### `FilaProcessos`

Representa a fila de processos prontos. Internamente, utiliza uma `PriorityQueue`, permitindo que os processos sejam organizados de acordo com a prioridade.

### `FilaEntrada`

Representa a fila inicial de processos criados pelo usuário antes de serem colocados na fila de prontos.

## Requisitos

- Java JDK 8 ou superior.
- Ambiente com suporte a interface gráfica, pois o projeto usa Java Swing.

## Como Compilar e Executar

A classe principal do projeto é `view.TesteView`.

A partir da raiz do projeto, execute:

```bash
mkdir -p build
javac -encoding UTF-8 -d build src/controller/*.java src/model/*.java src/view/*.java
java -cp build view.TesteView
```

No Windows, caso esteja usando o Prompt de Comando, use:

```bat
mkdir build
javac -encoding UTF-8 -d build src\controller\*.java src\model\*.java src\view\*.java
java -cp build view.TesteView
```

## Como Usar a Interface

1. Clique em **Criar**.
2. Informe o tempo de surto do processo.
3. Repita o processo para criar quantos processos desejar.
4. Clique em **Escalonar** para iniciar a execução.
5. Use **Suspender** para suspender o processo em execução.
6. Use **Prosseguir** para retornar um processo suspenso para a fila de prontos.
7. Use **Finalizar** para finalizar manualmente o processo atualmente em execução.

## Funcionamento do Escalonamento

Quando o usuário inicia o escalonamento, os processos da fila de entrada são movidos para a fila de prontos, desde que exista espaço disponível na memória.

O escalonador seleciona um processo da fila de prontos e o envia para o processador. O processador executa esse processo até que uma das seguintes condições ocorra:

- o processo consome todo o quantum disponível;
- o tempo de surto do processo chega a zero.

Se o processo ainda não terminou, ele volta para a fila de prontos. Caso contrário, seu estado passa para `FINALIZADO` e a memória ocupada por ele é liberada.

## Quantum

O quantum é definido dinamicamente com base na quantidade de processos prontos. A ideia é aproximar o comportamento do escalonamento garantido, no qual cada processo deve receber uma fração justa da CPU.

No código atual, o quantum é calculado com limite mínimo de 2 e limite máximo de 10. Quanto maior a quantidade de processos prontos, menor tende a ser o quantum por rodada.

## Prioridade dos Processos

Após cada ciclo de execução, o escalonador recalcula a prioridade dos processos prontos. Esse cálculo considera a relação entre o tempo de processamento de cada processo e a média de tempo dos processos na fila.

A prioridade pode ser:

- `ALTA`
- `MEDIA`
- `BAIXA`

Processos que receberam menos tempo de CPU em relação à média tendem a ganhar prioridade maior, enquanto processos que já receberam mais tempo tendem a ter prioridade reduzida.

## Memória

A memória simulada possui 32 blocos de 4 MB cada, totalizando 128 MB. Cada processo criado pela `ProcessoFactory` ocupa 8 MB.

Quando um processo é colocado na fila de prontos, ele também é armazenado na memória. Quando finaliza, seus blocos de memória são liberados.

## Observações Importantes

Embora a classe `Processo` estenda `Thread`, os processos não são executados como threads independentes do sistema operacional. A execução é simulada pelo objeto `Processador`, que usa `Thread.sleep(1000)` para representar a passagem do tempo lógico.

Portanto, o projeto deve ser entendido como uma simulação didática de escalonamento, e não como uma implementação real de escalonamento concorrente de processos do sistema operacional.

## Exemplo de Fluxo

Um fluxo típico de uso seria:

1. Criar três processos com surtos diferentes, por exemplo `5`, `8` e `12`.
2. Clicar em **Escalonar**.
3. Observar a tabela enquanto os processos alternam entre os estados `PRONTO`, `EXECUTANDO` e `FINALIZADO`.
4. Suspender ou finalizar processos manualmente, se desejar testar esses comportamentos.

## Objetivo Didático

O projeto pode ser usado para estudar conceitos básicos de sistemas operacionais, especialmente:

- escalonamento de processos;
- quantum;
- tempo de surto;
- fila de prontos;
- troca de contexto simulada;
- estados de processos;
- alocação simples de memória;
- escalonamento justo baseado em tempo de CPU recebido.
