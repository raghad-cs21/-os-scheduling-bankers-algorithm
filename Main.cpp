#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Process {
    int id;
    int arrival;
    int burst;
    int priority;
    int start;
    int completion;
    int waiting;
    int turnaround;
    int remaining;
};

// ====== Helper to print results ======
void printResults(const vector<Process>& procs, const string& algoName) {
    cout << "\n=== " << algoName << " ===\n";
    cout << "ID\tAT\tBT\tPR\tST\tCT\tWT\tTAT\n";
    double totalWT = 0, totalTAT = 0;
    for (auto p : procs) {
        cout << "P" << p.id << "\t" << p.arrival << "\t" << p.burst << "\t"
             << p.priority << "\t" << p.start << "\t" << p.completion << "\t"
             << p.waiting << "\t" << p.turnaround << "\n";
        totalWT += p.waiting;
        totalTAT += p.turnaround;
    }
    cout << "Average Waiting Time = " << totalWT / procs.size() << "\n";
    cout << "Average Turnaround Time = " << totalTAT / procs.size() << "\n";
}

// ====== FCFS ======
void fcfs(vector<Process> procs) {
    // sort by arrival then id
    sort(procs.begin(), procs.end(), [](const Process& a, const Process& b) {
        if (a.arrival == b.arrival) return a.id < b.id;
        return a.arrival < b.arrival;
    });
    int time = 0;
    for (auto &p : procs) {
        if (time < p.arrival) time = p.arrival;
        p.start = time;
        p.completion = time + p.burst;
        p.turnaround = p.completion - p.arrival;
        p.waiting = p.turnaround - p.burst;
        time = p.completion;
    }
    printResults(procs, "FCFS");
}

