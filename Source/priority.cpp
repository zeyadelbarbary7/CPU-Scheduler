#include "priority.h"
#include "ganttchartwindow.h"
#include <algorithm>
#include <QDebug>
#include <QThread>

void Priority::moveToReady(QVector<Process*>& waitingList, QVector<Process*>& readyQueue, int currentTime) {
    qDebug() << "Ana bn2l aho";
    for (auto it = waitingList.begin(); it != waitingList.end();) {
        if ((*it)->getArrivalTime() <= currentTime) {
            readyQueue.push_back(*it);
            it = waitingList.erase(it);
        } else {
            ++it;
        }
    }
}

void Priority::runNonPreemptive(bool& stopFlag, QVector<QPair<QString,int>>& ganttData, bool live)
{
    int currentTime = 0;
    Process* current = nullptr;
    int remaining = 0;

    // loop until asked to stop AND no work remains
    while (!stopFlag && ( current || !waitingList.isEmpty() || !readyQueue.isEmpty() )) {

        // bring newly-arrived into readyQueue
        moveToReady(waitingList, readyQueue, currentTime);

        // if no current job, pick next by priority
        if (!current) {
            if (!readyQueue.isEmpty()) {
                std::sort(readyQueue.begin(), readyQueue.end(), [](auto a, auto b){
                    return a->getPriority() < b->getPriority();
                });
                current = readyQueue.front();
                readyQueue.pop_front();
                remaining = current->getBurstTime();
            }
        }

        // if we have a job, run one tick
        if (current) {
            ganttData.append(qMakePair(QString::fromStdString(current->getName()), 1));
            --remaining;
            if (live) QThread::sleep(1);
            ++currentTime;

            // if it just finished, clear current so next tick picks new one
            if (remaining == 0) {
                current = nullptr;
            }
        }
        else {
            // idle tick
            ganttData.append(qMakePair(QString("IDLE"), 1));
            if (live) QThread::sleep(1);
            ++currentTime;
        }
    }
}

void Priority::runPreemptive(bool& stopFlag, QVector<QPair<QString, int>>& ganttData, bool live)
{
    int currentTime = 0;

    while (!stopFlag && (!waitingList.isEmpty() || !readyQueue.isEmpty())) {
        moveToReady(waitingList, readyQueue, currentTime);

        if (!readyQueue.isEmpty()) {
            // Sort ready queue by priority every time
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
                return a->getPriority() < b->getPriority();
            });

            // Always choose the highest-priority process
            Process* current = readyQueue.front();
            current->setBurstTime(current->getBurstTime() - 1);
            ganttData.append(qMakePair(QString::fromStdString(current->getName()), 1));

            // If process is done, remove it
            if (current->getBurstTime() == 0) {
                readyQueue.pop_front();
            }

            if (live) QThread::sleep(1);
        } else {
            if (live) QThread::sleep(1);
        }

        currentTime++;
    }
}

void Priority::runNonPreemptiveLiveUpdated(bool& stopFlag,
                                QVector<QPair<QString,int>>& ganttData,
                                bool live,
                                double& avgWT,
                                double& avgTAT)
{
    int currentTime = 0;
    Process* current = nullptr;
    int remaining = 0;

    // Inside runNonPreemptiveLiveUpdated
    for (Process* p : allProcesses) {
        remainingBursts[QString::fromStdString(p->getName())] = p->getBurstTime();
    }


    // loop until asked to stop AND no work remains
    while (!stopFlag && (current || !waitingList.isEmpty() || !readyQueue.isEmpty())) {

        // bring newly-arrived into readyQueue
        moveToReady(waitingList, readyQueue, currentTime);

        // if no current job, pick next by priority
        if (!current) {
            if (!readyQueue.isEmpty()) {
                std::sort(readyQueue.begin(), readyQueue.end(), [](auto a, auto b){
                    return a->getPriority() < b->getPriority();
                });
                current = readyQueue.front();
                readyQueue.pop_front();
                remaining = current->getBurstTime();
            }
        }

        // if we have a job, run one tick
        if (current) {
            ganttData.append(qMakePair(QString::fromStdString(current->getName()), 1));
            --remaining;
            QString name = QString::fromStdString(current->getName());
            remainingBursts[name]--;
            current->setBurstTime(remainingBursts[name]);  // optional, sync Process object

            if (live) QThread::sleep(1);
            ++currentTime;

            // if it just finished, clear current so next tick picks new one
            if (remaining == 0) {
                current->setCompletionTime(currentTime);  // ✅ Track when it finishes
                current = nullptr;
            }
        } else {
            // idle tick
            ganttData.append(qMakePair(QString("IDLE"), 1));
            if (live) QThread::sleep(1);
            ++currentTime;
        }
    }

    // ✅ Final stats calculation
    double totalWT = 0, totalTAT = 0;
    int n = allProcesses.size();  // use all original processes

    for (Process* p : allProcesses) {
        int arrival = p->getArrivalTime();
        int burst   = p->getBurstTime();
        int completion = p->getCompletionTime();

        int tat = completion - arrival;
        int wt  = tat - burst;

        totalTAT += tat;
        totalWT  += wt;
    }

    avgWT  = n ? totalWT / n : 0;
    avgTAT = n ? totalTAT / n : 0;
}

