#include "roundrobin.h"
#include <QString>
#include <QThread>
#include <QMap>
#include <queue>
#include "process.h"

void roundRobin::moveToReady(QVector<Process>& temp, std::queue<Process>& q, int currentTime) {
    for (auto it = temp.begin(); it != temp.end();) {
        if (it->getArrivalTime() <= currentTime) {
            q.push(*it);
            it = temp.erase(it);
        } else ++it;
    }
}

void roundRobin::runRRLive(bool& stopFlag,
                           QVector<QPair<QString,int>>& ganttData,
                           bool live,
                           int quantum)
{
    std::queue<Process*> q;
    int currentTime = 0;
    Process* current = nullptr;
    int sliceRem = 0;

    // Seed remaining bursts
    for (Process* p : waitingList) {
        QString name = QString::fromStdString(p->getName());
        remainingBursts[name] = p->getBurstTime();
    }

    while (!stopFlag && ( current || !waitingList.empty() || !q.empty() )) {
        // Admit new arrivals
        for (auto it = waitingList.begin(); it != waitingList.end();) {
            if ((*it)->getArrivalTime() <= currentTime) {
                q.push(*it);
                it = waitingList.erase(it);
            } else {
                ++it;
            }
        }

        // Pull new process if needed
        if (!current && !q.empty()) {
            current = q.front();
            q.pop();
            sliceRem = quantum;
        }

        // Tick
        if (current) {
            QString name = QString::fromStdString(current->getName());
            ganttData.append(qMakePair(name, 1));
            if (live) QThread::sleep(1);
            ++currentTime;

            int remaining = --remainingBursts[name];
            current->setBurstTime(remaining);

            --sliceRem;

            if (remaining == 0) {
                current->setCompletionTime(currentTime);
                current = nullptr;
            } else if (sliceRem == 0) {
                for (auto it = waitingList.begin(); it != waitingList.end();) {
                    if ((*it)->getArrivalTime() <= currentTime) {
                        q.push(*it);
                        it = waitingList.erase(it);
                    } else {
                        ++it;
                    }
                }
                q.push(current);
                current = nullptr;
            }
        } else {
            ganttData.append(qMakePair("IDLE", 1));
            if (live) QThread::sleep(1);
            ++currentTime;
        }
    }

}

void roundRobin::runRRLiveUpdated(bool& stopFlag,
                           QVector<QPair<QString,int>>& ganttData,
                           bool live,
                           int quantum,
                           double& avgWT,
                           double& avgTAT)
{
    std::queue<Process*> q;
    int currentTime = 0;
    Process* current = nullptr;
    int sliceRem = 0;
    allProcesses = waitingList;  // ✅ save all processes for final stats

    QMap<QString, int> completion;
    QMap<QString, int> originalBurst;

    // Seed original burst + remaining
    for (Process* p : waitingList) {
        QString name = QString::fromStdString(p->getName());
        originalBurst[name] = p->getBurstTime();
        remainingBursts[name] = p->getBurstTime();
    }

    while (!stopFlag && ( current || !waitingList.isEmpty() || !q.empty() )) {
        // Admit newly arrived processes
        for (auto it = waitingList.begin(); it != waitingList.end();) {
            if ((*it)->getArrivalTime() <= currentTime) {
                q.push(*it);
                it = waitingList.erase(it);
            } else {
                ++it;
            }
        }

        // If no current process, fetch from queue
        if (!current && !q.empty()) {
            current = q.front();
            q.pop();
            sliceRem = quantum;
        }

        if (current) {
            QString name = QString::fromStdString(current->getName());
            ganttData.append(qMakePair(name, 1));
            if (live) QThread::sleep(1);
            ++currentTime;

            int rem = --remainingBursts[name];
            current->setBurstTime(rem);
            --sliceRem;

            if (rem == 0) {
                current->setCompletionTime(currentTime);
                completion[name] = currentTime;
                current = nullptr;
            } else if (sliceRem == 0) {
                for (auto it = waitingList.begin(); it != waitingList.end();) {
                    if ((*it)->getArrivalTime() <= currentTime) {
                        q.push(*it);
                        it = waitingList.erase(it);
                    } else {
                        ++it;
                    }
                }
                q.push(current);
                current = nullptr;
            }
        } else {
            ganttData.append(qMakePair("IDLE", 1));
            if (live) QThread::sleep(1);
            ++currentTime;
        }
    }

    // Final Stats Calculation
    double totalWT = 0, totalTAT = 0;
    int n = allProcesses.size();

    for (Process* p : allProcesses) {
        QString name = QString::fromStdString(p->getName());
        int arrival = p->getArrivalTime();
        int burst   = originalBurst.value(name, 0);
        int comp    = p->getCompletionTime();

        int tat = comp - arrival;
        int wt  = tat - burst;

        totalTAT += tat;
        totalWT  += wt;
    }

    avgWT = n ? totalWT / n : 0;
    avgTAT = n ? totalTAT / n : 0;
}