// ====== SJF (Non-Preemptive) ======
void sjf(vector<Process> procs) {
    int n = procs.size();
    int completed = 0, time = 0;
    vector<bool> done(n, false);

    cout << "\nGantt (SJF): ";
    while (completed < n) {
        int idx = -1;
        int minBT = 1e9;

        for (int i = 0; i < n; i++) {
            if (!done[i] && procs[i].arrival <= time && procs[i].burst < minBT) {
                minBT = procs[i].burst;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        procs[idx].start = time;
        cout << "| P" << procs[idx].id << " ";
        time += procs[idx].burst;
        procs[idx].completion = time;
        procs[idx].turnaround = procs[idx].completion - procs[idx].arrival;
        procs[idx].waiting = procs[idx].turnaround - procs[idx].burst;
        done[idx] = true;
        completed++;
    }
    cout << "|\n";

    printResults(procs, "SJF (Non-Preemptive)");
}

// ====== Priority (Non-Preemptive) ======
void priorityNonPreemptive(vector<Process> procs) {
    int n = procs.size();
    int completed = 0, time = 0;
    vector<bool> done(n, false);

    cout << "\nGantt (Priority Non-Preemptive): ";
    while (completed < n) {
        int idx = -1;
        int bestPr = 1e9;

        for (int i = 0; i < n; i++) {
            if (!done[i] && procs[i].arrival <= time && procs[i].priority < bestPr) {
                bestPr = procs[i].priority;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        procs[idx].start = time;
        cout << "| P" << procs[idx].id << " ";
        time += procs[idx].burst;
        procs[idx].completion = time;
        procs[idx].turnaround = procs[idx].completion - procs[idx].arrival;
        procs[idx].waiting = procs[idx].turnaround - procs[idx].burst;
        done[idx] = true;
        completed++;
    }
    cout << "|\n";

    printResults(procs, "Priority (Non-Preemptive)");
}

// ====== Priority (Preemptive) ======
void priorityPreemptive(vector<Process> procs) {
    int n = procs.size();
    int completed = 0, time = 0;
    for (auto &p : procs) p.remaining = p.burst;

    cout << "\nGantt (Priority Preemptive): ";
    int lastId = -1;

    while (completed < n) {
        int idx = -1;
        int bestPr = 1e9;

        for (int i = 0; i < n; i++) {
            if (procs[i].arrival <= time && procs[i].remaining > 0 &&
                procs[i].priority < bestPr) {
                bestPr = procs[i].priority;
                idx = i;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        if (procs[idx].remaining == procs[idx].burst) {
            procs[idx].start = time; // first time to run
        }

        if (lastId != procs[idx].id) {
            cout << "| P" << procs[idx].id << " ";
            lastId = procs[idx].id;
        }

        procs[idx].remaining--;
        time++;

        if (procs[idx].remaining == 0) {
            procs[idx].completion = time;
            procs[idx].turnaround = procs[idx].completion - procs[idx].arrival;
            procs[idx].waiting = procs[idx].turnaround - procs[idx].burst;
            completed++;
        }
    }
    cout << "|\n";

    printResults(procs, "Priority (Preemptive)");
}

// ====== Round Robin ======
void roundRobin(vector<Process> procs, int quantum) {
    int n = procs.size();
    for (auto &p : procs) p.remaining = p.burst;

    int time = 0;
    int completed = 0;
    cout << "\nGantt (Round Robin): ";
    bool doneFlag;

    while (completed < n) {
        doneFlag = true;
        for (int i = 0; i < n; i++) {
            if (procs[i].arrival <= time && procs[i].remaining > 0) {
                doneFlag = false;

                if (procs[i].remaining == procs[i].burst) {
                    procs[i].start = (procs[i].start == 0 && time > 0) ? procs[i].start : time;
                }

                cout << "| P" << procs[i].id << " ";

                if (procs[i].remaining > quantum) {
                    time += quantum;
                    procs[i].remaining -= quantum;
                } else {
                    time += procs[i].remaining;
                    procs[i].remaining = 0;
                    procs[i].completion = time;
                    procs[i].turnaround = procs[i].completion - procs[i].arrival;
                    procs[i].waiting = procs[i].turnaround - procs[i].burst;
                    completed++;
                }
            } else if (procs[i].arrival > time && procs[i].remaining > 0) {
                // CPU idle until next arrival
                if (doneFlag) {
                    time = procs[i].arrival;
                    doneFlag = false;
                    i--; // re-check this process after time jump
                }
            }
        }
        if (doneFlag) break;
    }
    cout << "|\n";

    printResults(procs, "Round Robin");
}

// ====== Part 2: Banker's Algorithm ======
bool isSafeState(vector<vector<int>>& maxNeed,
                 vector<vector<int>>& alloc,
                 vector<int>& avail,
                 vector<int>& safeSeq) {
    int n = alloc.size();      // processes
    int m = avail.size();      // resources

    // Need = Max - Allocation
    vector<vector<int>> need(n, vector<int>(m));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = maxNeed[i][j] - alloc[i][j];

    vector<int> work = avail;
    vector<bool> finish(n, false);
    int count = 0;

    while (count < n) {
        bool found = false;
        for (int i = 0; i < n; i++) {
            if (!finish[i]) {
                bool canRun = true;
                for (int j = 0; j < m; j++) {
                    if (need[i][j] > work[j]) {
                        canRun = false;
                        break;
                    }
                }
                if (canRun) {
                    for (int j = 0; j < m; j++)
                        work[j] += alloc[i][j];
                    safeSeq.push_back(i);
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (!found) {
            return false; // no safe sequence
        }
    }
    return true;
}

void bankersAlgorithm() {
    int n, m;
    cout << "\nEnter number of processes: ";
    cin >> n;
    cout << "Enter number of resource types: ";
    cin >> m;

    vector<vector<int>> maxNeed(n, vector<int>(m));
    vector<vector<int>> alloc(n, vector<int>(m));
    vector<int> avail(m);

    cout << "\nEnter MAX matrix (row by row):\n";
    for (int i = 0; i < n; i++) {
        cout << "Process " << i << ": ";
        for (int j = 0; j < m; j++) cin >> maxNeed[i][j];
    }

    cout << "\nEnter ALLOCATION matrix (row by row):\n";
    for (int i = 0; i < n; i++) {
        cout << "Process " << i << ": ";
        for (int j = 0; j < m; j++) cin >> alloc[i][j];
    }

    cout << "\nEnter AVAILABLE vector: ";
    for (int j = 0; j < m; j++) cin >> avail[j];

    vector<int> safeSeq;
    if (isSafeState(maxNeed, alloc, avail, safeSeq)) {
        cout << "\nSystem is in a SAFE state.\nSafe sequence: ";
        for (int i = 0; i < (int)safeSeq.size(); i++) {
            cout << "P" << safeSeq[i];
            if (i != (int)safeSeq.size() - 1) cout << " -> ";
        }
        cout << "\n";
    } else {
        cout << "\nSystem is in an UNSAFE state (no safe sequence).\n";
    }
}

// ====== Input for Part 1: Scheduling ======
void runScheduling() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;
    vector<Process> procs(n);

    for (int i = 0; i < n; i++) {
        procs[i].id = i;
        cout << "\nProcess P" << i << ":\n";
        cout << "Arrival Time: ";
        cin >> procs[i].arrival;
        cout << "Burst Time: ";
        cin >> procs[i].burst;
        cout << "Priority (smaller = higher priority): ";
        cin >> procs[i].priority;
        procs[i].start = 0;
        procs[i].completion = 0;
        procs[i].waiting = 0;
        procs[i].turnaround = 0;
        procs[i].remaining = procs[i].burst;
    }

    int quantum;
    cout << "\nEnter Time Quantum for Round Robin: ";
    cin >> quantum;

    // Run all algorithms
    fcfs(procs);
    sjf(procs);
    priorityNonPreemptive(procs);
    priorityPreemptive(procs);
    roundRobin(procs, quantum);
}

// ====== main menu ======
int main() {
    int choice;
    while (true) {
        cout << "\n===== Operating Systems Project =====\n";
        cout << "1. CPU Scheduling Simulation\n";
        cout << "2. Banker's Algorithm Simulation\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            runScheduling();
        } else if (choice == 2) {
            bankersAlgorithm();
        } else if (choice == 0) {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}