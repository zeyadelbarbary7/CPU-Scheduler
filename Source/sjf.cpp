#include "sjf.h"
#include <algorithm>
#include <QThread>
#include <QDebug>
#include <QMap>
#include "process.h"

bool compareByArrivalTime(Process* a, Process* b) {
    return a->getArrivalTime() < b->getArrivalTime();
}

// SJF Non-Preemptive implementation
QVector<QPair<QString, int>> SJF::runNonPreemptive(QVector<Process*> processes, bool live) {
    QVector<QPair<QString, int>> gantt;
    QVector<Process*> readyQueue;
    QVector<Process*> processesQueue = processes;

    // Sort processes based on arrival time
    std::sort(processesQueue.begin(), processesQueue.end(), compareByArrivalTime);

    int currentTime = 0;

    while (!processesQueue.isEmpty() || !readyQueue.isEmpty()) {
        // Move arrived processes to ready queue
        for (int i = 0; i < processesQueue.size(); ) {
            if (processesQueue[i]->getArrivalTime() <= currentTime) {
                readyQueue.push_back(processesQueue[i]);
                processesQueue.removeAt(i);
            } else {
                ++i;
            }
        }

        // If ready queue is empty, advance time to next process arrival
        if (readyQueue.isEmpty()) {
            if (!processesQueue.isEmpty()) {
                int idleTime = processesQueue.first()->getArrivalTime() - currentTime;
                if (idleTime > 0) {
                    if (live) {
                        // For live scheduling, advance one time unit at a time
                        gantt.append(qMakePair(QString("IDLE"), 1));
                        QThread::sleep(1);
                        currentTime++;
                    } else {
                        // For non-live scheduling, skip directly to next arrival
                        gantt.append(qMakePair(QString("IDLE"), idleTime));
                        currentTime = processesQueue.first()->getArrivalTime();
                    }
                }
            }
            continue;
        }

        // Sort ready queue by burst time (shortest first)
        std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
            return a->getBurstTime() < b->getBurstTime();
        });

        // Execute the shortest job
        Process* current = readyQueue.takeFirst();
        int burstTime = current->getBurstTime();

        // Update the process state
        current->setRemainingTime(0);
        current->setCompletionTime(currentTime + burstTime);

        // Add to Gantt chart
        if (live) {
            // For live scheduling, add one time unit at a time
            for (int i = 0; i < burstTime; ++i) {
                gantt.append(qMakePair(QString::fromStdString(current->getName()), 1));
                QThread::sleep(1);
                currentTime++;
            }
        } else {
            // For non-live scheduling, add the entire burst at once
            gantt.append(qMakePair(QString::fromStdString(current->getName()), burstTime));
            currentTime += burstTime;
        }
    }

    return gantt;
}


// SJF Preemptive (Shortest Remaining Time First) implementation
QVector<QPair<QString, int>> SJF::runPreemptive(QVector<Process*> processes, bool live) {
    QVector<QPair<QString, int>> gantt;
    QVector<Process*> processesQueue;

    // Create a deep copy of the processes to preserve original burst times
    for (Process* p : processes) {
        Process* copy = new Process(*p); // Use copy constructor instead
        copy->setRemainingTime(p->getBurstTime());
        processesQueue.append(copy);
    }

    // Sort processes based on arrival time
    std::sort(processesQueue.begin(), processesQueue.end(), compareByArrivalTime);

    int currentTime = 0;
    QVector<Process*> readyQueue;
    Process* currentProcess = nullptr;

    while (!processesQueue.isEmpty() || !readyQueue.isEmpty() || currentProcess != nullptr) {
        // Move arrived processes to ready queue
        for (int i = 0; i < processesQueue.size(); ) {
            if (processesQueue[i]->getArrivalTime() <= currentTime) {
                readyQueue.push_back(processesQueue[i]);
                processesQueue.removeAt(i);
            } else {
                ++i;
            }
        }

        // If we have a current process, add it back to ready queue for re-evaluation
        if (currentProcess != nullptr) {
            readyQueue.push_back(currentProcess);
            currentProcess = nullptr;
        }

        // If ready queue is empty, advance time to next process arrival or end simulation
        if (readyQueue.isEmpty()) {
            if (!processesQueue.isEmpty()) {
                int idleTime = processesQueue.first()->getArrivalTime() - currentTime;
                if (idleTime > 0) {
                    QString processName = "IDLE";

                    // Check if we need to merge with previous idle time
                    if (gantt.isEmpty() || gantt.last().first != processName) {
                        gantt.append(qMakePair(processName, 1));
                    } else {
                        gantt.last().second += 1;
                    }

                    if (live) QThread::sleep(1);
                    currentTime++;
                }
            } else {
                // No more processes to execute
                break;
            }
            continue;
        }

        // Sort ready queue by remaining time (shortest first)
        std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
            return a->getRemainingTime() < b->getRemainingTime();
        });

        // Select the process with shortest remaining time
        currentProcess = readyQueue.takeFirst();

        // Execute one time unit
        QString processName = QString::fromStdString(currentProcess->getName());

        // Check if we can merge with previous entry in Gantt chart
        gantt.append(qMakePair(processName, 1));


        // Decrease remaining time
        currentProcess->setRemainingTime(currentProcess->getRemainingTime() - 1);

        // If process is complete, update completion time
        if (currentProcess->getRemainingTime() <= 0) {
            currentProcess->setCompletionTime(currentTime + 1);
            currentProcess = nullptr;
        }

        if (live) QThread::sleep(1);
        currentTime++;
    }

    return gantt;
}

