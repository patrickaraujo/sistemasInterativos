#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <string.h>

/*
 * Fair-Share Scheduler em C
 *
 * Este programa é uma versão em C do simulador Java enviado originalmente.
 * Ele simula um processador executando vários processos de dois grupos:
 *
 *   - users: 75% da CPU
 *   - admin: 25% da CPU
 *
 * O objetivo é comparar dois modos de escalonamento:
 *
 *   1. Fair-Share Scheduler (--fss)
 *      Tenta aproximar o uso real da CPU à porcentagem de share de cada grupo.
 *
 *   2. Prioridade simples do sistema (--system)
 *      Escolhe os processos apenas pela prioridade individual, sem considerar
 *      a divisão justa entre grupos.
 *
 * Importante:
 * Este programa é um simulador em loop. Ele não cria threads reais do sistema
 * operacional. Cada iteração representa um "tick" lógico de CPU.
 */

/* Número máximo de ticks que um processo executa antes de sofrer preempção. */
#define QUANTUM 50

/* Capacidade inicial dos vetores dinâmicos usados no simulador. */
#define INITIAL_CAPACITY 128

/* Limite de grupos registrados no log. Neste exemplo usamos apenas 2. */
#define LOG_GROUP_LIMIT 10

/* Declarações antecipadas para permitir structs com ponteiros entre si. */
typedef struct Processor Processor;
typedef struct ShareGroup ShareGroup;
typedef struct Process Process;
typedef struct ReadyQueue ReadyQueue;

/*
 * Representa um processo simulado.
 *
 * count    = quanto tempo de CPU o processo já consumiu.
 * burst    = quanto tempo total o processo precisa para terminar.
 * start    = instante de criação do processo.
 * last_run = último instante em que o processo foi colocado de volta na fila.
 * group    = grupo ao qual o processo pertence.
 * idle     = indica se este é o processo ocioso da CPU.
 */
struct Process {
    Processor *cpu;
    int count;
    int burst;
    long start;
    long last_run;
    ShareGroup *group;
    bool idle;
};

/*
 * Representa um grupo de processos com uma porcentagem desejada de CPU.
 *
 * shares        = porcentagem desejada de CPU. Ex.: 75 ou 25.
 * members       = processos pertencentes ao grupo.
 * member_count  = quantidade atual de processos no grupo.
 * last_rate     = última taxa registrada no log, usada para evitar logs repetidos.
 */
struct ShareGroup {
    int shares;
    Process **members;
    int member_count;
    int member_capacity;
    Processor *cpu;
    double last_rate;
};

/*
 * Fila de processos prontos.
 *
 * A fila é implementada como um min-heap. O processo com menor valor de
 * prioridade fica no topo e será escolhido primeiro.
 *
 * fair_share = true  -> usa prioridade considerando o grupo.
 * fair_share = false -> usa apenas prioridade individual do processo.
 */
struct ReadyQueue {
    Process **heap;
    int size;
    int capacity;
    int terminated_count;
    long idle_time;
    bool fair_share;
    Process *idle_process;
};

/*
 * Processador simulado.
 *
 * time         = tempo lógico total da simulação.
 * cycle        = ticks executados desde a última troca de contexto.
 * total_cycles = quantidade de trocas de contexto.
 * current_task = processo atualmente em execução.
 * queue        = fila de processos prontos.
 */
struct Processor {
    long time;
    int cycle;
    int total_cycles;
    Process *current_task;
    ReadyQueue queue;
};

static double process_priority(Process *p);
static double process_share_priority(Process *p);
static long processor_idle_time(Processor *cpu);
static void ready_queue_add(ReadyQueue *q, Process *p);
static Process *ready_queue_next(ReadyQueue *q);
static void ready_queue_schedule(ReadyQueue *q);
static void processor_task_switch(Processor *cpu);

/* Encerra o programa caso uma alocação de memória ou abertura de arquivo falhe. */
static void die_if_null(void *ptr, const char *message) {
    if (ptr == NULL) {
        fprintf(stderr, "%s\n", message);
        exit(EXIT_FAILURE);
    }
}

