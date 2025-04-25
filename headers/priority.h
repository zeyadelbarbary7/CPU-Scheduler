#ifndef PRIORITY_H
#define PRIORITY_H

#include <QVector>
#include <QString>
#include <QPair>
#include <QMap>
#include "process.h"

struct PriorityResult {
    QVector<QPair<QString,int>> gantt;
    double averageWT;
    double averageTAT;
};



class Priority {
public:
    // QVector<QPair<QString, int>> runNonPreemptive(QVector<Process*> processes, bool live = false);
    // QVector<QPair<QString, int>> runPreemptive(QVector<Process*> processes, bool live = false);
    void runPreemptive(bool& stopFlag, QVector<QPair<QString, int>>& ganttData, bool live);
    void runNonPreemptive(bool& stopFlag, QVector<QPair<QString,int>>& ganttData, bool live);
    PriorityResult runNonPreemptiveWithStats(QVector<Process*> processes, bool live = false);
    PriorityResult runPreemptiveWithStats(QVector<Process*> processes, bool live = false);

    void runPreemptiveLiveUpdated(bool& stopFlag,QVector<QPair<QString,int>>& ganttData,bool live,double& avgWT,double& avgTAT);
    void runNonPreemptiveLiveUpdated(bool& stopFlag,QVector<QPair<QString,int>>& ganttData,bool live,double& avgWT,double& avgTAT);
    QVector<Process*> waitingList;
    QVector<Process*> readyQueue;
    QVector<Process*> allProcesses;
    void setLastTickTime(int time) { lastTickTime = time; }
    int getLastTickTime() const { return lastTickTime; }
    QMap<QString, int> remainingBursts; // ✅ Stores remaining burst time for live preemptive



private:
    void moveToReady(QVector<Process*>& waitingList, QVector<Process*>& readyQueue, int currentTime);
    int lastTickTime = 0;  // ✅ stores current tick for dynamic processes

};

#endif // PRIORITY_H
