/*
 * Lottery Scheduling em C
 *
 * Este programa simula o algoritmo de escalonamento Lottery Scheduling.
 * A ideia principal é simples:
 *
 * 1. Cada processo possui uma demanda restante de CPU.
 * 2. A cada rodada, processos que já chegaram recebem tickets.
 * 3. Quanto maior a demanda restante de CPU, maior o número de tickets.
 * 4. Um ticket é sorteado aleatoriamente.
 * 5. O processo dono do ticket executa por um quantum de tempo.
 * 6. O processo termina quando sua demanda restante chega a zero.
 *
 * O programa imprime a simulação no terminal e também grava o resultado
 * em um arquivo chamado log.txt.
 *
 * Compilar:
 *   gcc -O2 -std=c11 -Wall -Wextra lottery.c -o lottery
 *
 * Executar:
 *   ./lottery
 *
 * Executar com seed fixa, para repetir o mesmo resultado:
 *   ./lottery --seed 42
 */

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 10          /* quantidade de processos */
#define MAX_ARRV 30   /* maior valor possível para o tempo de chegada */
#define MAX_REQ 20    /* maior valor possível para a demanda de CPU */
#define MAX_QUAN 8    /* maior valor possível para o quantum */

/*
 * Estrutura que representa um processo dentro da simulação.
 *
 * name:
 *   Nome simbólico do processo. Exemplo: 'a', 'b', 'c'.
 *
 * arrival:
 *   Momento em que o processo chega ao sistema e passa a poder disputar CPU.
 *
 * request:
 *   Quantidade restante de CPU que o processo ainda precisa consumir.
 *   Quando request chega a zero, o processo terminou.
 */
typedef struct {
    char name;
    int arrival;
    int request;
} Process;

/*
 * Escreve uma mensagem tanto no terminal quanto no arquivo de log.
 *
 * Isso evita duplicar vários printf/fprintf ao longo do código.
 */
static void log_printf(FILE *log_file, const char *format, ...) {
    va_list args_terminal;
    va_list args_file;

    va_start(args_terminal, format);
    va_copy(args_file, args_terminal);

    vprintf(format, args_terminal);
    if (log_file != NULL) {
        vfprintf(log_file, format, args_file);
    }

    va_end(args_file);
    va_end(args_terminal);
}

/*
 * Retorna um número aleatório no intervalo [0, limit).
 *
 * Exemplo:
 *   random_int(10) pode retornar valores de 0 até 9.
 */
static int random_int(int limit) {
    if (limit <= 0) {
        return 0;
    }
    return rand() % limit;
}

/*
 * Soma a demanda restante de CPU de todos os processos.
 *
 * Enquanto essa soma for maior que zero, ainda existe algum processo
 * que precisa executar.
 */
static int sum_requests(const Process processes[]) {
    int total = 0;

    for (int i = 0; i < N; i++) {
        total += processes[i].request;
    }

    return total;
}

/*
 * Ordena os processos pelo tempo de chegada usando bubble sort.
 *
 * O código Java original também usa bubble sort estável. Aqui mantemos
 * a mesma ideia para deixar a conversão mais próxima da versão original.
 */
static void sort_by_arrival(Process processes[]) {
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - 1 - i; j++) {
            if (processes[j].arrival > processes[j + 1].arrival) {
                Process tmp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = tmp;
            }
        }
    }
}

/*
 * Encontra o próximo tempo de chegada de algum processo ainda não finalizado.
 *
 * Essa função é usada quando, no tempo atual, nenhum processo está pronto.
 * Nesse caso, a simulação adianta o relógio para a próxima chegada.
 */
static int next_arrival_time(const Process processes[], int current_time) {
    int next = INT_MAX;

    for (int i = 0; i < N; i++) {
        if (processes[i].request > 0 &&
            processes[i].arrival > current_time &&
            processes[i].arrival < next) {
            next = processes[i].arrival;
        }
    }

    return next;
}

/*
 * Calcula os tickets de cada processo no instante atual.
 *
 * Apenas processos que já chegaram e que ainda possuem request > 0
 * participam do sorteio.
 *
 * A regra usada é a mesma do Java:
 *   tickets = max(1, request / 2)
 *
 * Assim, processos com maior demanda restante tendem a ter maior chance
 * de serem escolhidos, mas todo processo pronto recebe pelo menos 1 ticket.
 */
static int calculate_tickets(const Process processes[], int current_time, int tickets[]) {
    int total_tickets = 0;

    for (int i = 0; i < N; i++) {
        tickets[i] = 0;

        if (processes[i].request > 0 && processes[i].arrival <= current_time) {
            tickets[i] = processes[i].request / 2;
            if (tickets[i] < 1) {
                tickets[i] = 1;
            }
            total_tickets += tickets[i];
        }
    }

    return total_tickets;
}

