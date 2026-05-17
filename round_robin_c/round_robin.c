/*
 * Round Robin Scheduling em C
 *
 * Este programa é uma versão em C do arquivo round_Robin.cpp.
 *
 * O Round Robin é um algoritmo de escalonamento preemptivo: cada processo
 * recebe a CPU por um tempo fixo chamado quantum. Se o processo não terminar
 * dentro desse quantum, ele volta para o final da fila e espera sua próxima vez.
 *
 * Compilar:
 *   gcc -O2 -std=c11 -Wall -Wextra round_robin.c -o round_robin
 *
 * Executar:
 *   ./round_robin
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/* Quantidade de processos usados no exemplo. */
#define N 3

/*
 * Função auxiliar para imprimir ao mesmo tempo no terminal e no arquivo log.txt.
 *
 * Assim, o programa mostra o resultado na tela e também salva a simulação
 * para consulta posterior.
 */
static void print_both(FILE *log_file, const char *format, ...) {
    va_list args_stdout;
    va_list args_file;

    va_start(args_stdout, format);
    va_copy(args_file, args_stdout);

    vprintf(format, args_stdout);

    if (log_file != NULL) {
        vfprintf(log_file, format, args_file);
    }

    va_end(args_file);
    va_end(args_stdout);
}

/*
 * Verifica se ainda existe algum processo com burst time restante.
 *
 * Retorna 1 se todos terminaram.
 * Retorna 0 se ainda há pelo menos um processo pendente.
 */
static int all_done(const int remaining_burst[], int n) {
    for (int i = 0; i < n; i++) {
        if (remaining_burst[i] > 0) {
            return 0;
        }
    }

    return 1;
}

/*
 * Calcula o waiting time de todos os processos usando Round Robin.
 *
 * Parâmetros:
 * - burst_time: tempo total de CPU necessário para cada processo.
 * - waiting_time: vetor onde será salvo o tempo de espera de cada processo.
 * - quantum: fatia fixa de tempo que cada processo recebe por rodada.
 * - n: quantidade de processos.
 * - log_file: arquivo onde a simulação será registrada.
 *
 * Ideia principal:
 * - remaining_burst guarda quanto tempo ainda falta para cada processo terminar.
 * - O laço percorre os processos repetidamente.
 * - Se um processo ainda não terminou, ele executa por no máximo um quantum.
 * - Quando um processo termina, seu waiting time pode ser calculado.
 *
 * Como todos os processos estão disponíveis desde o tempo 0, temos:
 *
 * waiting time = tempo de término - burst time original
 */
static void find_waiting_time(
    const int burst_time[],
    int waiting_time[],
    int quantum,
    int n,
    FILE *log_file
) {
    int remaining_burst[N];
    int current_time = 0;
    int round = 1;

    /* Copia os burst times originais para o vetor de tempo restante. */
    for (int i = 0; i < n; i++) {
        remaining_burst[i] = burst_time[i];
        waiting_time[i] = 0;
    }

    print_both(log_file, "Linha do tempo da execução:\n");
    print_both(log_file, "---------------------------------------------------------\n");
    print_both(log_file, "%8s%12s%12s%16s\n", "Rodada", "Processo", "Executou", "Tempo atual");
    print_both(log_file, "---------------------------------------------------------\n");

    /* Repete até todos os processos terem remaining_burst igual a zero. */
    while (!all_done(remaining_burst, n)) {
        for (int i = 0; i < n; i++) {
            if (remaining_burst[i] <= 0) {
                continue;
            }

            int executed_time;

            /*
             * Caso 1: o processo precisa de mais tempo do que o quantum.
             * Ele executa exatamente um quantum e volta para a fila.
             */
            if (remaining_burst[i] > quantum) {
                executed_time = quantum;
                current_time += quantum;
                remaining_burst[i] -= quantum;
            } else {
                /*
                 * Caso 2: o processo termina nesta rodada.
                 * Ele executa apenas o tempo que ainda faltava.
                 */
                executed_time = remaining_burst[i];
                current_time += remaining_burst[i];
                remaining_burst[i] = 0;

                /*
                 * Quando o processo termina, o tempo de espera é o tempo total
                 * até a finalização menos o tempo que ele realmente executou.
                 */
                waiting_time[i] = current_time - burst_time[i];
            }

            print_both(
                log_file,
                "%8d%12d%12d%16d\n",
                round,
                i + 1,
                executed_time,
                current_time
            );
        }

        round++;
    }

    print_both(log_file, "---------------------------------------------------------\n\n");
}

/*
 * Calcula o turnaround time de cada processo.
 *
 * Turnaround time é o tempo total desde o início da simulação até o processo
 * terminar. Neste exemplo, como todos os processos chegam no tempo 0:
 *
 * turnaround time = burst time + waiting time
 */
static void find_turnaround_time(
    const int burst_time[],
    const int waiting_time[],
    int turnaround_time[],
    int n
) {
    for (int i = 0; i < n; i++) {
        turnaround_time[i] = burst_time[i] + waiting_time[i];
    }
}

/*
 * Executa o escalonamento Round Robin e imprime as métricas finais.
 *
 * Métricas calculadas:
 * - waiting time: quanto tempo o processo ficou esperando pela CPU.
 * - turnaround time: tempo total até o processo terminar.
 * - médias dessas duas métricas.
 */
static void find_average_time(
    const int processes[],
    const int burst_time[],
    int quantum,
    int n,
    FILE *log_file
) {
    int waiting_time[N];
    int turnaround_time[N];
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    print_both(log_file, "Round Robin Scheduling\n");
    print_both(log_file, "======================\n\n");

    print_both(log_file, "Estado inicial:\n");
    print_both(log_file, "Processo\tBurst time\n");
    for (int i = 0; i < n; i++) {
        print_both(log_file, "   %d\t\t%d\n", processes[i], burst_time[i]);
    }
    print_both(log_file, "\nQuantum = %d\n\n", quantum);

    find_waiting_time(burst_time, waiting_time, quantum, n, log_file);
    find_turnaround_time(burst_time, waiting_time, turnaround_time, n);

    print_both(log_file, "Resultado final:\n");
    print_both(log_file, "Processo\tBurst time\tWaiting time\tTurnaround time\n");

    for (int i = 0; i < n; i++) {
        total_waiting_time += waiting_time[i];
        total_turnaround_time += turnaround_time[i];

        print_both(
            log_file,
            "   %d\t\t%d\t\t%d\t\t%d\n",
            processes[i],
            burst_time[i],
            waiting_time[i],
            turnaround_time[i]
        );
    }

    print_both(
        log_file,
        "\nTempo medio de espera  = %.2f\n",
        (double) total_waiting_time / n
    );

    print_both(
        log_file,
        "Tempo medio de retorno = %.2f\n",
        (double) total_turnaround_time / n
    );
}

int main(void) {
    /*
     * Dados do exemplo original em C++.
     *
     * processes identifica os processos.
     * burst_time indica quanto tempo de CPU cada processo precisa.
     * quantum é a fatia fixa de CPU usada pelo Round Robin.
     */
    int processes[N] = {1, 2, 3};
    int burst_time[N] = {10, 5, 8};
    int quantum = 2;

    FILE *log_file = fopen("log.txt", "w");
    if (log_file == NULL) {
        fprintf(stderr, "Erro: nao foi possivel criar o arquivo log.txt\n");
        return EXIT_FAILURE;
    }

    find_average_time(processes, burst_time, quantum, N, log_file);

    fclose(log_file);
    return EXIT_SUCCESS;
}
