/*
 * Multilevel Queue Scheduling
 *
 * Tres filas com politicas diferentes, cada uma recebendo uma fatia
 * de 10 unidades de tempo em round-robin entre as filas:
 *   Queue 1 (mais prioritaria): Round Robin com quantum 4
 *   Queue 2:                    Priority Scheduling (menor numero = maior prioridade)
 *   Queue 3 (menos prioritaria): FCFS (First Come First Served)
 *
 * Cada processo eh alocado em uma fila com base em sua prioridade,
 * definida pelas faixas informadas no inicio.
 *
 * Compilar: gcc -Wall -o multilevel multiLevel.c
 * Executar: ./multilevel
 *
 * NOTA: este arquivo foi renomeado de multiLevel.cpp para multiLevel.c
 *       pois eh C puro (usa stdio.h e stdbool.h, sem features de C++).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROC   50
#define FATIA      10  /* tempo dado a cada fila por rodada */
#define QUANTUM_RR 4   /* quantum interno do Round Robin   */

typedef struct {
    int priority;
    int burst_time;
    int rem_burst_time;
    int wt;
} Process;

/* Estado global da simulacao */
static Process p[MAX_PROC];
static int n;
static int queue1[MAX_PROC], q1 = 0;  /* indices dos processos em cada fila */
static int queue2[MAX_PROC], q2 = 0;
static int queue3[MAX_PROC], q3 = 0;
static int t = 0;                     /* tempo global */

/* Verifica se todos terminaram */
static bool todos_terminaram(void) {
    for (int i = 0; i < n; i++) {
        if (p[i].rem_burst_time > 0) return false;
    }
    return true;
}

