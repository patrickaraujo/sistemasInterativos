# Simulador de Escalonamento Fair-Share em Java

Este projeto implementa um simulador de escalonamento de CPU em Java. O objetivo é demonstrar o comportamento do algoritmo **Fair-Share Scheduler (FSS)** e compará-lo com um modo de escalonamento baseado apenas na prioridade individual dos processos.

A simulação usa uma thread para representar o processador. A CPU executa um processo por vez durante um quantum fixo. Quando o quantum termina ou o processo atual finaliza, ocorre uma troca de contexto e um novo processo é escolhido a partir da fila de prontos.

---

## Estrutura do Projeto

```text
fair-share_Java/
├── README.md
└── src/
    ├── scheduler/
    │   ├── FSSLogger.java
    │   ├── IdleProcess.java
    │   ├── Process.java
    │   ├── Processor.java
    │   ├── ReadyQueue.java
    │   └── ShareGroup.java
    └── tester/
        └── ScheduleTest.java
```

O código está dividido em dois pacotes principais:

- `scheduler`: contém o motor de escalonamento.
- `tester`: contém a classe de teste responsável por executar a simulação.

---

## Componentes Principais

### `Processor.java`

Representa a CPU simulada. A classe estende `Thread` e executa o loop principal da simulação, avançando o tempo lógico em ciclos.

O processador usa um quantum fixo de 50 ciclos. Quando esse limite é atingido ou o processo atual termina, o método `taskSwitch()` realiza a troca de contexto.

### `ReadyQueue.java`

Representa a fila de processos prontos. A implementação usa `PriorityBlockingQueue`, permitindo acesso seguro em um cenário com múltiplas threads.

A ordenação da fila depende do modo de escalonamento escolhido:

- modo FSS: usa `ProcessGroupComparator`;
- modo sistema: usa `ProcessSystemComparator`.

### `Process.java`

Representa uma tarefa do sistema. Cada processo possui um tempo de execução, chamado de `burst`, gerado aleatoriamente entre 10 e 299 ciclos.

A classe também calcula a prioridade individual do processo com base no histórico de execução.

### `IdleProcess.java`

Representa um processo especial usado quando não há tarefas prontas na fila.

Ele permite contabilizar corretamente o tempo ocioso da CPU durante a simulação.

### `ShareGroup.java`

Agrupa processos sob uma cota de CPU. Cada grupo possui uma parcela esperada de uso do processador, como 75% ou 25%.

A prioridade do grupo é calculada a partir da diferença entre o uso real da CPU e a cota esperada. Assim, grupos que consumiram menos do que sua parcela prevista tendem a ganhar prioridade no modo Fair-Share.

### `FSSLogger.java`

Registra informações da simulação no arquivo `log.txt`.

A cada 1000 ciclos lógicos, o logger calcula o percentual de CPU usado por cada grupo e salva uma nova linha quando há mudança nos valores observados.

### `ScheduleTest.java`

É a classe principal do projeto. Ela cria os grupos, gera os processos, inicia a CPU simulada e imprime as estatísticas finais da execução.

---

## Experimento Base

O experimento configurado em `ScheduleTest.java` cria dois grupos:

1. `users`, com cota de 75% da CPU;
2. `admin`, com cota de 25% da CPU.

Para testar o comportamento do escalonador, a carga de trabalho é propositalmente desbalanceada:

- o grupo `users` recebe 1.500 processos;
- o grupo `admin` recebe 3.000 processos.

Mesmo com mais processos no grupo `admin`, o modo Fair-Share tenta respeitar a divisão de CPU definida pelas cotas dos grupos.

Os processos são adicionados de forma gradual à CPU usando filas auxiliares (`ArrayBlockingQueue`) e pausas com `Thread.sleep()`. Isso simula a chegada dinâmica de tarefas ao longo do tempo.

---

## Como Executar

A classe principal é `ScheduleTest.java`, localizada no pacote `tester`.

A partir da raiz do projeto, compile com:

```bash
mkdir -p build
javac -d build src/scheduler/*.java src/tester/*.java
```

Depois, execute com:

```bash
java -cp build tester.ScheduleTest
```

Ao final da execução, o programa exibirá estatísticas no terminal e criará o arquivo `log.txt` na raiz do projeto.

Observação: a execução completa pode levar algum tempo, pois o teste cria milhares de processos e usa pausas aleatórias para simular chegadas dinâmicas.

---

## Alternando os Modos de Escalonamento

O modo de escalonamento é definido na criação do processador, dentro de `ScheduleTest.java`:

```java
Processor cpu = new Processor(true);
```

Use `true` para executar no modo Fair-Share Scheduler:

```java
Processor cpu = new Processor(true);
```

Nesse modo, a fila usa `ProcessGroupComparator`, considerando a prioridade do grupo e tentando aproximar o uso real da CPU das cotas definidas.

Use `false` para executar no modo sistema:

```java
Processor cpu = new Processor(false);
```

Nesse modo, a fila usa `ProcessSystemComparator`, considerando apenas a prioridade individual dos processos e ignorando as cotas dos grupos.

---

## Saída da Simulação

Ao fim do experimento, o programa imprime informações como:

- tempo ocupado da CPU;
- tempo ocioso da CPU;
- total de ciclos executados por grupo;
- quantidade total de processos registrados;
- tempo real gasto pela execução do teste.

Exemplo de saída no console:

```text
Busy time: ... , idle time: ...
Admin group total: ...
User group total: ...
Total number of processes: ...
Tester time: ...
Total time: ...
Finished
```

Além disso, o arquivo `log.txt` registra snapshots do uso percentual da CPU por grupo ao longo do tempo lógico da simulação.

---

## Observações

- O projeto simula o escalonamento de processos, mas não executa vários processos reais em paralelo.
- A thread principal injeta processos na simulação, enquanto a thread `Processor` representa a CPU simulada.
- O objetivo do código é didático: mostrar a diferença entre um escalonador baseado em prioridade individual e um escalonador Fair-Share baseado em cotas de grupo.
