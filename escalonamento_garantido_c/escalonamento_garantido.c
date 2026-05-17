#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

/*
 * Escalonamento Garantido em C
 *
 * Este programa é uma adaptação em C do projeto Java enviado no arquivo
 * garantido.zip. O projeto original usa Swing para mostrar uma interface
 * gráfica. Esta versão usa o terminal e um arquivo de log.
 *
 * A ideia principal do simulador é:
 *
 *   1. Criar processos com um tempo de execução chamado surto.
 *   2. Colocar os processos em uma fila de entrada.
 *   3. Alocar processos na memória quando houver espaço.
 *   4. Executar os processos na CPU usando quantum.
 *   5. Recalcular prioridades durante a execução para aproximar a ideia de
 *      escalonamento garantido.
 *
 * Observação:
 * Esta versão não cria threads reais. Cada unidade de execução é simulada por
 * um tick lógico dentro de um loop.
 */

#define MEMORY_BLOCKS 32
#define BLOCK_SIZE_MB 4
#define MEMORY_TOTAL_MB (MEMORY_BLOCKS * BLOCK_SIZE_MB)
#define DEFAULT_PROCESS_MB 8
#define MAX_NAME 32
#define INITIAL_QUEUE_CAPACITY 32

/*
 * Prioridade segue a ordem do Java original:
 * ALTA vem antes de MEDIA, que vem antes de BAIXA.
 */
typedef enum {
    PRIORITY_ALTA = 0,
    PRIORITY_MEDIA = 1,
    PRIORITY_BAIXA = 2
} Priority;

/* Estados possíveis de um processo. */
typedef enum {
    STATE_NOVO,
    STATE_PRONTO,
    STATE_SUSPENSO,
    STATE_EXECUTANDO,
    STATE_FINALIZADO
} State;

/*
 * Processo simulado.
 *
 * id                 = identificador único.
 * name               = nome do processo, como P1, P2, P3.
 * mb                 = quantidade de memória necessária.
 * burst_remaining    = tempo restante para terminar.
 * burst_initial      = tempo inicial, usado apenas para relatório.
 * processed_time     = tempo de CPU já recebido pelo processo.
 * quantum_used       = tempo usado no quantum atual.
 * quantum_ratio      = razão usada para atualizar a prioridade.
 * state              = estado atual do processo.
 * priority           = prioridade atual.
 */
typedef struct {
    int id;
    char name[MAX_NAME];
    int mb;
    int burst_remaining;
    int burst_initial;
    int processed_time;
    int quantum_used;
    double quantum_ratio;
    State state;
    Priority priority;
} Process;

/*
 * Fila genérica de processos.
 *
 * A mesma estrutura é usada como:
 *
 *   - fila de entrada;
 *   - fila de processos prontos;
 *   - fila de processos suspensos.
 *
 * Quando ordered_by_priority é true, a remoção escolhe o processo com maior
 * prioridade, imitando a PriorityQueue do Java original.
 * Quando ordered_by_priority é false, a fila funciona como FIFO simples.
 */
typedef struct {
    Process **items;
    int size;
    int capacity;
    bool ordered_by_priority;
} ProcessQueue;

/*
 * Memória simulada.
 *
 * O projeto original divide a memória em 32 blocos de 4 MB.
 * Aqui guardamos o ID do processo dono de cada bloco.
 * Valor zero significa bloco livre.
 */
typedef struct {
    int blocks[MEMORY_BLOCKS];
    int free_blocks;
} Memory;

/* CPU simulada. */
typedef struct {
    Process *running;
    Memory memory;
    int total_time;
    int quantum;
    int partial_time;
} Processor;

/* Escalonador principal, equivalente à classe Escalonador do Java. */
typedef struct {
    ProcessQueue input_queue;
    ProcessQueue ready_queue;
    ProcessQueue suspended_queue;
    Processor cpu;
    bool started;
} Scheduler;

static const char *priority_to_string(Priority priority) {
    switch (priority) {
        case PRIORITY_ALTA: return "ALTA";
        case PRIORITY_MEDIA: return "MEDIA";
        case PRIORITY_BAIXA: return "BAIXA";
        default: return "DESCONHECIDA";
    }
}