/*
 * Cria um processo comum.
 *
 * O burst é sorteado entre 10 e 299 ticks, imitando processos com tempos de
 * execução diferentes. Se um grupo for informado, o processo é registrado como
 * membro desse grupo.
 */
static Process *process_create(Processor *cpu, ShareGroup *group) {
    Process *p = calloc(1, sizeof(Process));
    die_if_null(p, "Could not allocate process");

    p->cpu = cpu;
    p->count = 0;
    p->start = cpu != NULL ? cpu->time : 0;
    p->last_run = 1;
    p->burst = 10 + rand() % 290;
    p->group = group;
    p->idle = false;

    if (group != NULL) {
        if (group->member_count == group->member_capacity) {
            group->member_capacity *= 2;
            group->members = realloc(group->members,
                                     group->member_capacity * sizeof(Process *));
            die_if_null(group->members, "Could not grow group members");
        }
        group->members[group->member_count++] = p;
    }

    return p;
}

/*
 * Cria o processo ocioso.
 *
 * Ele é executado quando não existe nenhum processo real na fila. O burst é
 * enorme para que ele nunca termine durante a simulação.
 */
static Process *idle_process_create(Processor *cpu) {
    Process *p = process_create(cpu, NULL);
    p->burst = INT_MAX;
    p->idle = true;
    return p;
}

/* Cria um grupo com uma porcentagem desejada de CPU. */
static ShareGroup *share_group_create(int shares) {
    ShareGroup *g = calloc(1, sizeof(ShareGroup));
    die_if_null(g, "Could not allocate share group");

    g->shares = shares;
    g->member_capacity = INITIAL_CAPACITY;
    g->members = calloc(g->member_capacity, sizeof(Process *));
    die_if_null(g->members, "Could not allocate group members");
    g->last_rate = 0.0;
    return g;
}

/* Soma o tempo de CPU já consumido por todos os processos de um grupo. */
static int share_group_total(ShareGroup *g) {
    int total = 0;
    for (int i = 0; i < g->member_count; i++) {
        total += g->members[i]->count;
    }
    return total;
}

/*
 * Calcula o desvio de uso de CPU de um grupo.
 *
 * A ideia é comparar:
 *
 *   uso real do grupo - uso esperado do grupo
 *
 * Se o valor é baixo, o grupo ainda recebeu pouco tempo de CPU em relação ao
 * que deveria receber. Por isso, processos desse grupo ganham prioridade.
 */
static double share_group_priority(ShareGroup *g) {
    if (g == NULL || g->member_count == 0) {
        return 1e18;
    }
    if (g->cpu == NULL) {
        g->cpu = g->members[0]->cpu;
    }

    long busy_time = g->cpu->time - processor_idle_time(g->cpu);
    double allotted_share = busy_time * ((double) g->shares / 100.0);
    double actual_share = (double) share_group_total(g) - allotted_share;
    return actual_share;
}

/*
 * Prioridade individual de um processo.
 *
 * Processos que consumiram pouco tempo de CPU em relação ao último momento em
 * que foram executados tendem a ter prioridade menor e, portanto, são escolhidos
 * mais cedo pelo heap.
 */
static double process_priority(Process *p) {
    if (p->idle) {
        return 1e18;
    }
    if (p->last_run <= 0) {
        return 0.0;
    }
    return (double) p->count / (double) p->last_run;
}

/*
 * Prioridade usada no modo Fair-Share.
 *
 * Ela combina:
 *
 *   - prioridade do grupo, para respeitar a divisão 75%/25%;
 *   - parte fracionária da prioridade individual, para desempatar processos
 *     dentro do mesmo grupo.
 */
static double process_share_priority(Process *p) {
    if (p->idle) {
        return 1e18;
    }
    double priority = process_priority(p);
    double fractional = priority - floor(priority);
    return share_group_priority(p->group) + fractional;
}

/* Escolhe a fórmula de prioridade de acordo com o modo selecionado. */
static double queue_priority(ReadyQueue *q, Process *p) {
    return q->fair_share ? process_share_priority(p) : process_priority(p);
}

