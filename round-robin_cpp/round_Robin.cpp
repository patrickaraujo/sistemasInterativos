// Round Robin Scheduling
//
// Cada processo recebe a CPU por um quantum fixo. Quando o quantum expira,
// volta para o fim da fila ate completar seu burst time.
//
// Compilar: g++ -std=c++17 -Wall -o round_robin round_Robin.cpp
// Executar: ./round_robin

#include <iostream>
#include <vector>
using namespace std;

// Calcula o tempo de espera (waiting time) para todos os processos
void findWaitingTime(const vector<int>& bt, vector<int>& wt, int quantum) {
    int n = (int) bt.size();
    vector<int> rem_bt = bt; // burst times restantes
    int t = 0;               // tempo atual

    // Percorre os processos em round robin ate todos terminarem
    while (true) {
        bool done = true;

        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = false; // ainda ha processo pendente

                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    // ultimo ciclo deste processo
                    t += rem_bt[i];
                    wt[i] = t - bt[i];
                    rem_bt[i] = 0;
                }
            }
        }

        if (done) break;
    }
}

// Tempo de retorno (turnaround) = burst + waiting
void findTurnAroundTime(const vector<int>& bt, const vector<int>& wt, vector<int>& tat) {
    for (size_t i = 0; i < bt.size(); i++) {
        tat[i] = bt[i] + wt[i];
    }
}

// Imprime resultados e calcula medias
void findavgTime(const vector<int>& processes, const vector<int>& bt, int quantum) {
    int n = (int) processes.size();
    vector<int> wt(n, 0), tat(n, 0);
    int total_wt = 0, total_tat = 0;

    findWaitingTime(bt, wt, quantum);
    findTurnAroundTime(bt, wt, tat);

    cout << "Processo\tBurst time\tWaiting time\tTurn around time\n";
    for (int i = 0; i < n; i++) {
        total_wt  += wt[i];
        total_tat += tat[i];
        cout << "   " << processes[i] << "\t\t" << bt[i]
             << "\t\t" << wt[i] << "\t\t" << tat[i] << "\n";
    }

    cout << "\nTempo medio de espera   = " << (float) total_wt  / n;
    cout << "\nTempo medio de retorno  = " << (float) total_tat / n << "\n";
}

int main() {
    vector<int> processes  = { 1, 2, 3 };
    vector<int> burst_time = { 10, 5, 8 };
    int quantum = 2;

    findavgTime(processes, burst_time, quantum);
    return 0;
}
