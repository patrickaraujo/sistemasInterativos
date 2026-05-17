/*
 * Priority Scheduling em C (nao-preemptivo)
 *
 * Este programa simula o algoritmo de escalonamento por prioridade.
 *
 * Ideia principal:
 * - Cada processo possui:
 *   - pid: identificador do processo;
 *   - burst_time: tempo de CPU necessario;
 *   - priority: prioridade do processo.
 *
 * Convencao usada neste programa:
 * - Quanto MAIOR o valor de priority, MAIOR a prioridade.
 *
 * Como o algoritmo aqui eh nao-preemptivo:
 * - Depois que um processo comeca a executar, ele roda ate terminar.
 * - Nao existe interrupcao no meio da execucao.
 *
 * Compilar:
 *   gcc -O2 -std=c11 -Wall -Wextra priority.c -o priority
 *
 * Executar:
 *   ./priority
 *
 * O programa tambem cria um arquivo log.txt com a mesma saida exibida
 * no terminal.
 */

#include <stdio.h>
#include <stdlib.h>

#define N 3

/*
 * Estrutura que representa um processo.
 *
 * pid:
 *   identificador do processo.
 *
 * burst_time:
 *   tempo total de CPU que o processo precisa para terminar.
 *
 * priority:
 *   prioridade do processo. Neste exemplo, maior numero significa
 *   maior prioridade.
 */
typedef struct {
    int pid;
    int burst_time;
    int priority;
} Process;

/*
 * Imprime uma mensagem tanto no terminal quanto no arquivo de log.
 *
 * Isso evita duplicar codigo toda vez que queremos registrar uma saida.
 */
static void print_line(FILE *log_file, const char *text) {
    printf("%s", text);

    if (log_file != NULL) {
        fprintf(log_file, "%s", text);
    }
}

/*
 * Imprime uma linha formatada tanto no terminal quanto no arquivo de log.
 *
 * Neste programa, usamos essa funcao para imprimir a tabela final
 * com burst time, waiting time e turnaround time.
 */
static void print_process_line(
    FILE *log_file,
    int pid,
    int burst_time,
    int waiting_time,
    int turnaround_time
) {
    printf("   %d\t\t%d\t\t%d\t\t%d\n",
           pid, burst_time, waiting_time, turnaround_time);

    if (log_file != NULL) {
        fprintf(log_file, "   %d\t\t%d\t\t%d\t\t%d\n",
                pid, burst_time, waiting_time, turnaround_time);
    }
}

/*
 * Ordena os processos em ordem decrescente de prioridade.
 *
 * Exemplo:
 *   priority 3 vem antes de priority 2.
 *
 * Em caso de empate, o processo com menor pid vem primeiro.
 *
 * Para manter o codigo simples e didatico, foi usado bubble sort.
 * Como a quantidade de processos neste exemplo eh pequena, isso nao
 * causa problema.
 */
static void sort_by_priority(Process proc[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            int should_swap = 0;

            if (proc[j].priority < proc[j + 1].priority) {
                should_swap = 1;
            } else if (
                proc[j].priority == proc[j + 1].priority &&
                proc[j].pid > proc[j + 1].pid
            ) {
                should_swap = 1;
            }

            if (should_swap) {
                Process temp = proc[j];
                proc[j] = proc[j + 1];
                proc[j + 1] = temp;
            }
        }
    }
}

/*
 * Calcula o waiting time de cada processo.
 *
 * Waiting time eh o tempo que um processo espera antes de comecar
 * a executar.
 *
 * Como o escalonamento eh nao-preemptivo, o primeiro processo da fila
 * nao espera nada. O segundo espera o burst do primeiro. O terceiro
 * espera o burst do primeiro + o burst do segundo, e assim por diante.
 */
static void find_waiting_time(const Process proc[], int waiting_time[], int n) {
    waiting_time[0] = 0;

    for (int i = 1; i < n; i++) {
        waiting_time[i] = proc[i - 1].burst_time + waiting_time[i - 1];
    }
}

