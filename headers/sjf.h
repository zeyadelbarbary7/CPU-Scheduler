#ifndef SJF_H
#define SJF_H

#include <QVector>
#include <QString>
#include <QPair>
#include <QMap>
#include "process.h"

struct SJFResult {
    QVector<QPair<QString,int>> gantt;
    double averageWT;
    double averageTAT;
};

class SJF {
public:
    QVector<QPair<QString, int>> runNonPreemptive(QVector<Process*> processes, bool live = false);
    QVector<QPair<QString, int>> runPreemptive(QVector<Process*> processes, bool live = false);
    void runNonPreemptiveLive(bool& stopFlag, QVector<QPair<QString, int>>& gantt, bool live);
    void runPreemptiveLive(bool& stopFlag,QVector<QPair<QString,int>>& ganttData,bool live);
    SJFResult runNonPreemptiveWithStats(QVector<Process*> processes, bool live=false);
    SJFResult runPreemptiveWithStats   (QVector<Process*> processes, bool live=false);
    void runNonPreemptiveLiveUpdated(bool& stopFlag, QVector<QPair<QString, int>>& gantt, bool live, double& avgWT, double& avgTAT);
    void runPreemptiveLiveUpdated(bool& stopFlag,QVector<QPair<QString,int>>& gantt,bool live,double& avgWT,double& avgTAT);
    void setLastTickTime(int time) { lastTickTime = time; }
    int getLastTickTime() const { return lastTickTime; }


    QVector<Process*> processes;
    QVector<Process*> waitingList;  // For dynamic input
    QVector<Process*> allProcesses;
    QMap<QString, int> remainingBursts;



private:
    int lastTickTime = 0;  // ✅ stores current tick for dynamic processes


};

#endif // SJF_H
