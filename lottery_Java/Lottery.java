package lottery;

import java.util.Random;

/**
 * Lottery Scheduling
 *
 * Cada processo recebe um numero de "tickets" proporcional a sua
 * demanda restante de CPU. A cada rodada, um ticket eh sorteado e o
 * processo vencedor executa por um quantum. Quanto mais tickets,
 * maior a probabilidade de ser escolhido.
 *
 * Compilar: javac -d . Lottery.java
 * Executar: java lottery.Lottery
 */
public class Lottery {

    private static final int N        = 10; // numero de processos
    private static final int MAX_ARRV = 30;
    private static final int MAX_REQ  = 20;
    private static final int MAX_QUAN = 8;

    public static void main(String[] args) {
        Random rand = new Random();

        char[] process = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j' };
        int[]  arrival = new int[N];
        int[]  request = new int[N];

        // gera tempos de chegada e demandas de CPU aleatorios
        for (int z = 0; z < N; z++) {
            arrival[z] = rand.nextInt(MAX_ARRV);
            int r = rand.nextInt(MAX_REQ);
            request[z] = (r == 0) ? 5 : r; // evita processo de duracao 0
        }

        // ordena por arrival (bubble sort estavel)
        for (int i = 0; i < N - 1; i++) {
            for (int j = 0; j < N - 1 - i; j++) {
                if (arrival[j] > arrival[j + 1]) {
                    int tmpA = arrival[j]; arrival[j] = arrival[j + 1]; arrival[j + 1] = tmpA;
                    char tmpP = process[j]; process[j] = process[j + 1]; process[j + 1] = tmpP;
                }
            }
        }

        int quantum = rand.nextInt(MAX_QUAN);
        if (quantum == 0) quantum = 5;

        // mostra estado inicial
        System.out.println("Estado inicial:");
        for (int i = 0; i < N; i++) {
            System.out.printf("\tprocess: %c\tarrival: %2d\trequest: %2d%n",
                    process[i], arrival[i], request[i]);
        }
        System.out.println("quantum: " + quantum);
        System.out.println("---------------------------------------------------------");
        System.out.printf("%6s%10s%10s%10s%10s%n", "time", "winner", "request", "tickets", "total");
        System.out.println("---------------------------------------------------------");

        int time = arrival[0];

        // loop principal: roda enquanto houver pelo menos um processo com request > 0
        while (somaRequests(request) > 0) {

            // calcula tickets de cada processo: proporcional ao request restante
            // (apenas processos que ja chegaram participam)
            int[] tickets = new int[N];
            int totalTickets = 0;
            for (int i = 0; i < N; i++) {
                if (request[i] > 0 && arrival[i] <= time) {
                    tickets[i] = Math.max(1, request[i] / 2);
                    totalTickets += tickets[i];
                }
            }

            // se nenhum processo chegou ainda, adianta o tempo ate o proximo
            if (totalTickets == 0) {
                int proximaChegada = Integer.MAX_VALUE;
                for (int i = 0; i < N; i++) {
                    if (request[i] > 0 && arrival[i] > time && arrival[i] < proximaChegada) {
                        proximaChegada = arrival[i];
                    }
                }
                if (proximaChegada == Integer.MAX_VALUE) break;
                time = proximaChegada;
                continue;
            }

            // sorteia um ticket entre [0, totalTickets)
            int winnerTicket = rand.nextInt(totalTickets);
            int acumulado = 0;
            int winner = -1;
            for (int i = 0; i < N; i++) {
                if (tickets[i] > 0) {
                    acumulado += tickets[i];
                    if (winnerTicket < acumulado) {
                        winner = i;
                        break;
                    }
                }
            }

            // executa o vencedor por um quantum (ou menos se for terminar)
            int consumido = Math.min(quantum, request[winner]);
            request[winner] -= consumido;
            time += consumido;

            System.out.printf("%6d%10c%10d%10d%10d%n",
                    time, process[winner], request[winner], tickets[winner], totalTickets);
        }

        System.out.println("---------------------------------------------------------");
        System.out.println("Todos os processos finalizados em t = " + time);
    }

    private static int somaRequests(int[] r) {
        int s = 0;
        for (int v : r) s += v;
        return s;
    }
}