/* Retorna true quando o processo 'a' deve vir antes do processo 'b' no heap. */
static bool less_than(ReadyQueue *q, Process *a, Process *b) {
    return queue_priority(q, a) < queue_priority(q, b);
}

static void heap_swap(Process **a, Process **b) {
    Process *tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Insere um processo no min-heap da fila de prontos. */
static void heap_push(ReadyQueue *q, Process *p) {
    if (q->size == q->capacity) {
        q->capacity *= 2;
        q->heap = realloc(q->heap, q->capacity * sizeof(Process *));
        die_if_null(q->heap, "Could not grow ready queue");
    }

    int i = q->size++;
    q->heap[i] = p;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (!less_than(q, q->heap[i], q->heap[parent])) {
            break;
        }
        heap_swap(&q->heap[i], &q->heap[parent]);
        i = parent;
    }
}

/* Remove e retorna o processo de maior prioridade, isto é, o topo do min-heap. */
static Process *heap_pop(ReadyQueue *q) {
    if (q->size == 0) {
        return NULL;
    }

    Process *result = q->heap[0];
    q->heap[0] = q->heap[--q->size];

    int i = 0;
    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < q->size && less_than(q, q->heap[left], q->heap[smallest])) {
            smallest = left;
        }
        if (right < q->size && less_than(q, q->heap[right], q->heap[smallest])) {
            smallest = right;
        }
        if (smallest == i) {
            break;
        }

        heap_swap(&q->heap[i], &q->heap[smallest]);
        i = smallest;
    }

    return result;
}

/* Inicializa a fila de processos prontos. */
static void ready_queue_init(ReadyQueue *q, bool fair_share) {
    q->capacity = INITIAL_CAPACITY;
    q->heap = calloc(q->capacity, sizeof(Process *));
    die_if_null(q->heap, "Could not allocate ready queue");
    q->size = 0;
    q->terminated_count = 0;
    q->idle_time = 0;
    q->fair_share = fair_share;
    q->idle_process = NULL;
}

/*
 * Adiciona um processo à fila de prontos.
 *
 * Casos tratados:
 *
 *   - processo idle: acumula tempo ocioso e não entra na fila;
 *   - processo ainda não terminou: volta para o heap;
 *   - processo terminou: incrementa o contador de terminados.
 */
static void ready_queue_add(ReadyQueue *q, Process *p) {
    if (p == q->idle_process) {
        q->idle_time += q->idle_process->count;
        q->idle_process->count = 0;
        return;
    }

    if (p->count < p->burst) {
        heap_push(q, p);
    } else {
        q->terminated_count++;
    }
}

/* Retorna o próximo processo. Se a fila estiver vazia, retorna o processo idle. */
static Process *ready_queue_next(ReadyQueue *q) {
    if (q->size == 0) {
        return q->idle_process;
    }
    return heap_pop(q);
}

/*
 * Reorganiza todo o heap.
 *
 * Isso é necessário porque a prioridade de um processo depende do tempo global
 * e do uso acumulado dos grupos. Mesmo que a estrutura do heap não mude, os
 * valores de prioridade mudam ao longo da simulação.
 */
static void ready_queue_schedule(ReadyQueue *q) {
    for (int i = q->size / 2 - 1; i >= 0; i--) {
        int parent = i;
        while (true) {
            int left = 2 * parent + 1;
            int right = 2 * parent + 2;
            int smallest = parent;

            if (left < q->size && less_than(q, q->heap[left], q->heap[smallest])) {
                smallest = left;
            }
            if (right < q->size && less_than(q, q->heap[right], q->heap[smallest])) {
                smallest = right;
            }
            if (smallest == parent) {
                break;
            }
            heap_swap(&q->heap[parent], &q->heap[smallest]);
            parent = smallest;
        }
    }
}

static int ready_queue_total_size(ReadyQueue *q) {
    return q->size + q->terminated_count;
}