void Priority::runPreemptiveLiveUpdated(bool& stopFlag,
                                        QVector<QPair<QString,int>>& ganttData,
                                        bool live,
                                        double& avgWT,
                                        double& avgTAT)
{
    int currentTime = 0;
    // QMap<QString, int> remainingBursts;
    QMap<QString, int> completionTimes;

    // Initialize burst tracking from allProcesses
    for (Process* p : allProcesses) {
        remainingBursts[QString::fromStdString(p->getName())] = p->getBurstTime();
    }

    while (!stopFlag && (!waitingList.isEmpty() || !readyQueue.isEmpty())) {
        moveToReady(waitingList, readyQueue, currentTime);

        if (!readyQueue.isEmpty()) {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
                return a->getPriority() < b->getPriority();
            });

            Process* current = readyQueue.front();
            QString name = QString::fromStdString(current->getName());

            ganttData.append(qMakePair(name, 1));
            remainingBursts[name]--;
            current->setBurstTime(remainingBursts[name]);

            if (remainingBursts[name] == 0) {
                current->setCompletionTime(currentTime + 1); // ✅ Store actual finish time
                readyQueue.pop_front();
            }

            if (live) QThread::sleep(1);
        } else {
            ganttData.append(qMakePair("IDLE", 1));
            if (live) QThread::sleep(1);
        }

        currentTime++;
        this->setLastTickTime(currentTime);  // ✅ UPDATE tick time for dynamic arrival

    }

    // ✅ Final stats calculation
    double totalWT = 0, totalTAT = 0;
    int n = allProcesses.size();

    for (Process* p : allProcesses) {
        QString name = QString::fromStdString(p->getName());
        int arrival = p->getArrivalTime();
        int burst   = p->getBurstTime();
        int comp    = p->getCompletionTime();
        int tat     = comp - arrival;
        int wt      = tat - burst;

        totalTAT += tat;
        totalWT  += wt;
    }

    avgWT  = n ? totalWT / n : 0;
    avgTAT = n ? totalTAT / n : 0;
}


PriorityResult Priority::runNonPreemptiveWithStats(QVector<Process*> processes, bool live)
{
    QVector<QPair<QString,int>> ganttData;
    bool dummyStop = false;

    // prepare internal queues
    this->waitingList = processes;
    this->readyQueue.clear();

    int currentTime = 0;
    Process* current = nullptr;
    int remaining = 0;

    // run the same scheduling logic as in runNonPreemptive
    while (!dummyStop && ( current || !waitingList.isEmpty() || !readyQueue.isEmpty() )) {

        moveToReady(waitingList, readyQueue, currentTime);

        if (!current && !readyQueue.isEmpty()) {
            std::sort(readyQueue.begin(), readyQueue.end(), [](auto a, auto b){
                return a->getPriority() < b->getPriority();
            });
            current = readyQueue.front();
            readyQueue.pop_front();
            remaining = current->getBurstTime();
        }

        if (current) {
            ganttData.append(qMakePair(QString::fromStdString(current->getName()), 1));
            --remaining;
            if (live) QThread::sleep(1);
            ++currentTime;

            if (remaining == 0) {
                current->setCompletionTime(currentTime);
                current = nullptr;
            }
        } else {
            ganttData.append(qMakePair("IDLE", 1));
            if (live) QThread::sleep(1);
            ++currentTime;
        }
    }

    // calculate AWT and ATAT
    double totalWT = 0, totalTAT = 0;
    int n = processes.size();
    for (Process* p : processes) {
        int arrival = p->getArrivalTime();
        int burst   = p->getBurstTime();
        int comp    = p->getCompletionTime();
        int tat = comp - arrival;
        int wt  = tat - burst;

        totalTAT += tat;
        totalWT  += wt;
    }

    return { ganttData, totalWT / n, totalTAT / n };
}


PriorityResult Priority::runPreemptiveWithStats(QVector<Process*> processes, bool live)
{
    QVector<QPair<QString, int>> ganttData;
    QVector<Process*> waitingList = processes;
    QVector<Process*> readyQueue;

    int currentTime = 0;
    QMap<QString, int> remainingBursts;
    QMap<QString, int> completionTimes;

    // initialize burst tracking
    for (auto* p : processes) {
        remainingBursts[QString::fromStdString(p->getName())] = p->getBurstTime();
    }

    while (!waitingList.isEmpty() || !readyQueue.isEmpty()) {
        // move arriving processes to ready queue
        for (auto it = waitingList.begin(); it != waitingList.end(); ) {
            if ((*it)->getArrivalTime() <= currentTime) {
                readyQueue.append(*it);
                it = waitingList.erase(it);
            } else {
                ++it;
            }
        }

        if (!readyQueue.isEmpty()) {
            std::sort(readyQueue.begin(), readyQueue.end(), [](Process* a, Process* b) {
                return a->getPriority() < b->getPriority();
            });

            Process* current = readyQueue.front();
            QString name = QString::fromStdString(current->getName());

            ganttData.append(qMakePair(name, 1));
            remainingBursts[name]--;

            if (remainingBursts[name] == 0) {
                completionTimes[name] = currentTime + 1;
                readyQueue.pop_front();
            }

            if (live) QThread::sleep(1);
        } else {
            ganttData.append(qMakePair("IDLE", 1));
            if (live) QThread::sleep(1);
        }

        currentTime++;
    }

    // calculate AWT and ATAT
    double totalWT = 0, totalTAT = 0;
    int n = processes.size();

    for (auto* p : processes) {
        QString name = QString::fromStdString(p->getName());
        int arrival = p->getArrivalTime();
        int burst = p->getBurstTime();
        int completion = completionTimes.value(name, arrival + burst);
        int tat = completion - arrival;
        int wt  = tat - burst;
        totalTAT += tat;
        totalWT  += wt;
    }

    return { ganttData, totalWT / n, totalTAT / n };
}

