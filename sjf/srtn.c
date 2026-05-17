/*
 * SRTN - Shortest Remaining Time Next (SJF preemptivo)
 *
 * A cada unidade de tempo, escolhe entre os processos que ja chegaram
 * aquele com o menor tempo de execucao restante.
 *
 * Compilar: gcc -o srtn srtn.c
 * Executar: ./srtn
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 100

int main(void) {
    int arrival[MAX], burst[MAX], original_burst[MAX];
    int waiting[MAX], turnaround[MAX], completion[MAX];
    int i, smallest, count = 0, time, n;
    double avg_wait = 0.0, avg_turn = 0.0;

    printf("Entre com o numero de processos (max %d): ", MAX);
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX) {
        fprintf(stderr, "Numero de processos invalido.\n");
        return 1;
    }

    for (i = 0; i < n; i++) {
        printf("Entre com o tempo de chegada do processo %d: ", i + 1);
        if (scanf("%d", &arrival[i]) != 1) {
            fprintf(stderr, "Entrada invalida.\n");
            return 1;
        }
    }

    for (i = 0; i < n; i++) {
        printf("Entre com a duracao (burst) do processo %d: ", i + 1);
        if (scanf("%d", &burst[i]) != 1 || burst[i] <= 0) {
            fprintf(stderr, "Duracao invalida.\n");
            return 1;
        }
        original_burst[i] = burst[i];
    }

    /* Simulacao tick a tick */
    for (time = 0; count != n; time++) {
        smallest = -1;
        for (i = 0; i < n; i++) {
            if (arrival[i] <= time && burst[i] > 0) {
                if (smallest == -1 || burst[i] < burst[smallest]) {
                    smallest = i;
                }
            }
        }

        if (smallest == -1) {
            /* CPU ociosa: nenhum processo chegou ainda */
            continue;
        }

        burst[smallest]--;

        if (burst[smallest] == 0) {
            count++;
            completion[smallest] = time + 1;
            turnaround[smallest] = completion[smallest] - arrival[smallest];
            waiting[smallest]    = turnaround[smallest] - original_burst[smallest];
        }
    }

    printf("\npID\tT.Duracao\tT.Chegada\tT.Espera\tT.Resposta\tT.Conclusao\n");
    for (i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
               i + 1, original_burst[i], arrival[i],
               waiting[i], turnaround[i], completion[i]);
        avg_wait += waiting[i];
        avg_turn += turnaround[i];
    }

    printf("\nTempo medio de espera   = %.2lf\n", avg_wait / n);
    printf("Tempo medio de resposta = %.2lf\n", avg_turn / n);

    return 0;
}