static const char *state_to_string(State state) {
    switch (state) {
        case STATE_NOVO: return "NOVO";
        case STATE_PRONTO: return "PRONTO";
        case STATE_SUSPENSO: return "SUSPENSO";
        case STATE_EXECUTANDO: return "EXECUTANDO";
        case STATE_FINALIZADO: return "FINALIZADO";
        default: return "DESCONHECIDO";
    }
}

/* Encerra o programa em caso de falha de alocação. */
static void die_if_null(void *ptr, const char *message) {
    if (ptr == NULL) {
        fprintf(stderr, "%s\n", message);
        exit(EXIT_FAILURE);
    }
}

/* Inicializa uma fila de processos. */
static void queue_init(ProcessQueue *queue, bool ordered_by_priority) {
    queue->size = 0;
    queue->capacity = INITIAL_QUEUE_CAPACITY;
    queue->ordered_by_priority = ordered_by_priority;
    queue->items = calloc((size_t) queue->capacity, sizeof(Process *));
    die_if_null(queue->items, "Erro ao alocar fila de processos");
}

/* Libera a memória interna de uma fila. A fila não é dona dos processos. */
static void queue_destroy(ProcessQueue *queue) {
    free(queue->items);
    queue->items = NULL;
    queue->size = 0;
    queue->capacity = 0;
}

static bool queue_empty(const ProcessQueue *queue) {
    return queue->size == 0;
}

/* Aumenta a capacidade da fila quando necessário. */
static void queue_ensure_capacity(ProcessQueue *queue) {
    if (queue->size < queue->capacity) {
        return;
    }

    queue->capacity *= 2;
    queue->items = realloc(queue->items, (size_t) queue->capacity * sizeof(Process *));
    die_if_null(queue->items, "Erro ao redimensionar fila de processos");
}

/* Adiciona um processo na fila. */
static void queue_add(ProcessQueue *queue, Process *process) {
    queue_ensure_capacity(queue);
    queue->items[queue->size++] = process;
}

/* Retorna o primeiro processo sem remover. */
static Process *queue_head(const ProcessQueue *queue) {
    if (queue_empty(queue)) {
        return NULL;
    }

    if (!queue->ordered_by_priority) {
        return queue->items[0];
    }

    int best = 0;
    for (int i = 1; i < queue->size; i++) {
        Process *candidate = queue->items[i];
        Process *current = queue->items[best];

        if (candidate->priority < current->priority) {
            best = i;
        } else if (candidate->priority == current->priority && candidate->id < current->id) {
            best = i;
        }
    }

    return queue->items[best];
}

/*
 * Remove um processo da fila.
 *
 * Na fila de entrada, remove o primeiro item.
 * Na fila de prontos, remove o item de maior prioridade.
 */
static Process *queue_remove(ProcessQueue *queue) {
    if (queue_empty(queue)) {
        return NULL;
    }

    int index = 0;

    if (queue->ordered_by_priority) {
        for (int i = 1; i < queue->size; i++) {
            Process *candidate = queue->items[i];
            Process *current = queue->items[index];

            if (candidate->priority < current->priority) {
                index = i;
            } else if (candidate->priority == current->priority && candidate->id < current->id) {
                index = i;
            }
        }
    }

    Process *removed = queue->items[index];

    for (int i = index; i < queue->size - 1; i++) {
        queue->items[i] = queue->items[i + 1];
    }

    queue->size--;
    return removed;
}

/* Inicializa a memória com todos os blocos livres. */
static void memory_init(Memory *memory) {
    for (int i = 0; i < MEMORY_BLOCKS; i++) {
        memory->blocks[i] = 0;
    }
    memory->free_blocks = MEMORY_BLOCKS;
}

/* Calcula quantos blocos um processo precisa. */
static int memory_blocks_needed(int megabytes) {
    int blocks = megabytes / BLOCK_SIZE_MB;
    if (megabytes % BLOCK_SIZE_MB != 0) {
        blocks++;
    }
    return blocks;
}