void SJF::runPreemptiveLiveUpdated(bool& stopFlag,
                                   QVector<QPair<QString,int>>& gantt,
                                   bool live,
                                   double& avgWT,
                                   double& avgTAT)
{
    QVector<Process*> readyQueue;
    QMap<QString, int> burstCount;
    QMap<QString, int> completion;

    int currentTime = 0;

    for (Process* p : processes) {
        p->setRemainingTime(p->getBurstTime());
        burstCount[QString::fromStdString(p->getName())] = 0;
    }

    Process* currentProcess = nullptr;

    while (!stopFlag && (!waitingList.isEmpty() || !readyQueue.isEmpty() || currentProcess)) {
        // admit new arrivals
        for (auto it = waitingList.begin(); it != waitingList.end(); ) {
            if ((*it)->getArrivalTime() <= currentTime) {
                readyQueue.push_back(*it);
                it = waitingList.erase(it);
            } else {
                ++it;
            }
        }

        // re-add current process to queue if not done
        if (currentProcess) {
            readyQueue.push_back(currentProcess);
            currentProcess = nullptr;
        }

        // pick process with shortest remaining time
        if (!readyQueue.isEmpty()) {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
                return a->getRemainingTime() < b->getRemainingTime();
            });
            currentProcess = readyQueue.takeFirst();
        } else {
            gantt.append(qMakePair("IDLE", 1));
            if (live) QThread::sleep(1);
            ++currentTime;
            continue;
        }

        QString name = QString::fromStdString(currentProcess->getName());
        gantt.append(qMakePair(name, 1));

        currentProcess->setRemainingTime(currentProcess->getRemainingTime() - 1);
        remainingBursts[name] = currentProcess->getRemainingTime(); // ✅ Live update

        if (currentProcess->getRemainingTime() == 0) {
            completion[name] = currentTime + 1;
            currentProcess = nullptr;
        }


        if (live) QThread::sleep(1);
        ++currentTime;

        this->setLastTickTime(currentTime); // ✅ keep dynamic arrival working
    }

    // ✅ FINAL AWT/ATAT calculation
    double totalWT = 0, totalTAT = 0;
    int n = allProcesses.size();

    for (Process* p : allProcesses) {
        QString name = QString::fromStdString(p->getName());
        int arrival = p->getArrivalTime();
        int burst   = burstCount.value(name, 0);
        int comp    = completion.value(name, arrival + burst);

        int tat = comp - arrival;
        int wt  = tat - burst;

        totalTAT += tat;
        totalWT  += wt;
    }

    avgWT  = n ? totalWT / n : 0;
    avgTAT = n ? totalTAT / n : 0;
}


