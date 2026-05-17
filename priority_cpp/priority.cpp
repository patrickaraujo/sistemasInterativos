// Priority Scheduling (nao-preemptivo)
//
// Processos sao executados em ordem decrescente de prioridade.
// Convencao deste programa: MAIOR numero = MAIOR prioridade.
//
// Compilar: g++ -std=c++17 -Wall -o priority priority.cpp
// Executar: ./priority

#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

struct Process {
    int pid;       // ID do processo
    int bt;        // Burst time (tempo de CPU necessario)
    int priority;  // Prioridade
};

// Ordena por prioridade: maior prioridade primeiro
bool comparison(const Process& a, const Process& b) {
    return a.priority > b.priority;
}

// Waiting time: como nao ha preempcao e estamos em ordem de prioridade,
// wt[i] = soma dos burst times dos processos anteriores
void findWaitingTime(const vector<Process>& proc, vector<int>& wt) {
    wt[0] = 0;
    for (size_t i = 1; i < proc.size(); i++) {
        wt[i] = proc[i - 1].bt + wt[i - 1];
    }
}

// Turnaround = burst + waiting
void findTurnAroundTime(const vector<Process>& proc, const vector<int>& wt, vector<int>& tat) {
    for (size_t i = 0; i < proc.size(); i++) {
        tat[i] = proc[i].bt + wt[i];
    }
}

void findavgTime(vector<Process>& proc) {
    int n = (int) proc.size();
    vector<int> wt(n, 0), tat(n, 0);
    int total_wt = 0, total_tat = 0;

    findWaitingTime(proc, wt);
    findTurnAroundTime(proc, wt, tat);

    cout << "\nProcesso\tBurst time\tWaiting time\tTurn around time\n";
    for (int i = 0; i < n; i++) {
        total_wt  += wt[i];
        total_tat += tat[i];
        cout << "   " << proc[i].pid << "\t\t" << proc[i].bt
             << "\t\t" << wt[i] << "\t\t" << tat[i] << "\n";
    }

    cout << "\nTempo medio de espera   = " << (float) total_wt  / n;
    cout << "\nTempo medio de retorno  = " << (float) total_tat / n << "\n";
}

void priorityScheduling(vector<Process>& proc) {
    sort(proc.begin(), proc.end(), comparison);

    cout << "Ordem de execucao dos processos: ";
    for (const auto& p : proc) cout << p.pid << " ";
    cout << "\n";

    findavgTime(proc);
}

int main() {
    vector<Process> proc = { {1, 10, 2}, {2, 5, 0}, {3, 8, 1} };
    priorityScheduling(proc);
    return 0;
}