RRResult roundRobin::runRRWithStats(const QVector<Process*>& inputProcesses, int quantum) {
    QVector<QPair<QString, int>> gantt;
    std::queue<Process> queue;
    std::vector<Process> tempProcesses;

    QMap<QString, int> completion;
    QMap<QString, int> burstCount;
    QMap<QString, int> originalBurst;

    for (const auto& p : inputProcesses) {
        tempProcesses.push_back(*p); // copy for simulation
        QString name = QString::fromStdString(p->getName());
        originalBurst[name] = p->getBurstTime();
    }

    int currentTime = 0;

    while (!tempProcesses.empty() || !queue.empty()) {
        for (auto it = tempProcesses.begin(); it != tempProcesses.end();) {
            if (it->getArrivalTime() <= currentTime) {
                queue.push(*it);
                it = tempProcesses.erase(it);
            } else {
                ++it;
            }
        }

        if (!queue.empty()) {
            Process p = queue.front();
            queue.pop();

            QString name = QString::fromStdString(p.getName());

            int sliceTime = std::min(quantum, p.getBurstTime());
            gantt.append(qMakePair(name, sliceTime));
            burstCount[name] += sliceTime;

            currentTime += sliceTime;
            p.setBurstTime(p.getBurstTime() - sliceTime);

            if (p.getBurstTime() > 0) {
                for (auto it = tempProcesses.begin(); it != tempProcesses.end();) {
                    if (it->getArrivalTime() <= currentTime) {
                        queue.push(*it);
                        it = tempProcesses.erase(it);
                    } else {
                        ++it;
                    }
                }
                queue.push(p);
            } else {
                completion[name] = currentTime;
            }
        } else {
            gantt.append(qMakePair("IDLE", 1));
            ++currentTime;
        }
    }

    // ✅ Final stats calculation
    double totalWT = 0, totalTAT = 0;
    int n = inputProcesses.size();

    for (const Process* p : inputProcesses) {
        QString name = QString::fromStdString(p->getName());
        int arrival = p->getArrivalTime();
        int burst   = originalBurst.value(name);
        int comp    = completion[name];

        int tat = comp - arrival;
        int wt  = tat - burst;

        totalTAT += tat;
        totalWT  += wt;
    }

    return { gantt, n ? totalWT / n : 0.0, n ? totalTAT / n : 0.0 };
}

// QVector<QPair<QString, int>> roundRobin::runRR(const QVector<Process*>& inputProcesses, int quantum) {
//     QVector<QPair<QString, int>> ganttData;
//     std::queue<Process> queue;
//     std::vector<Process> tempProcesses;

//     for (const auto& p : inputProcesses) {
//         tempProcesses.push_back(*p);
//     }

//     int currentTime = 0;

//     while (!tempProcesses.empty() || !queue.empty()) {
//         for (auto it = tempProcesses.begin(); it != tempProcesses.end();) {
//             if (it->getArrivalTime() <= currentTime) {
//                 queue.push(*it);
//                 it = tempProcesses.erase(it);
//             } else {
//                 ++it;
//             }
//         }

//         if (!queue.empty()) {
//             Process p = queue.front();
//             queue.pop();

//             int sliceTime = std::min(quantum, p.getBurstTime());
//             ganttData.append(qMakePair(QString::fromStdString(p.getName()), sliceTime));

//             currentTime += sliceTime;
//             p.setBurstTime(p.getBurstTime() - sliceTime);

//             if (p.getBurstTime() > 0) {
//                 queue.push(p);
//             }
//         } else {
//             ganttData.append(qMakePair(QString("IDLE"), 1));
//             currentTime++;
//         }
//     }