/* Inicializa o processador e coloca o processo idle como tarefa atual. */
static void processor_init(Processor *cpu, bool fair_share) {
    cpu->time = 0;
    cpu->cycle = 0;
    cpu->total_cycles = 0;
    ready_queue_init(&cpu->queue, fair_share);
    cpu->queue.idle_process = idle_process_create(cpu);
    cpu->current_task = cpu->queue.idle_process;
}

static long processor_idle_time(Processor *cpu) {
    return cpu->queue.idle_time;
}

/*
 * Realiza uma troca de contexto.
 *
 * A tarefa atual volta para a fila, caso ainda não tenha terminado. Em seguida,
 * o escalonador escolhe o próximo processo pronto.
 */
static void processor_task_switch(Processor *cpu) {
    cpu->cycle = 0;
    cpu->total_cycles++;
    cpu->current_task->last_run = cpu->time;
    ready_queue_add(&cpu->queue, cpu->current_task);
    cpu->current_task = ready_queue_next(&cpu->queue);
}

static void processor_enqueue(Processor *cpu, Process *p) {
    ready_queue_add(&cpu->queue, p);
}

/*
 * Registra no arquivo de log a taxa real de CPU usada por cada grupo.
 *
 * Formato do log:
 *
 *   tempo<TAB>taxa_users<TAB>taxa_admin
 *
 * Exemplo:
 *
 *   1000    74.5000    25.5000
 */
static void log_rates(FILE *log_file, Processor *cpu, ShareGroup **groups, int group_count) {
    long time_value = cpu->time < 1 ? 1 : cpu->time;
    long busy_time = cpu->time - processor_idle_time(cpu);
    if (busy_time < 1) {
        busy_time = 1;
    }

    double rates[LOG_GROUP_LIMIT];
    bool changed = false;

    for (int i = 0; i < group_count; i++) {
        ShareGroup *g = groups[i];
        rates[i] = 100.0 * (double) share_group_total(g) / (double) busy_time;
        if (fabs(rates[i] - g->last_rate) > 1e-9) {
            changed = true;
        }
    }

    if (!changed) {
        return;
    }

    fprintf(log_file, "%ld", time_value);
    for (int i = 0; i < group_count; i++) {
        fprintf(log_file, "\t%.4f", rates[i]);
        groups[i]->last_rate = rates[i];
    }
    fprintf(log_file, "\n");
}

/*
 * Executa um tick da CPU simulada.
 *
 * Em cada tick:
 *
 *   1. O tempo global avança.
 *   2. O processo atual consome 1 unidade de CPU.
 *   3. Se o quantum acabou ou o processo terminou, ocorre troca de contexto.
 *   4. A cada 1000 ticks, a fila é reorganizada e o log é atualizado.
 */
static void processor_run_tick(Processor *cpu, FILE *log_file, ShareGroup **groups, int group_count) {
    cpu->time++;
    cpu->cycle++;

    if (cpu->cycle >= QUANTUM) {
        processor_task_switch(cpu);
    }

    if (cpu->current_task->count < cpu->current_task->burst) {
        cpu->current_task->count++;
    } else {
        processor_task_switch(cpu);
    }

    if (cpu->time % 1000 == 0) {
        ready_queue_schedule(&cpu->queue);
        log_rates(log_file, cpu, groups, group_count);
    }
}

/* A simulação termina quando só resta o processo idle em execução. */
static bool processor_done(Processor *cpu) {
    return cpu->queue.size == 0 && cpu->current_task == cpu->queue.idle_process;
}

/* Adiciona uma quantidade de processos de um pool à fila da CPU. */
static void enqueue_from_pool(Processor *cpu, Process **pool, int *index, int total, int amount) {
    for (int i = 0; i < amount; i++) {
        if (*index >= total) {
            return;
        }
        processor_enqueue(cpu, pool[*index]);
        (*index)++;
    }
}

/* Libera a memória usada por um grupo e seus processos. */
static void share_group_destroy(ShareGroup *g) {
    if (g == NULL) {
        return;
    }
    for (int i = 0; i < g->member_count; i++) {
        free(g->members[i]);
    }
    free(g->members);
    free(g);
}

/* Libera a memória interna da fila, incluindo o processo idle. */
static void ready_queue_destroy(ReadyQueue *q) {
    free(q->idle_process);
    free(q->heap);
}