/*
 * Procura uma sequência contígua de blocos livres.
 * Retorna a posição inicial ou -1 quando não há espaço contíguo suficiente.
 */
static int memory_find_position(const Memory *memory, int needed_blocks) {
    int sequence = 0;

    for (int i = 0; i < MEMORY_BLOCKS; i++) {
        if (memory->blocks[i] == 0) {
            sequence++;
        } else {
            sequence = 0;
        }

        if (sequence >= needed_blocks) {
            return i - sequence + 1;
        }
    }

    return -1;
}

/* Verifica se há espaço contíguo livre para um processo. */
static bool memory_has_space(const Memory *memory, int megabytes) {
    int needed_blocks = memory_blocks_needed(megabytes);
    return memory_find_position(memory, needed_blocks) >= 0;
}

/* Armazena um processo na memória. */
static bool memory_store(Memory *memory, const Process *process) {
    int needed_blocks = memory_blocks_needed(process->mb);
    int start = memory_find_position(memory, needed_blocks);

    if (start < 0) {
        return false;
    }

    for (int i = start; i < start + needed_blocks; i++) {
        memory->blocks[i] = process->id;
        memory->free_blocks--;
    }

    return true;
}

/* Libera todos os blocos ocupados por um processo. */
static void memory_free_process(Memory *memory, const Process *process) {
    for (int i = 0; i < MEMORY_BLOCKS; i++) {
        if (memory->blocks[i] == process->id) {
            memory->blocks[i] = 0;
            memory->free_blocks++;
        }
    }
}

static int memory_used_mb(const Memory *memory) {
    return (MEMORY_BLOCKS - memory->free_blocks) * BLOCK_SIZE_MB;
}

/* Imprime uma visão compacta dos blocos de memória. */
static void memory_print(FILE *out, const Memory *memory) {
    fprintf(out, "Memoria: ");
    for (int i = 0; i < MEMORY_BLOCKS; i++) {
        if (memory->blocks[i] == 0) {
            fprintf(out, ".");
        } else {
            fprintf(out, "#");
        }
    }
    fprintf(out, "  %dMB/%dMB usados\n", memory_used_mb(memory), MEMORY_TOTAL_MB);
}

/* Inicializa a CPU. */
static void processor_init(Processor *cpu) {
    cpu->running = NULL;
    cpu->total_time = 0;
    cpu->quantum = 2;
    cpu->partial_time = 0;
    memory_init(&cpu->memory);
}

/* Cria um processo. O tamanho de memória segue o padrão do Java: 8 MB. */
static Process *process_create(int id, int burst) {
    Process *process = calloc(1, sizeof(Process));
    die_if_null(process, "Erro ao alocar processo");

    process->id = id;
    snprintf(process->name, sizeof(process->name), "P%d", id);
    process->mb = DEFAULT_PROCESS_MB;
    process->burst_remaining = burst;
    process->burst_initial = burst;
    process->processed_time = 0;
    process->quantum_used = 0;
    process->quantum_ratio = 0.0;
    process->state = STATE_NOVO;
    process->priority = PRIORITY_MEDIA;

    return process;
}

static void process_destroy_all(Process **processes, int count) {
    for (int i = 0; i < count; i++) {
        free(processes[i]);
    }
}

/* Simula um tick de CPU para o processo em execução. */
static void process_tick(Process *process) {
    process->quantum_used++;
    process->processed_time++;
    process->burst_remaining--;
}

/* Remove o processo da CPU e zera o quantum atual, como no Java original. */
static Process *processor_remove(Processor *cpu) {
    Process *process = cpu->running;
    if (process != NULL) {
        process->quantum_used = 0;
    }
    cpu->running = NULL;
    return process;
}

/* Coloca um processo na CPU. */
static void processor_receive(Processor *cpu, Process *process) {
    cpu->running = process;
}

/* Verifica se o processo deve sair da CPU. */
static bool processor_finished_quantum_or_process(const Processor *cpu) {
    if (cpu->running == NULL) {
        return false;
    }

    return cpu->running->quantum_used >= cpu->quantum || cpu->running->burst_remaining <= 0;
}