/*
 * Sorteia o processo vencedor com base nos tickets calculados.
 *
 * Exemplo:
 *   Processo A: 3 tickets
 *   Processo B: 2 tickets
 *   Total: 5 tickets
 *
 * O sorteio escolhe um número de 0 até 4. Os primeiros 3 valores pertencem
 * ao processo A, e os outros 2 pertencem ao processo B.
 */
static int choose_winner(const int tickets[], int total_tickets) {
    int winner_ticket = random_int(total_tickets);
    int accumulated = 0;

    for (int i = 0; i < N; i++) {
        if (tickets[i] > 0) {
            accumulated += tickets[i];
            if (winner_ticket < accumulated) {
                return i;
            }
        }
    }

    return -1;
}

/*
 * Lê uma seed opcional da linha de comando.
 *
 * Sem seed fixa, o programa usa time(NULL), gerando resultados diferentes
 * a cada execução.
 *
 * Com seed fixa, por exemplo --seed 42, o resultado fica reproduzível.
 */
static unsigned int read_seed(int argc, char *argv[]) {
    unsigned int seed = (unsigned int) time(NULL);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int) strtoul(argv[i + 1], NULL, 10);
            i++;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Uso: %s [--seed NUMERO]\n", argv[0]);
            exit(0);
        }
    }

    return seed;
}

int main(int argc, char *argv[]) {
    Process processes[N];
    FILE *log_file = NULL;
    int quantum;
    int time_now;

    unsigned int seed = read_seed(argc, argv);
    srand(seed);

    log_file = fopen("log.txt", "w");
    if (log_file == NULL) {
        fprintf(stderr, "Erro: nao foi possivel criar o arquivo log.txt\n");
        return 1;
    }

    /*
     * Cria os processos da simulação.
     *
     * Os nomes vão de 'a' até 'j'. O tempo de chegada e a demanda de CPU
     * são gerados aleatoriamente, como no código Java original.
     */
    for (int i = 0; i < N; i++) {
        int request = random_int(MAX_REQ);

        processes[i].name = (char) ('a' + i);
        processes[i].arrival = random_int(MAX_ARRV);
        processes[i].request = (request == 0) ? 5 : request;
    }

    sort_by_arrival(processes);

    /*
     * Sorteia o quantum.
     *
     * Assim como no Java, se o valor sorteado for 0, usamos 5 para evitar
     * um quantum inválido.
     */
    quantum = random_int(MAX_QUAN);
    if (quantum == 0) {
        quantum = 5;
    }

    log_printf(log_file, "Lottery Scheduling\n");
    log_printf(log_file, "Seed: %u\n", seed);
    log_printf(log_file, "\nEstado inicial:\n");

    for (int i = 0; i < N; i++) {
        log_printf(log_file,
                   "\tprocess: %c\tarrival: %2d\trequest: %2d\n",
                   processes[i].name,
                   processes[i].arrival,
                   processes[i].request);
    }

    log_printf(log_file, "quantum: %d\n", quantum);
    log_printf(log_file, "---------------------------------------------------------\n");
    log_printf(log_file, "%6s%10s%10s%10s%10s\n",
               "time", "winner", "request", "tickets", "total");
    log_printf(log_file, "---------------------------------------------------------\n");

    /*
     * A simulação começa no tempo de chegada do primeiro processo.
     */
    time_now = processes[0].arrival;

    /*
     * Loop principal do escalonador.
     *
     * A cada iteração:
     *   1. Calcula os tickets dos processos prontos.
     *   2. Sorteia um processo vencedor.
     *   3. Executa o vencedor por um quantum ou até ele terminar.
     *   4. Atualiza o relógio da simulação.
     */
    while (sum_requests(processes) > 0) {
        int tickets[N];
        int total_tickets = calculate_tickets(processes, time_now, tickets);
        int winner;
        int consumed;

        /*
         * Se nenhum processo está pronto no tempo atual, avançamos o relógio
         * para o próximo processo que ainda vai chegar.
         */
        if (total_tickets == 0) {
            int next_time = next_arrival_time(processes, time_now);

            if (next_time == INT_MAX) {
                break;
            }

            time_now = next_time;
            continue;
        }

        winner = choose_winner(tickets, total_tickets);
        if (winner < 0) {
            fprintf(stderr, "Erro interno: nenhum processo vencedor foi escolhido.\n");
            fclose(log_file);
            return 1;
        }

        /*
         * O processo vencedor executa por no máximo um quantum.
         * Se ele precisar de menos tempo que o quantum, consome apenas
         * o tempo necessário para terminar.
         */
        consumed = quantum;
        if (processes[winner].request < consumed) {
            consumed = processes[winner].request;
        }

        processes[winner].request -= consumed;
        time_now += consumed;

        log_printf(log_file,
                   "%6d%10c%10d%10d%10d\n",
                   time_now,
                   processes[winner].name,
                   processes[winner].request,
                   tickets[winner],
                   total_tickets);
    }

    log_printf(log_file, "---------------------------------------------------------\n");
    log_printf(log_file, "Todos os processos finalizados em t = %d\n", time_now);

    fclose(log_file);
    return 0;
}