void SJF::runNonPreemptiveLive(bool& stopFlag, QVector<QPair<QString, int>>& gantt, bool live)
{
    QVector<Process*> readyQueue;
    int currentTime = 0;
    Process* current = nullptr;

    // ✅ FIX: Correct condition so it exits when all lists and current are empty
    while (!stopFlag && (current || !waitingList.isEmpty() || !readyQueue.isEmpty())) {
        // 1. Admit any arriving processes
        for (auto it = waitingList.begin(); it != waitingList.end(); ) {
            if ((*it)->getArrivalTime() <= currentTime) {
                readyQueue.append(*it);
                it = waitingList.erase(it);
            } else {
                ++it;
            }
        }

        // 2. If no process running, pick the shortest job
        if (!current && !readyQueue.isEmpty()) {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
                return a->getBurstTime() < b->getBurstTime();
            });

            current = readyQueue.takeFirst();
        }

        // 3. Execute current process fully
        if (current) {
            for (int i = 0; i < current->getBurstTime(); ++i) {
                gantt.append(qMakePair(QString::fromStdString(current->getName()), 1));
                if (live) QThread::sleep(1);
                ++currentTime;

                // check arrivals during execution
                for (auto it = waitingList.begin(); it != waitingList.end(); ) {
                    if ((*it)->getArrivalTime() <= currentTime) {
                        readyQueue.append(*it);
                        it = waitingList.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            // done with this process
            current = nullptr;
        } else {
            // IDLE
            gantt.append(qMakePair(QString("IDLE"), 1));
            if (live) QThread::sleep(1);
            ++currentTime;
        }
    }
}

void SJF::runNonPreemptiveLiveUpdated(bool& stopFlag,
                                      QVector<QPair<QString, int>>& gantt,
                                      bool live,
                                      double& avgWT,
                                      double& avgTAT)
{
    QVector<Process*> readyQueue;
    int currentTime = 0;
    Process* current = nullptr;

    // Initialize remaining bursts
    for (Process* p : allProcesses) {
        QString name = QString::fromStdString(p->getName());
        remainingBursts[name] = p->getBurstTime();
    }

    while (!stopFlag && (current || !waitingList.isEmpty() || !readyQueue.isEmpty())) {
        // Admit new arrivals
        for (auto it = waitingList.begin(); it != waitingList.end(); ) {
            if ((*it)->getArrivalTime() <= currentTime) {
                readyQueue.append(*it);
                QString name = QString::fromStdString((*it)->getName());
                remainingBursts[name] = (*it)->getBurstTime();
                (*it)->setRemainingTime((*it)->getBurstTime());
                it = waitingList.erase(it);
            } else {
                ++it;
            }
        }

        // Select the shortest burst job from readyQueue
        if (!current && !readyQueue.isEmpty()) {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
                return a->getBurstTime() < b->getBurstTime();
            });
            current = readyQueue.takeFirst();
        }

        if (current) {
            QString name = QString::fromStdString(current->getName());
            int burst = current->getBurstTime();

            // Merge Gantt entries
            if (!gantt.isEmpty() && gantt.last().first == name)
                gantt.last().second += burst;
            else
                gantt.append(qMakePair(name, burst));

            // Run the process
            for (int i = 0; i < burst; ++i) {
                remainingBursts[name]--;
                if (live) QThread::sleep(1);
                ++currentTime;

                // Check if other processes arrive during this process execution
                for (auto it = waitingList.begin(); it != waitingList.end(); ) {
                    if ((*it)->getArrivalTime() <= currentTime) {
                        readyQueue.append(*it);
                        QString newName = QString::fromStdString((*it)->getName());
                        remainingBursts[newName] = (*it)->getBurstTime();
                        (*it)->setRemainingTime((*it)->getBurstTime());
                        it = waitingList.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            current->setCompletionTime(currentTime);
            current = nullptr;
        } else {
            // CPU idle
            if (!gantt.isEmpty() && gantt.last().first == "IDLE")
                gantt.last().second += 1;
            else
                gantt.append(qMakePair("IDLE", 1));

            if (live) QThread::sleep(1);
            ++currentTime;
        }
    }

    // Final statistics
    double totalWT = 0, totalTAT = 0;
    int n = allProcesses.size();

    for (Process* p : allProcesses) {
        int arrival = p->getArrivalTime();
        int burst = p->getBurstTime();
        int completion = p->getCompletionTime();

        int tat = completion - arrival;
        int wt = tat - burst;

        totalWT += wt;
        totalTAT += tat;
    }

    avgWT  = n ? totalWT / n : 0;
    avgTAT = n ? totalTAT / n : 0;
}


void SJF::runPreemptiveLive(bool& stopFlag,
                            QVector<QPair<QString,int>>& gantt,
                            bool live)
{
    // waitingList is your member filled by ganttChartWindow
    QVector<Process*> readyQueue;
    int currentTime = 0;
    for (Process* p : waitingList) {
        p->setRemainingTime(p->getBurstTime());
    }
    // We'll reuse currentProcess between ticks
    Process* currentProcess = nullptr;

    // Run until stopped OR no work remains
    while (!stopFlag && (!waitingList.isEmpty() || !readyQueue.isEmpty() || currentProcess))
    {
        // 1) admit any arrivals into readyQueue
        for (auto it = waitingList.begin(); it != waitingList.end(); ) {
            if ((*it)->getArrivalTime() <= currentTime) {
                readyQueue.push_back(*it);
                it = waitingList.erase(it);
            } else {
                ++it;
            }
        }

        // 2) if we had a process from last tick, put it back for re-sorting
        if (currentProcess) {
            readyQueue.push_back(currentProcess);
            currentProcess = nullptr;
        }

        // 3) pick the shortest‐remaining‐time job
        if (!readyQueue.isEmpty()) {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b){
                return a->getRemainingTime() < b->getRemainingTime();
            });
            currentProcess = readyQueue.takeFirst();
        }
        else {
            // nothing to run → IDLE one tick
            gantt.append(qMakePair(QString("IDLE"),1));
            if (live) QThread::sleep(1);
            ++currentTime;
            continue;
        }

        // 4) execute exactly one time‐unit
        gantt.append(qMakePair(QString::fromStdString(currentProcess->getName()),1));
        currentProcess->setRemainingTime(currentProcess->getRemainingTime() - 1);

        if (live) QThread::sleep(1);
        ++currentTime;

        // 5) if it still has work, it will be re-added next loop iteration
        if (currentProcess->getRemainingTime() > 0) {
            // leave it in currentProcess so next iteration re-enqueues it
        } else {
            // done
            currentProcess = nullptr;
        }
    }

    // after the while(...) loop ends
    stopFlag = true;
    return;

}