//     return ganttData;
// }


// void roundRobin::runRRLive(bool& stopFlag,
//                            QVector<QPair<QString,int>>& ganttData,
//                            bool live,
//                            int quantum)
// {
//     std::queue<Process*> q;
//     int currentTime = 0;
//     Process* current = nullptr;
//     int sliceRem = 0;

//     // Continue while someone is still to arrive or finish
//     while (!stopFlag && ( current || !waitingList.empty() || !q.empty() )) {

//         // 1) admit any newly‐arrived processes into the queue
//         for (auto it = waitingList.begin(); it != waitingList.end();) {
//             if ((*it)->getArrivalTime() <= currentTime) {
//                 q.push(*it);
//                 it = waitingList.erase(it);
//             } else {
//                 ++it;
//             }
//         }

//         // 2) if no current slice, grab next from queue
//         if (!current && !q.empty()) {
//             current = q.front();
//             q.pop();
//             sliceRem = quantum;
//         }

//         // 3) execute one unit
//         if (current) {
//             ganttData.append(qMakePair(
//                 QString::fromStdString(current->getName()), 1));
//             if (live) QThread::sleep(1);
//             ++currentTime;

//             current->setBurstTime(current->getBurstTime() - 1);
//             --sliceRem;

//             // if finished, clear current
//             if (current->getBurstTime() == 0) {
//                 current = nullptr;
//             }
//             // if slice used up but not finished, re-queue
//             else if (sliceRem == 0) {
//                 q.push(current);
//                 current = nullptr;
//             }
//         }
//         else {
//             // idle tick
//             ganttData.append(qMakePair(QString("IDLE"), 1));
//             if (live) QThread::sleep(1);
//             ++currentTime;
//         }
//     }
// }

// RRResult roundRobin::runRRWithStats(const QVector<Process*>& inputProcesses, int quantum) {
//     QVector<QPair<QString, int>> gantt;
//     std::queue<Process> queue;
//     std::vector<Process> tempProcesses;

//     QMap<QString, int> completion;
//     QMap<QString, int> burstCount;
//     QMap<QString, int> startTime;  // ✅ Track first time each process starts

//     QMap<QString, int> originalBurst;

//     for (const auto& p : inputProcesses) {
//         tempProcesses.push_back(*p); // copy for simulation
//         originalBurst[QString::fromStdString(p->getName())] = p->getBurstTime(); // ✅ store original burst
//     }

//     int currentTime = 0;

//     while (!tempProcesses.empty() || !queue.empty()) {
//         for (auto it = tempProcesses.begin(); it != tempProcesses.end();) {
//             if (it->getArrivalTime() <= currentTime) {
//                 queue.push(*it);
//                 it = tempProcesses.erase(it);
//             } else {
//                 ++it;
//             }
//         }

//         if (!queue.empty()) {
//             Process p = queue.front();
//             queue.pop();

//             QString name = QString::fromStdString(p.getName());

//             // ✅ If first time this process runs
//             if (!startTime.contains(name)) {
//                 startTime[name] = currentTime;
//             }

//             int sliceTime = std::min(quantum, p.getBurstTime());
//             gantt.append(qMakePair(name, sliceTime));
//             burstCount[name] += sliceTime;

//             currentTime += sliceTime;
//             p.setBurstTime(p.getBurstTime() - sliceTime);

//             if (p.getBurstTime() > 0) {
//                 queue.push(p);
//             } else {
//                 completion[name] = currentTime;
//             }
//         } else {
//             gantt.append(qMakePair("IDLE", 1));
//             ++currentTime;
//         }
//     }

//     // ✅ Calculate correct stats
//     double totalWT = 0, totalTAT = 0;
//     int n = inputProcesses.size();

//     for (const Process* p : inputProcesses) {
//         QString name = QString::fromStdString(p->getName());
//         int arrival = p->getArrivalTime();
//         int burst   = originalBurst.value(name); // ✅ correct, stored before simulation
//         int comp    = completion[name];

//         int tat = comp - arrival;
//         int wt  = tat - burst;

//         totalTAT += tat;
//         totalWT  += wt;
//     }


//     return { gantt, n ? totalWT / n : 0.0, n ? totalTAT / n : 0.0 };
// }