/*
 * Executa o processo atual até ele terminar ou até consumir o quantum.
 * Cada tick representa uma unidade lógica de tempo.
 */
static void processor_run(Processor *cpu, FILE *log) {
    while (cpu->running != NULL && !processor_finished_quantum_or_process(cpu)) {
        cpu->total_time++;
        cpu->partial_time++;
        process_tick(cpu->running);

        fprintf(log,
                "tempo=%03d | executando=%s | restante=%d | processado=%d | quantum_usado=%d\n",
                cpu->total_time,
                cpu->running->name,
                cpu->running->burst_remaining,
                cpu->running->processed_time,
                cpu->running->quantum_used);
    }
}

/* Inicializa o escalonador. */
static void scheduler_init(Scheduler *scheduler) {
    queue_init(&scheduler->input_queue, false);
    queue_init(&scheduler->ready_queue, true);
    queue_init(&scheduler->suspended_queue, false);
    processor_init(&scheduler->cpu);
    scheduler->started = false;
}

static void scheduler_destroy(Scheduler *scheduler) {
    queue_destroy(&scheduler->input_queue);
    queue_destroy(&scheduler->ready_queue);
    queue_destroy(&scheduler->suspended_queue);
}

/* Define o quantum com base na quantidade de processos prontos. */
static int scheduler_define_quantum(Scheduler *scheduler) {
    int ready_count = scheduler->ready_queue.size;

    if (ready_count == 0) {
        scheduler->cpu.quantum = 2;
        return scheduler->cpu.quantum;
    }

    int quantum = 20 / ready_count;

    if (quantum < 2) {
        quantum = 2;
    }
    if (quantum > 10) {
        quantum = 10;
    }

    scheduler->cpu.quantum = quantum;
    return quantum;
}

/*
 * Move processos da fila de entrada para a fila de prontos enquanto houver
 * espaço contíguo na memória.
 */
static void scheduler_admit_processes(Scheduler *scheduler, FILE *log) {
    while (!queue_empty(&scheduler->input_queue)) {
        Process *next = queue_head(&scheduler->input_queue);

        if (!memory_has_space(&scheduler->cpu.memory, next->mb)) {
            break;
        }

        Process *process = queue_remove(&scheduler->input_queue);
        process->state = STATE_PRONTO;
        memory_store(&scheduler->cpu.memory, process);
        queue_add(&scheduler->ready_queue, process);

        fprintf(log,
                "tempo=%03d | admitido=%s | memoria=%dMB | estado=%s\n",
                scheduler->cpu.total_time,
                process->name,
                process->mb,
                state_to_string(process->state));
    }

    scheduler_define_quantum(scheduler);
}

/* Calcula a média de tempo processado pelos processos prontos. */
static double scheduler_calculate_partial_quantum(const Scheduler *scheduler) {
    if (scheduler->ready_queue.size == 0) {
        return 0.0;
    }

    double total = 0.0;
    for (int i = 0; i < scheduler->ready_queue.size; i++) {
        total += scheduler->ready_queue.items[i]->processed_time;
    }

    return total / scheduler->ready_queue.size;
}

/*
 * Atualiza a prioridade dos processos prontos.
 *
 * A lógica segue o Java original:
 *
 *   - processo que recebeu mais CPU que a média fica com prioridade BAIXA;
 *   - processo próximo da média fica com prioridade MEDIA;
 *   - processo que recebeu menos CPU que a média fica com prioridade ALTA.
 */
static void scheduler_update_priorities(Scheduler *scheduler) {
    if (scheduler->ready_queue.size == 0) {
        return;
    }

    double partial_quantum = scheduler_calculate_partial_quantum(scheduler);
    if (partial_quantum <= 0.0) {
        return;
    }

    for (int i = 0; i < scheduler->ready_queue.size; i++) {
        Process *process = scheduler->ready_queue.items[i];
        process->quantum_ratio = process->processed_time / partial_quantum;

        if (process->quantum_ratio > 1.0) {
            process->priority = PRIORITY_BAIXA;
        } else if (fabs(process->quantum_ratio - 1.0) < 0.000001) {
            process->priority = PRIORITY_MEDIA;
        } else {
            process->priority = PRIORITY_ALTA;
        }
    }
}