int main(int argc, char **argv) {
    bool fair_share = true;
    unsigned int seed = (unsigned int) time(NULL);

    /*
     * Lê os argumentos de linha de comando.
     *
     * --fss          usa Fair-Share Scheduler.
     * --system       usa prioridade simples por processo.
     * --seed NUM     fixa a semente aleatória para repetir o experimento.
     */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--system") == 0) {
            fair_share = false;
        } else if (strcmp(argv[i], "--fss") == 0) {
            fair_share = true;
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int) strtoul(argv[++i], NULL, 10);
        }
    }

    srand(seed);

    Processor cpu;
    processor_init(&cpu, fair_share);

    /* Dois grupos do experimento: users recebe 75% e admin recebe 25%. */
    ShareGroup *users = share_group_create(75);
    ShareGroup *admin = share_group_create(25);
    ShareGroup *groups[LOG_GROUP_LIMIT] = { users, admin };
    int group_count = 2;

    /* Quantidade total de processos que serão criados para cada grupo. */
    const int user_total = 1500;
    const int admin_total = 3000;

    /* Pools guardam todos os processos antes de eles chegarem à fila. */
    Process **user_pool = calloc(user_total, sizeof(Process *));
    Process **admin_pool = calloc(admin_total, sizeof(Process *));
    die_if_null(user_pool, "Could not allocate user pool");
    die_if_null(admin_pool, "Could not allocate admin pool");

    for (int i = 0; i < user_total; i++) {
        user_pool[i] = process_create(&cpu, users);
    }
    for (int i = 0; i < admin_total; i++) {
        admin_pool[i] = process_create(&cpu, admin);
    }

    FILE *log_file = fopen("log.txt", "w");
    die_if_null(log_file, "Could not open log.txt");

    clock_t start = clock();

    int user_index = 0;
    int admin_index = 0;
    int j = 0;

    /*
     * Fase de chegada dos processos.
     *
     * Em cada rodada, alguns processos dos grupos users e admin entram na fila.
     * Entre uma chegada e outra, a CPU executa por um número aleatório de ticks.
     */
    while (user_index < user_total || admin_index < admin_total) {
        if (j % 30 == 0) {
            enqueue_from_pool(&cpu, user_pool, &user_index, user_total, 30);
        }

        enqueue_from_pool(&cpu, user_pool, &user_index, user_total, rand() % 5);
        enqueue_from_pool(&cpu, admin_pool, &admin_index, admin_total, rand() % 5);

        int ticks_between_arrivals = 1 + rand() % 100;
        for (int t = 0; t < ticks_between_arrivals; t++) {
            processor_run_tick(&cpu, log_file, groups, group_count);
        }

        j++;
    }

    /* Depois que todos chegaram, continua executando até todos terminarem. */
    while (!processor_done(&cpu)) {
        processor_run_tick(&cpu, log_file, groups, group_count);
    }

    fclose(log_file);

    double elapsed_seconds = (double) (clock() - start) / CLOCKS_PER_SEC;
    long busy_time = cpu.time - processor_idle_time(&cpu);

    printf("Mode: %s\n", fair_share ? "fair-share" : "system priority");
    printf("Busy time: %ld, idle time: %ld\n", busy_time, processor_idle_time(&cpu));
    printf("Admin group total: %d\n", share_group_total(admin));
    printf("User group total: %d\n", share_group_total(users));
    printf("Admin actual share: %.2f%%\n", 100.0 * share_group_total(admin) / (double) busy_time);
    printf("User actual share: %.2f%%\n", 100.0 * share_group_total(users) / (double) busy_time);
    printf("Total number of processes: %d\n", ready_queue_total_size(&cpu.queue));
    printf("CPU simulation time: %.3fs\n", elapsed_seconds);
    printf("Finished. Rates were written to log.txt\n");

    /* Limpeza de memória. */
    free(user_pool);
    free(admin_pool);
    ready_queue_destroy(&cpu.queue);
    share_group_destroy(users);
    share_group_destroy(admin);

    return 0;
}
