#include "fcfs.h"
#include <algorithm>
#include <QThread>
#include <QMap>

bool compareByArrival(Process* a, Process* b)
{
    if (a->getArrivalTime() == b->getArrivalTime())
        return a->getBurstTime() < b->getBurstTime();
    return a->getArrivalTime() < b->getArrivalTime();
}

QVector<QPair<QString, int>> FCFS::FCFSRun(QVector<Process*> processes)
{
    std::sort(processes.begin(), processes.end(), compareByArrival);

    int currentTime = 0;
    QVector<QPair<QString, int>> gantt;

    for (Process* p : processes) {
        int startTime = std::max(currentTime, p->getArrivalTime());
        int endTime = startTime + p->getBurstTime();
        int executionTime = p->getBurstTime();

        for (int t = 0; t < p->getBurstTime(); ++t) {
            gantt.append(qMakePair(QString::fromStdString(p->getName()), 1)); // 1 unit at a time
            currentTime++;
        }
    }

    return gantt;
}

void FCFS::runFCFSLive(bool& stopFlag,QVector<QPair<QString, int>>& gantt,bool live,double& avgWT,double& avgTAT)
{
    QVector<Process*> readyQueue;
    QMap<QString, int> originalBurst;
    QMap<QString, int> startTime;
    QMap<QString, int> completionTime;

    int currentTime = 0;

    // Store initial burst times
    for (Process* p : waitingList) {
        QString name = QString::fromStdString(p->getName());
        remainingBurst[name] = p->getBurstTime();
        originalBurst[name] = p->getBurstTime();
    }

    while (!stopFlag && (!waitingList.isEmpty() || !readyQueue.isEmpty())) {
        // Admit new arrivals
        for (auto it = waitingList.begin(); it != waitingList.end();) {
            if ((*it)->getArrivalTime() <= currentTime) {
                QString name = QString::fromStdString((*it)->getName());
                remainingBurst[name] = (*it)->getBurstTime();
                originalBurst[name] = (*it)->getBurstTime();
                readyQueue.push_back(*it);
                it = waitingList.erase(it);
            } else ++it;
        }

        if (!readyQueue.isEmpty()) {
            Process* cur = readyQueue.front();
            readyQueue.pop_front();

            QString name = QString::fromStdString(cur->getName());

            if (!startTime.contains(name)) {
                startTime[name] = currentTime;
            }

            // Simulate 1 unit of work
            gantt.append(qMakePair(name, 1));
            if (live) QThread::sleep(1);
            currentTime++;

            remainingBurst[name]--;
            if (remainingBurst[name] > 0) {
                readyQueue.push_front(cur);
            } else {
                completionTime[name] = currentTime;
            }


        } else {
            // CPU is idle
            gantt.append(qMakePair("IDLE", 1));
            if (live) QThread::sleep(1);
            currentTime++;
        }

    }

    // Calculate WT and TAT
    double totalWT = 0, totalTAT = 0;
    int count = allProcesses.size();

    for (Process* p : allProcesses) {
        QString name = QString::fromStdString(p->getName());

        int arrival = p->getArrivalTime();
        int burst = originalBurst[name];
        int completion = completionTime[name];

        int tat = completion - arrival;
        int wt = tat - burst;

        totalWT += wt;
        totalTAT += tat;
    }

    // ← **You were missing these assignments**:
    avgWT  = count ? totalWT  / count : 0;
    avgTAT = count ? totalTAT / count : 0;

}



FCFSResult FCFS::FCFSRunWithStats(QVector<Process*> processes) {
    std::sort(processes.begin(), processes.end(), compareByArrival);

    int currentTime = 0;
    double totalWT = 0, totalTAT = 0;
    QVector<QPair<QString, int>> gantt;

    for (Process* p : processes) {
        int startTime = std::max(currentTime, p->getArrivalTime());
        int endTime = startTime + p->getBurstTime();

        int tat = endTime - p->getArrivalTime();       // Turnaround time
        int wt  = tat - p->getBurstTime();              // Waiting time

        totalWT += wt;
        totalTAT += tat;

        for (int t = 0; t < p->getBurstTime(); ++t) {
            gantt.append(qMakePair(QString::fromStdString(p->getName()), 1));
            currentTime++;
        }
    }

    FCFSResult result;
    result.gantt = gantt;
    result.averageWT = totalWT / processes.size();
    result.averageTAT = totalTAT / processes.size();
    return result;
}