/* Imprime resultado final */
static void imprime_resultado(void) {
    int avg_wait = 0, avg_turn = 0;
    printf("\n\n*****************************************************\n");
    printf("\t\t\tRESULTADO\n");
    printf("*****************************************************\n");
    printf("Processo\tWaiting time\tTurnaround time\n");
    for (int i = 0; i < n; i++) {
        printf("   %d\t\t    %d\t\t    %d\n",
               i + 1, p[i].wt, p[i].wt + p[i].burst_time);
        avg_wait += p[i].wt;
        avg_turn += p[i].wt + p[i].burst_time;
    }
    printf("\nTempo medio de espera   = %.2f\n", avg_wait / (float) n);
    printf("Tempo medio de retorno  = %.2f\n", avg_turn / (float) n);
    printf("\n~~~~~~~~~~~~~~~~~~~~~FIM~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
}

/* Ordena queue2 por prioridade ascendente (menor numero = maior prioridade) */
static void sort_queue2(void) {
    for (int i = 0; i < q2 - 1; i++) {
        for (int j = 0; j < q2 - i - 1; j++) {
            if (p[queue2[j]].priority > p[queue2[j + 1]].priority) {
                int temp = queue2[j];
                queue2[j] = queue2[j + 1];
                queue2[j + 1] = temp;
            }
        }
    }
}

/* Executa Round Robin na queue1 por ate FATIA unidades de tempo */
static void run_RR(void) {
    int gasto = 0;
    /* loop ate gastar FATIA ou ate nao haver mais processo ativo */
    while (gasto < FATIA) {
        bool achou = false;
        for (int i = 0; i < q1 && gasto < FATIA; i++) {
            if (p[queue1[i]].rem_burst_time > 0) {
                achou = true;
                int slice = QUANTUM_RR;
                if (slice > p[queue1[i]].rem_burst_time) slice = p[queue1[i]].rem_burst_time;
                if (slice > FATIA - gasto)              slice = FATIA - gasto;

                p[queue1[i]].rem_burst_time -= slice;
                t     += slice;
                gasto += slice;

                if (p[queue1[i]].rem_burst_time == 0) {
                    p[queue1[i]].wt = t - p[queue1[i]].burst_time;
                }
            }
        }
        if (!achou) return; /* fila vazia ou todos terminaram */
    }
}

/* Executa Priority Scheduling na queue2 por ate FATIA unidades de tempo */
static void run_PS(void) {
    sort_queue2();
    int gasto = 0;
    for (int i = 0; i < q2 && gasto < FATIA; i++) {
        if (p[queue2[i]].rem_burst_time > 0) {
            int slice = p[queue2[i]].rem_burst_time;
            if (slice > FATIA - gasto) slice = FATIA - gasto;

            p[queue2[i]].rem_burst_time -= slice;
            t     += slice;
            gasto += slice;

            if (p[queue2[i]].rem_burst_time == 0) {
                p[queue2[i]].wt = t - p[queue2[i]].burst_time;
            }
        }
    }
}

/* Executa FCFS na queue3 por ate FATIA unidades de tempo */
static void run_FCFS(void) {
    int gasto = 0;
    for (int i = 0; i < q3 && gasto < FATIA; i++) {
        if (p[queue3[i]].rem_burst_time > 0) {
            int slice = p[queue3[i]].rem_burst_time;
            if (slice > FATIA - gasto) slice = FATIA - gasto;

            p[queue3[i]].rem_burst_time -= slice;
            t     += slice;
            gasto += slice;

            if (p[queue3[i]].rem_burst_time == 0) {
                p[queue3[i]].wt = t - p[queue3[i]].burst_time;
            }
        }
    }
}

int main(void) {
    int prq[3][2];
    int i, j;

    printf("Entre com faixa de prioridade de cada fila (min max):\n");
    for (i = 0; i < 3; i++) {
        printf("  Queue %d: ", i + 1);
        for (j = 0; j < 2; j++) {
            if (scanf("%d", &prq[i][j]) != 1) {
                fprintf(stderr, "Entrada invalida.\n");
                return 1;
            }
        }
    }

    printf("\nNumero total de processos (max %d): ", MAX_PROC);
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_PROC) {
        fprintf(stderr, "Numero de processos invalido.\n");
        return 1;
    }

    printf("Entre com PRIORIDADE e BURST TIME de cada processo:\n");
    for (i = 0; i < n; i++) {
        printf("  Processo %d: ", i + 1);
        if (scanf("%d %d", &p[i].priority, &p[i].burst_time) != 2 || p[i].burst_time <= 0) {
            fprintf(stderr, "Entrada invalida.\n");
            return 1;
        }
        p[i].rem_burst_time = p[i].burst_time;
        p[i].wt = 0;
    }

    printf("\nDados inseridos:\n");
    printf("Processo\tPriority\tBurst time\n");
    for (i = 0; i < n; i++) {
        printf("   %d\t\t    %d\t\t    %d\n", i + 1, p[i].priority, p[i].burst_time);
    }

    /* Distribui cada processo em UMA fila apenas (a primeira faixa que casar) */
    for (i = 0; i < n; i++) {
        if      (p[i].priority >= prq[0][0] && p[i].priority <= prq[0][1]) queue1[q1++] = i;
        else if (p[i].priority >= prq[1][0] && p[i].priority <= prq[1][1]) queue2[q2++] = i;
        else if (p[i].priority >= prq[2][0] && p[i].priority <= prq[2][1]) queue3[q3++] = i;
        else {
            fprintf(stderr, "Processo %d (prioridade %d) nao se encaixa em nenhuma fila.\n",
                    i + 1, p[i].priority);
            return 1;
        }
    }

    /* Loop principal: alterna RR -> PS -> FCFS, FATIA unidades cada, ate todos terminarem */
    while (!todos_terminaram()) {
        if (q1 > 0) run_RR();
        if (todos_terminaram()) break;
        if (q2 > 0) run_PS();
        if (todos_terminaram()) break;
        if (q3 > 0) run_FCFS();
    }

    imprime_resultado();
    return 0;
}