/*
 * Calcula o turnaround time de cada processo.
 *
 * Turnaround time eh o tempo total desde que o processo entra na fila
 * ate terminar sua execucao.
 *
 * Neste exemplo simples, todos os processos estao disponiveis desde o
 * inicio. Por isso:
 *
 *   turnaround time = waiting time + burst time
 */
static void find_turnaround_time(
    const Process proc[],
    const int waiting_time[],
    int turnaround_time[],
    int n
) {
    for (int i = 0; i < n; i++) {
        turnaround_time[i] = proc[i].burst_time + waiting_time[i];
    }
}

/*
 * Calcula e imprime:
 * - waiting time de cada processo;
 * - turnaround time de cada processo;
 * - tempo medio de espera;
 * - tempo medio de retorno.
 */
static void find_average_time(const Process proc[], int n, FILE *log_file) {
    int waiting_time[N] = {0};
    int turnaround_time[N] = {0};

    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    find_waiting_time(proc, waiting_time, n);
    find_turnaround_time(proc, waiting_time, turnaround_time, n);

    print_line(log_file, "\nProcesso\tBurst time\tWaiting time\tTurn around time\n");

    for (int i = 0; i < n; i++) {
        total_waiting_time += waiting_time[i];
        total_turnaround_time += turnaround_time[i];

        print_process_line(
            log_file,
            proc[i].pid,
            proc[i].burst_time,
            waiting_time[i],
            turnaround_time[i]
        );
    }

    double avg_waiting = (double) total_waiting_time / n;
    double avg_turnaround = (double) total_turnaround_time / n;

    printf("\nTempo medio de espera   = %.2f", avg_waiting);
    printf("\nTempo medio de retorno  = %.2f\n", avg_turnaround);

    if (log_file != NULL) {
        fprintf(log_file, "\nTempo medio de espera   = %.2f", avg_waiting);
        fprintf(log_file, "\nTempo medio de retorno  = %.2f\n", avg_turnaround);
    }
}

/*
 * Funcao principal do escalonador por prioridade.
 *
 * Passos:
 * 1. Ordena os processos por prioridade.
 * 2. Mostra a ordem de execucao.
 * 3. Calcula waiting time e turnaround time.
 * 4. Mostra as medias.
 */
static void priority_scheduling(Process proc[], int n, FILE *log_file) {
    sort_by_priority(proc, n);

    print_line(log_file, "Ordem de execucao dos processos: ");

    for (int i = 0; i < n; i++) {
        printf("%d ", proc[i].pid);

        if (log_file != NULL) {
            fprintf(log_file, "%d ", proc[i].pid);
        }
    }

    print_line(log_file, "\n");

    find_average_time(proc, n, log_file);
}

int main(void) {
    /*
     * Conjunto de processos usado no exemplo.
     *
     * Formato:
     *   {pid, burst_time, priority}
     *
     * Quanto maior o valor de priority, mais cedo o processo sera
     * escolhido pelo escalonador.
     */
    Process processes[N] = {
        {1, 10, 2},
        {2,  5, 0},
        {3,  8, 1}
    };

    FILE *log_file = fopen("log.txt", "w");

    if (log_file == NULL) {
        fprintf(stderr, "Aviso: nao foi possivel criar log.txt. ");
        fprintf(stderr, "A saida sera exibida apenas no terminal.\n");
    }

    print_line(log_file, "Priority Scheduling nao-preemptivo\n");
    print_line(log_file, "Maior numero = maior prioridade\n\n");

    print_line(log_file, "Estado inicial dos processos:\n");
    print_line(log_file, "Processo\tBurst time\tPrioridade\n");

    for (int i = 0; i < N; i++) {
        printf("   %d\t\t%d\t\t%d\n",
               processes[i].pid,
               processes[i].burst_time,
               processes[i].priority);

        if (log_file != NULL) {
            fprintf(log_file, "   %d\t\t%d\t\t%d\n",
                    processes[i].pid,
                    processes[i].burst_time,
                    processes[i].priority);
        }
    }

    print_line(log_file, "\n");

    priority_scheduling(processes, N, log_file);

    if (log_file != NULL) {
        fclose(log_file);
    }

    return 0;
}
