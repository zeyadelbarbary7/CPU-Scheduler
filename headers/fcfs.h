#ifndef FCFS_H
#define FCFS_H

#include <QVector>
#include <QPair>
#include <QString>
#include <QMap>
#include "process.h"

struct FCFSResult {
    QVector<QPair<QString, int>> gantt;
    double averageWT;
    double averageTAT;
};

class FCFS
{
public:
    QVector<QPair<QString, int>> FCFSRun(QVector<Process*> processes);
    // void runFCFSLive(bool& stopFlag,QVector<QPair<QString,int>>& ganttData,bool live);
    QVector<Process*> waitingList;
    FCFSResult FCFSRunWithStats(QVector<Process*> processes);
    void runFCFSLive(bool& stopFlag, QVector<QPair<QString,int>>& gantt, bool live, double& avgWT, double& avgTAT);
    void runFCFSLive(bool& stopFlag, QVector<QPair<QString,int>>& ganttData, bool live);
    QMap<QString, int> remainingBurst;

    QVector<Process*> allProcesses;

};

#endif // FCFS_H
