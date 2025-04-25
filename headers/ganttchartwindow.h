#ifndef GANTTCHARTWINDOW_H
#define GANTTCHARTWINDOW_H

#include <QVector>
#include <QDialog>
#include <QPair>
#include <QString>
#include <QWidget>
#include <qlabel.h>
#include <QTableWidget>
#include "process.h"
#include "roundrobin.h"
#include "priority.h"
#include "sjf.h"
#include "fcfs.h"

namespace Ui {
class ganttChartWindow;
}

class ganttChartWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ganttChartWindow(QWidget *parent = nullptr);
    // explicit ganttChartWindow(const QVector<Process*>& processes, QWidget *parent = nullptr);
    explicit ganttChartWindow(const QVector<QPair<QString, int>>& ganttData, QWidget *parent = nullptr);
    explicit ganttChartWindow(const QVector<QPair<QString, int>>& ganttData, bool live = false, QWidget *parent = nullptr);
    explicit ganttChartWindow(const QVector<Process*>& processes, const QString &algorithm, bool live, QWidget *parent = nullptr);
    explicit ganttChartWindow(const QVector<Process*>& processes, const QString& algorithm, bool live, int rrQuantum, QWidget *parent = nullptr);
    explicit ganttChartWindow(const QVector<QPair<QString, int>>& ganttData, double avgWT, double avgTAT, bool live, QWidget *parent = nullptr);
    // explicit ganttChartWindow(const QVector<QPair<QString, int>>& ganttData,
    //                  double avgWT,
    //                  double avgTAT,
    //                  bool live,
    //                  QWidget* parent = nullptr,
    //                  int quantum = -1); // default for non-RR

    void setLastTickTime(int time) {
        lastTickTime = time;
    }

    int getLastTickTime() const {
        return lastTickTime;
    }

    // void setProcessList(const QVector<Process*>& processes) { allProcesses = processes; }


    void setProcessList(const QVector<Process*>& processes);


    ~ganttChartWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void showNextBlock();
    void updateGantt();


private slots:
    void on_addProcess_clicked();
    void addProcessToBackend(Process* newProcess);
    // void computeAverages();


private:
    Ui::ganttChartWindow *ui;
    QVector<QPair<QString, int>> ganttData;
    QVector<QPair<QString, int>> fullGanttData;
    QVector<QPair<QString, int>> visibleData;
    QTimer *timer;
    int currentIndex = 0;
    int lastTickTime = 0;  // Holds the last tick time value
    int dynamicProcessCounter = 1;
    QVector<Process*> waitingList;
    QVector<Process*> readyQueue;
    bool isLive;
    QVector<Process*> allProcesses;
    Priority* priorityScheduler = nullptr;
    bool stopFlag = false;
    QString selectedAlgorithm;
    int rrQuantum = 1;
    roundRobin* rrScheduler = nullptr;  // shared Round Robin scheduler
    SJF* sjfScheduler = nullptr;
    FCFS* fcfsScheduler = nullptr;

    QTableWidget* remainingTable;
    QLabel*       avgWtLabel;
    QLabel*       avgTatLabel;
    void recalcStats();
    bool hasPrecomputedStats = false;




};

#endif // GANTTCHARTWINDOW_H
