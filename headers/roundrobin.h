#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include <QString>
#include <qmutex.h>
#include <QVector>
#include <QMap>
#include <QString>
#include <queue>
#include "process.h"

struct RRResult {
    QVector<QPair<QString, int>> gantt;
    double averageWT;
    double averageTAT;
};


class roundRobin
{
public:
    QVector<QPair<QString, int>> runRR(const QVector<Process*>& inputProcesses, int quantum);
    void runRRLive(bool& stopFlag,
                   QVector<QPair<QString,int>>& ganttData,
                   bool live,
                   int quantum);
    void runRRLiveUpdated(bool& stopFlag,
              QVector<QPair<QString,int>>& ganttData,
              bool live,
              int quantum,
              double& avgWT,
                double& avgTAT);
    RRResult runRRWithStats(const QVector<Process*>& inputProcesses, int quantum);
    QVector<Process*> waitingList;
    QMap<QString, int> remainingBursts;
    QVector<Process*> allProcesses;



private:
    void moveToReady(QVector<Process>& temp, std::queue<Process>& q, int currentTime);
};





#endif // ROUNDROBIN_H