/* Finaliza manualmente o processo que está na CPU. */
static void scheduler_finish_running_process(Scheduler *scheduler, FILE *log) {
    Process *process = processor_remove(&scheduler->cpu);
    if (process == NULL) {
        return;
    }

    process->state = STATE_FINALIZADO;
    process->burst_remaining = 0;
    memory_free_process(&scheduler->cpu.memory, process);

    fprintf(log, "tempo=%03d | finalizacao_manual=%s\n", scheduler->cpu.total_time, process->name);
    scheduler_admit_processes(scheduler, log);
}

/* Suspende manualmente o processo em execução. */
static void scheduler_suspend_running_process(Scheduler *scheduler, FILE *log) {
    Process *process = processor_remove(&scheduler->cpu);
    if (process == NULL) {
        return;
    }

    process->state = STATE_SUSPENSO;
    queue_add(&scheduler->suspended_queue, process);

    fprintf(log, "tempo=%03d | suspenso=%s\n", scheduler->cpu.total_time, process->name);
}

/* Retorna um processo suspenso para a fila de prontos. */
static void scheduler_resume_one_process(Scheduler *scheduler, FILE *log) {
    Process *process = queue_remove(&scheduler->suspended_queue);
    if (process == NULL) {
        return;
    }

    process->state = STATE_PRONTO;
    queue_add(&scheduler->ready_queue, process);

    fprintf(log, "tempo=%03d | retomado=%s\n", scheduler->cpu.total_time, process->name);
    scheduler_define_quantum(scheduler);
}

/* Imprime uma linha de processo no relatório final. */
static void print_process_row(FILE *out, const Process *process) {
    fprintf(out,
            "%-4d %-6s %-10s %-12s %-8d %-12d %-8.2f\n",
            process->id,
            process->name,
            priority_to_string(process->priority),
            state_to_string(process->state),
            process->burst_remaining,
            process->processed_time,
            process->quantum_ratio);
}

/*
 * Executa o escalonamento até todos os processos terminarem.
 */
static void scheduler_run(Scheduler *scheduler, FILE *log) {
    scheduler->started = true;
    scheduler_admit_processes(scheduler, log);

    fprintf(log, "\n=== Inicio do escalonamento ===\n");

    while (!queue_empty(&scheduler->ready_queue) || !queue_empty(&scheduler->input_queue)) {
        if (queue_empty(&scheduler->ready_queue)) {
            scheduler_admit_processes(scheduler, log);
            if (queue_empty(&scheduler->ready_queue)) {
                fprintf(log, "tempo=%03d | CPU sem processo pronto\n", scheduler->cpu.total_time);
                break;
            }
        }

        scheduler_define_quantum(scheduler);

        Process *process = queue_remove(&scheduler->ready_queue);
        processor_receive(&scheduler->cpu, process);
        process->state = STATE_EXECUTANDO;

        fprintf(log,
                "\nDispatch | tempo=%03d | processo=%s | quantum=%d | prioridade=%s\n",
                scheduler->cpu.total_time,
                process->name,
                scheduler->cpu.quantum,
                priority_to_string(process->priority));

        processor_run(&scheduler->cpu, log);

        process = processor_remove(&scheduler->cpu);

        if (process->burst_remaining <= 0) {
            process->state = STATE_FINALIZADO;
            memory_free_process(&scheduler->cpu.memory, process);

            fprintf(log,
                    "Finalizado | tempo=%03d | processo=%s | total_processado=%d\n",
                    scheduler->cpu.total_time,
                    process->name,
                    process->processed_time);

            scheduler_admit_processes(scheduler, log);
        } else {
            process->state = STATE_PRONTO;
            queue_add(&scheduler->ready_queue, process);

            fprintf(log,
                    "Preemptado | tempo=%03d | processo=%s | restante=%d | volta_para_fila\n",
                    scheduler->cpu.total_time,
                    process->name,
                    process->burst_remaining);
        }

        scheduler_update_priorities(scheduler);
        scheduler_define_quantum(scheduler);
        memory_print(log, &scheduler->cpu.memory);
    }

    fprintf(log, "\n=== Fim do escalonamento ===\n");
}

