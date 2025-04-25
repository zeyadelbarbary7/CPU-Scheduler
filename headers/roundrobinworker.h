#ifndef ROUNDROBINWORKER_H
#define ROUNDROBINWORKER_H

#include <QObject>
#include <QVector>
#include <QQueue>
#include <QPair>
#include <QString>
#include "process.h"
#include "roundrobin.h"

class roundRobinWorker : public QObject {
    Q_OBJECT

public:
    roundRobinWorker(QVector<Process*> processes, QQueue<Process*> waitingList, int quantum, bool live);

public slots:
    void process();  // Slot to start the processing

signals:
    void finished(const QVector<QPair<QString, int>>& ganttData);  // Signal emitted when processing is done

private:
    QVector<Process*> processes;
    QQueue<Process*> waitingList;
    int quantum;
    bool live;
};

#endif // ROUNDROBINWORKER_H_H