SJFResult SJF::runNonPreemptiveWithStats(QVector<Process*> processes, bool live)
{
    QVector<QPair<QString,int>> gantt = runNonPreemptive(processes, live);

    // 1. Build map of burst times (per name)
    QMap<QString,int> burstMap;
    for (auto p : processes) {
        burstMap[QString::fromStdString(p->getName())] = p->getBurstTime();
    }

    // 2. Build map of completion times (you already set this inside runNonPreemptive)
    QMap<QString,int> completionMap;
    for (auto p : processes) {
        completionMap[QString::fromStdString(p->getName())] = p->getCompletionTime();
    }

    // Calculate WT and TAT
    double totalWT = 0, totalTAT = 0;
    int count = processes.size();

    for (Process* p : processes) {
        QString name = QString::fromStdString(p->getName());

        int arrival = p->getArrivalTime();
        int burst = burstMap[name];
        int completion = completionMap[name];

        int tat = completion - arrival;
        int wt = tat - burst;

        totalWT += wt;
        totalTAT += tat;
    }

    return { gantt, totalWT / count, totalTAT / count };
}


SJFResult SJF::runPreemptiveWithStats(QVector<Process*> processes, bool live)
{
    QVector<QPair<QString,int>> gantt = runPreemptive(processes, live);

    // 1. Track when each process finished (last time it appears)
    QMap<QString,int> completion;
    for (int i = 0; i < gantt.size(); ++i) {
        auto &seg = gantt[i];
        if (seg.first != "IDLE") {
            completion[seg.first] = i + 1; // +1 because time ends after execution
        }
    }

    // 2. Track how many time units each process ran
    QMap<QString,int> burstCount;
    for (auto &seg : gantt) {
        if (seg.first != "IDLE")
            burstCount[seg.first] += seg.second;
    }

    // 3. Compute AWT and ATAT
    double totalWT = 0, totalTAT = 0;
    int n = processes.size();

    for (Process* p : processes) {
        QString name = QString::fromStdString(p->getName());
        int arrival = p->getArrivalTime();
        int burst   = burstCount.value(name, 0);           // from Gantt data
        int comp    = completion.value(name, arrival);     // from Gantt data

        int tat = comp - arrival;
        int wt  = tat - burst;

        totalTAT += tat;
        totalWT  += wt;
    }

    return { gantt, totalWT / n, totalTAT / n };
}