/* Adiciona processos na fila de entrada. */
static Process **create_processes_from_bursts(Scheduler *scheduler, const int *bursts, int count) {
    Process **processes = calloc((size_t) count, sizeof(Process *));
    die_if_null(processes, "Erro ao alocar vetor de processos");

    for (int i = 0; i < count; i++) {
        processes[i] = process_create(i + 1, bursts[i]);
        queue_add(&scheduler->input_queue, processes[i]);
    }

    return processes;
}

/* Mostra ajuda de uso. */
static void print_usage(const char *program_name) {
    printf("Uso:\n");
    printf("  %s\n", program_name);
    printf("  %s 10 4 7 13 6\n", program_name);
    printf("\n");
    printf("Sem argumentos, o programa usa um conjunto padrao de processos.\n");
    printf("Com argumentos, cada numero vira o surto de um processo.\n");
}

int main(int argc, char **argv) {
    if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }

    Scheduler scheduler;
    scheduler_init(&scheduler);

    int default_bursts[] = {10, 4, 7, 13, 6, 9, 5, 11, 8, 12, 3, 15, 6, 10, 7, 14, 5, 8};
    int *bursts = default_bursts;
    int count = (int) (sizeof(default_bursts) / sizeof(default_bursts[0]));

    if (argc > 1) {
        count = argc - 1;
        bursts = calloc((size_t) count, sizeof(int));
        die_if_null(bursts, "Erro ao alocar vetor de surtos");

        for (int i = 0; i < count; i++) {
            int value = atoi(argv[i + 1]);
            if (value <= 0) {
                fprintf(stderr, "Surto invalido: %s\n", argv[i + 1]);
                free(bursts);
                scheduler_destroy(&scheduler);
                return EXIT_FAILURE;
            }
            bursts[i] = value;
        }
    }

    Process **processes = create_processes_from_bursts(&scheduler, bursts, count);

    FILE *log = fopen("log.txt", "w");
    if (log == NULL) {
        fprintf(stderr, "Nao foi possivel criar log.txt\n");
        process_destroy_all(processes, count);
        free(processes);
        if (argc > 1) {
            free(bursts);
        }
        scheduler_destroy(&scheduler);
        return EXIT_FAILURE;
    }

    fprintf(log, "Escalonamento Garantido em C\n");
    fprintf(log, "Total de processos: %d\n", count);
    fprintf(log, "Memoria total: %dMB\n", MEMORY_TOTAL_MB);
    fprintf(log, "Tamanho padrao por processo: %dMB\n\n", DEFAULT_PROCESS_MB);

    scheduler_run(&scheduler, log);

    fclose(log);

    printf("Escalonamento finalizado.\n");
    printf("Tempo total de CPU: %d ticks\n", scheduler.cpu.total_time);
    printf("Memoria utilizada no fim: %dMB/%dMB\n", memory_used_mb(&scheduler.cpu.memory), MEMORY_TOTAL_MB);
    printf("Log gerado em: log.txt\n\n");

    printf("%-4s %-6s %-10s %-12s %-8s %-12s %-8s\n",
           "ID", "Nome", "Priorid.", "Estado", "Rest.", "Processado", "Razao");

    for (int i = 0; i < count; i++) {
        print_process_row(stdout, processes[i]);
    }

    /*
     * As funções abaixo existem para espelhar as ações da interface Java.
     * Elas não são chamadas no modo automático, mas ficam disponíveis para
     * extensão futura do simulador.
     */
    (void) scheduler_finish_running_process;
    (void) scheduler_suspend_running_process;
    (void) scheduler_resume_one_process;

    process_destroy_all(processes, count);
    free(processes);

    if (argc > 1) {
        free(bursts);
    }

    scheduler_destroy(&scheduler);
    return EXIT_SUCCESS;
}
