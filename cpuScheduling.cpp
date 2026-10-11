#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <string>
#include <random>
#include <chrono>

using namespace std;

struct Process {
    int id;
    int arrival;
    int burst;
    int remaining;
    int completion;
    int waiting;
    int turnaround;
};

struct Result {
    string name;
    double avg_wt;
    double avg_tat;
};

class CPUScheduler {
public:
    Result fcfs(vector<Process> p) {
        int time = 0;
        double total_wt = 0, total_tat = 0;
        int n = p.size();

        for (auto& proc : p) {
            if (time < proc.arrival) time = proc.arrival;
            proc.completion = time + proc.burst;
            proc.turnaround = proc.completion - proc.arrival;
            proc.waiting = proc.turnaround - proc.burst;
            time = proc.completion;
            
            total_wt += proc.waiting;
            total_tat += proc.turnaround;
        }
        return {"FCFS", total_wt / n, total_tat / n};
    }

    Result sjf(vector<Process> p) {
        int time = 0, completed = 0, n = p.size();
        vector<bool> done(n, false);
        double total_wt = 0, total_tat = 0;

        while (completed < n) {
            int idx = -1;
            int min_burst = 1e9;
            for (int i = 0; i < n; i++) {
                if (!done[i] && p[i].arrival <= time && p[i].burst < min_burst) {
                    min_burst = p[i].burst;
                    idx = i;
                }
            }
            if (idx == -1) {
                time++;
            } else {
                time += p[idx].burst;
                p[idx].completion = time;
                p[idx].turnaround = p[idx].completion - p[idx].arrival;
                p[idx].waiting = p[idx].turnaround - p[idx].burst;
                done[idx] = true;
                completed++;

                total_wt += p[idx].waiting;
                total_tat += p[idx].turnaround;
            }
        }
        return {"SJF (Non-Preemptive)", total_wt / n, total_tat / n};
    }

    Result srtf(vector<Process> p) {
        int time = 0, completed = 0, n = p.size();
        for (auto& proc : p) proc.remaining = proc.burst;
        double total_wt = 0, total_tat = 0;

        while (completed < n) {
            int idx = -1;
            int min_remaining = 1e9;

            for (int i = 0; i < n; i++) {
                if (p[i].arrival <= time && p[i].remaining > 0) {
                    if (p[i].remaining < min_remaining) {
                        min_remaining = p[i].remaining;
                        idx = i;
                    }
                }
            }

            if (idx == -1) {
                time++;
            } else {
                p[idx].remaining--;
                time++;

                if (p[idx].remaining == 0) {
                    completed++;
                    p[idx].completion = time;
                    p[idx].turnaround = p[idx].completion - p[idx].arrival;
                    p[idx].waiting = p[idx].turnaround - p[idx].burst;

                    total_wt += p[idx].waiting;
                    total_tat += p[idx].turnaround;
                }
            }
        }
        return {"SRTF (Preemptive)", total_wt / n, total_tat / n};
    }

    Result roundRobin(vector<Process> p, int quantum) {
        int time = 0, completed = 0, n = p.size();
        for (auto& proc : p) proc.remaining = proc.burst;
        
        queue<int> q;
        vector<bool> in_queue(n, false);
        double total_wt = 0, total_tat = 0;

        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= time) {
                q.push(i);
                in_queue[i] = true;
            }
        }

        while (completed < n) {
            if (q.empty()) {
                time++;
                for (int i = 0; i < n; i++) {
                    if (!in_queue[i] && p[i].arrival <= time) {
                        q.push(i);
                        in_queue[i] = true;
                    }
                }
                continue;
            }

            int idx = q.front();
            q.pop();

            int exec_time = min(p[idx].remaining, quantum);
            p[idx].remaining -= exec_time;
            time += exec_time;

            for (int i = 0; i < n; i++) {
                if (!in_queue[i] && p[i].arrival <= time) {
                    q.push(i);
                    in_queue[i] = true;
                }
            }

            if (p[idx].remaining > 0) {
                q.push(idx);
            } else {
                completed++;
                p[idx].completion = time;
                p[idx].turnaround = p[idx].completion - p[idx].arrival;
                p[idx].waiting = p[idx].turnaround - p[idx].burst;
                
                total_wt += p[idx].waiting;
                total_tat += p[idx].turnaround;
            }
        }
        return {"Round Robin (Q=2)", total_wt / n, total_tat / n};
    }
};

vector<Process> generateTestCases(int num_processes) {
    vector<Process> p(num_processes);
    
    // Use high-resolution clock seed to ensure true randomness on every run
    unsigned seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    
    uniform_int_distribution<> arrival_dist(0, 10);
    uniform_int_distribution<> burst_dist(1, 10);

    for (int i = 0; i < num_processes; i++) {
        p[i].id = i + 1;
        p[i].arrival = arrival_dist(gen);
        p[i].burst = burst_dist(gen);
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        p[i].waiting = 0;
        p[i].turnaround = 0;
    }

    sort(p.begin(), p.end(), [](const Process& a, const Process& b) {
        return a.arrival < b.arrival;
    });

    return p;
}

int main() {
    int num_processes = 5;
    vector<Process> processes = generateTestCases(num_processes);

    cout << "=================================================" << endl;
    cout << "         GENERATED RANDOM TEST CASES            " << endl;
    cout << "=================================================" << endl;
    cout << left << setw(12) << "Process ID" 
         << setw(15) << "Arrival Time" 
         << setw(15) << "Burst Time" << endl;
    cout << "-------------------------------------------------" << endl;
    for (const auto& p : processes) {
        cout << left << setw(12) << ("P" + to_string(p.id))
             << setw(15) << p.arrival 
             << setw(15) << p.burst << endl;
    }

    CPUScheduler scheduler;

    vector<Result> results;
    results.push_back(scheduler.fcfs(processes));
    results.push_back(scheduler.sjf(processes));
    results.push_back(scheduler.srtf(processes));
    results.push_back(scheduler.roundRobin(processes, 2));

    cout << "\n=================================================" << endl;
    cout << "         CPU SCHEDULER PERFORMANCE COMPARISON     " << endl;
    cout << "=================================================" << endl;
    cout << left << setw(22) << "Algorithm" 
         << setw(18) << "Avg Waiting Time" 
         << setw(18) << "Avg Turnaround" << endl;
    cout << "-------------------------------------------------" << endl;

    for (const auto& r : results) {
        cout << left << setw(22) << r.name 
             << setw(18) << fixed << setprecision(2) << r.avg_wt 
             << setw(18) << fixed << setprecision(2) << r.avg_tat << endl;
    }

    cout << "=================================================" << endl;

    return 0;
}