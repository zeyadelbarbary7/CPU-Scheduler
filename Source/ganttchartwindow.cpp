#include "ganttchartwindow.h"
#include "ui_ganttchartwindow.h"
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QInputDialog>
#include <QMutexLocker>
#include <qthread.h>
#include "roundrobin.h"
#include "priority_window.h"
#include "process.h"
#include "priority.h"

ganttChartWindow::ganttChartWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::ganttChartWindow)
{
    ui->setupUi(this);
    setWindowTitle("Gantt Chart");
    // resize(800, 200);  // Resize as per your requirement

    // Apply modern stylesheet
    this->setStyleSheet(
        "QDialog {"
        "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e3a8a, stop:1 #3b82f6);"
        "}"
        "QLabel {"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   letter-spacing: 0.5px;"
        "   text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);"
        "}"
        "QPushButton {"
        "   background-color: #10b981;"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   border: none;"
        "   box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);"
        "}"
        "QPushButton:hover {"
        "   background-color: #059669;"
        "   box-shadow: 0 6px 8px rgba(0, 0, 0, 0.3);"
        "   transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "   background-color: #047857;"
        "   box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);"
        "   transform: translateY(1px);"
        "}"
        "QLineEdit {"
        "   background-color: #ffffff;"
        "   color: #1e3a8a;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   border: 1px solid #3b82f6;"
        "}"
        );
}


ganttChartWindow::ganttChartWindow(const QVector<QPair<QString, int>>& ganttData, double avgWT, double avgTAT, bool live, QWidget *parent)
    : QDialog(parent), ui(new Ui::ganttChartWindow), fullGanttData(ganttData)
{
    ui->setupUi(this);
    setWindowTitle("Gantt Chart");

    hasPrecomputedStats = true;

    avgWtLabel  = new QLabel(QString("Average Waiting Time : %1").arg(avgWT), this);
    avgTatLabel = new QLabel(QString("Average Turnaround Time : %1").arg(avgTAT), this);
    QVBoxLayout* statsLayout = new QVBoxLayout(ui->statsLayout); // assuming `statsPlaceholder` is a QWidget in the UI
    statsLayout->addWidget(avgWtLabel);
    statsLayout->addWidget(avgTatLabel);


    visibleData = fullGanttData;
    resize(800, 200);
}



ganttChartWindow::ganttChartWindow(const QVector<QPair<QString, int>>& ganttData, bool live, QWidget *parent)
    : QDialog(parent), ui(new Ui::ganttChartWindow), fullGanttData(ganttData)
{
    ui->setupUi(this);
    setWindowTitle("Gantt Chart");

    // Apply modern stylesheet
    this->setStyleSheet(
        "QDialog {"
        "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e3a8a, stop:1 #3b82f6);"
        "}"
        "QLabel {"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   letter-spacing: 0.5px;"
        "   text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);"
        "}"
        "QPushButton {"
        "   background-color: #10b981;"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   border: none;"
        "   box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);"
        "}"
        "QPushButton:hover {"
        "   background-color: #059669;"
        "   box-shadow: 0 6px 8px rgba(0, 0, 0, 0.3);"
        "   transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "   background-color: #047857;"
        "   box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);"
        "   transform: translateY(1px);"
        "}"
        "QLineEdit {"
        "   background-color: #ffffff;"
        "   color: #1e3a8a;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   border: 1px solid #3b82f6;"
        "}"
        );

    ui->addProcess->hide();
    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->lineEdit->hide();
    ui->lineEdit_2->hide();
    ui->lineEdit_3->hide();

    if (live) {
        isLive = true;
        stopFlag = false;
        priorityScheduler = new Priority();

        QThread* schedulerThread = QThread::create([=]() {
            priorityScheduler->waitingList = allProcesses; // You may adjust
            priorityScheduler->runPreemptive(stopFlag, fullGanttData, true);
        });

        schedulerThread->start();

        // Animate the Gantt chart
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ganttChartWindow::updateGantt);
        timer->start(1000);
    }else {
        visibleData = fullGanttData;
    }

    resize(800, 200);  // Resize as per your requirement
}


ganttChartWindow::ganttChartWindow(
    const QVector<Process*>& processes,
    const QString& algorithm,
    bool live,
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ganttChartWindow)
    , selectedAlgorithm(algorithm)
    , isLive(live)
    , stopFlag(false)
{
    ui->setupUi(this);
    setWindowTitle("Gantt Chart — " + selectedAlgorithm);

    this->setStyleSheet(
        "QDialog {"
        "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e3a8a, stop:1 #3b82f6);"
        "}"
        "QLabel {"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   letter-spacing: 0.5px;"
        "   text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);"
        "}"
        "QPushButton {"
        "   background-color: #10b981;"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   border: none;"
        "   box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);"
        "}"
        "QPushButton:hover {"
        "   background-color: #059669;"
        "   box-shadow: 0 6px 8px rgba(0, 0, 0, 0.3);"
        "   transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "   background-color: #047857;"
        "   box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);"
        "   transform: translateY(1px);"
        "}"
        "QLineEdit {"
        "   background-color: #ffffff;"
        "   color: #1e3a8a;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   border: 1px solid #3b82f6;"
        "}"
        );


    allProcesses = processes;

    avgWtLabel  = new QLabel("Avg WT: 0", this);
    avgTatLabel = new QLabel("Avg TAT: 0", this);
    QVBoxLayout* statsLayout = new QVBoxLayout(ui->statsLayout);
    statsLayout->addWidget(avgWtLabel);
    statsLayout->addWidget(avgTatLabel);

    QVBoxLayout * tableLayout = new QVBoxLayout( ui->tableLayout );
    auto heading = new QLabel("Remaining Burst", this);
    tableLayout->addWidget( heading );

    if(selectedAlgorithm == "SJF - Preemptive" || selectedAlgorithm == "SJF - Non-Preemptive" || selectedAlgorithm == "FCFS"){
        ui->lineEdit_3->hide();
        ui->label_3->hide();
    }


    // Priority Scheduling
    if (selectedAlgorithm.contains("Priority")) {
        priorityScheduler = new Priority();
        priorityScheduler->waitingList = processes;
        priorityScheduler->allProcesses = processes;

        // ─── Remaining-Burst table for Priority ────────────────────────

        remainingTable = new QTableWidget(this);
        remainingTable->setColumnCount(2);
        remainingTable->setHorizontalHeaderLabels({"Process","Remaining"});
        remainingTable->setRowCount(allProcesses.size());
        tableLayout->addWidget(remainingTable);
        // ───────────────────────────────────────────────────────────────────

        if (!isLive) {
            if (selectedAlgorithm == "Priority - Non-Preemptive") {
                priorityScheduler->runNonPreemptive(stopFlag, fullGanttData, false);
            } else {
                priorityScheduler->runPreemptive(stopFlag, fullGanttData, false);
            }
            visibleData = fullGanttData;
            update();
            return;
        }

        QThread* schedulerThread = QThread::create([this]() {
            if (selectedAlgorithm == "Priority - Non-Preemptive") {
                double avgWT = 0, avgTAT = 0;
                priorityScheduler->runNonPreemptiveLiveUpdated(stopFlag, fullGanttData, true,avgWT,avgTAT);
                QMetaObject::invokeMethod(this, [=]() {
                    avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
                    avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
                }, Qt::QueuedConnection);
            } else {
                double avgWT = 0, avgTAT = 0;
                priorityScheduler->runPreemptiveLiveUpdated(stopFlag, fullGanttData, true,avgWT,avgTAT);
                QMetaObject::invokeMethod(this, [=]() {
                    avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
                    avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
                }, Qt::QueuedConnection);
            }
        });
        schedulerThread->start();
        connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);
    }

    // SJF Preemptive
    else if (selectedAlgorithm == "SJF - Preemptive"){
        sjfScheduler = new SJF();
        sjfScheduler->waitingList = processes;
        sjfScheduler->allProcesses = processes;

        remainingTable = new QTableWidget(this);
        remainingTable->setColumnCount(2);
        remainingTable->setHorizontalHeaderLabels({"Process","Remaining"});
        remainingTable->setRowCount(allProcesses.size());
        tableLayout->addWidget(remainingTable);

        if (!isLive) {
            sjfScheduler->runPreemptiveLive(stopFlag, fullGanttData, /*live=*/false);
            visibleData = fullGanttData;
            update();
            return;
        }

        QThread* schedulerThread = QThread::create([this]() {
            double avgWT = 0, avgTAT = 0;

            sjfScheduler->runPreemptiveLiveUpdated(stopFlag, fullGanttData, /*live=*/true,avgWT,avgTAT);
            QMetaObject::invokeMethod(this, [=]() {
                avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
                avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
            }, Qt::QueuedConnection);
        });

        schedulerThread->start();
        connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);

    }

    // SJF Non-Preemptive
    else if (selectedAlgorithm == "SJF - Non-Preemptive") {
        sjfScheduler = new SJF();
        sjfScheduler->waitingList = processes;
        sjfScheduler->processes = processes;

        remainingTable = new QTableWidget(this);
        remainingTable->setColumnCount(2);
        remainingTable->setHorizontalHeaderLabels({"Process","Remaining"});
        remainingTable->setRowCount(allProcesses.size());
        tableLayout->addWidget(remainingTable);

        if (!isLive) {
            sjfScheduler->runNonPreemptiveLive(stopFlag, fullGanttData, false);
            visibleData = fullGanttData;
            update();
            return;
        }

        QThread* schedulerThread = QThread::create([this]() {
            double avgWT = 0, avgTAT = 0;
            sjfScheduler->runNonPreemptiveLiveUpdated(stopFlag,fullGanttData,true,avgWT,avgTAT);
            QMetaObject::invokeMethod(this, [=]() {
                avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
                avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
            }, Qt::QueuedConnection);
        });
        schedulerThread->start();
        connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);

    }

    // // Round Robin
    // else if (selectedAlgorithm == "Round Robin") {
    //     rrScheduler = new roundRobin();
    //     rrScheduler->waitingList = processes;
    //     rrScheduler->allProcesses = processes; // 🔑 Needed for stats

    //     remainingTable = new QTableWidget(this);
    //     remainingTable->setColumnCount(2);
    //     remainingTable->setHorizontalHeaderLabels({"Process", "Remaining"});
    //     remainingTable->setRowCount(allProcesses.size());
    //     QVBoxLayout *tableLayout = new QVBoxLayout(ui->tableLayout);
    //     tableLayout->addWidget(remainingTable);

    //     if (!isLive) {
    //         rrScheduler->runRRLive(stopFlag, fullGanttData, false, rrQuantum);
    //         visibleData = fullGanttData;
    //         update();
    //         return;
    //     }

    //     QThread* schedulerThread = QThread::create([this]() {
    //         double avgWT = 0, avgTAT = 0;
    //         rrScheduler->runRRLiveUpdated(stopFlag, fullGanttData, true, rrQuantum, avgWT, avgTAT);

    //         QMetaObject::invokeMethod(this, [=]() {
    //             avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
    //             avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
    //         }, Qt::QueuedConnection);
    //     });

    //     schedulerThread->start();
    //     connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);
    // }



    else if (selectedAlgorithm == "FCFS"){
        fcfsScheduler = new FCFS();
        // give it the initial list
        fcfsScheduler->waitingList = processes;
        fcfsScheduler->allProcesses  = processes;

        if (!isLive) {
            // static bulk
            fullGanttData = fcfsScheduler->FCFSRun(processes);
            visibleData = fullGanttData;
            update();
            return;
        }

        // ─── add these lines ──────────────────────────────────────────
        remainingTable = new QTableWidget(this);
        remainingTable->setColumnCount(2);
        remainingTable->setHorizontalHeaderLabels({"Process","Remaining"});
        remainingTable->setRowCount(allProcesses.size());
        tableLayout->addWidget(remainingTable);  // put it under your stats
        // ─────────────────────────────────────────────────────────────

        QThread* schedulerThread = QThread::create([this]() {
            double avgWT = 0, avgTAT = 0;
            fcfsScheduler->runFCFSLive(stopFlag, fullGanttData, true, avgWT, avgTAT);
            QMetaObject::invokeMethod(this, [=]() {
                avgWtLabel->setText(QString("Average Waiting Time : %1").arg(avgWT));
                avgTatLabel->setText(QString("Average Turnaround Time : %1").arg(avgTAT));
            }, Qt::QueuedConnection);
        });
        schedulerThread->start();
        connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);
    }

    // Setup live animation
    if (isLive) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ganttChartWindow::updateGantt);
        timer->start(1000);
    }
}

void ganttChartWindow::setProcessList(const QVector<Process*>& processes) {
    // store for stats & “addProcess” logic
    allProcesses = processes;

    // if you ever run live RR and want to show remaining bursts:
    if (rrScheduler) {
        rrScheduler->waitingList = allProcesses;
        // initialize remainingBursts map
        for (Process* p : allProcesses) {
            QString name = QString::fromStdString(p->getName());
            rrScheduler->remainingBursts[name] = p->getBurstTime();
        }
    }
}


// ganttChartWindow::ganttChartWindow(
//     const QVector<Process*>& processes,
//     const QString& algorithm,
//     bool live,
//     int quantum,
//     QWidget *parent)
//     : QDialog(parent)
//     , ui(new Ui::ganttChartWindow)
//     , selectedAlgorithm(algorithm)
//     , isLive(live)
//     , stopFlag(false)
//     , rrQuantum(quantum)            // ← store quantum
// {
//     ui->setupUi(this);
//     setWindowTitle("Gantt Chart — " + selectedAlgorithm);

//     rrScheduler = new roundRobin();
//     rrScheduler->waitingList = processes;
//     rrScheduler->allProcesses = processes; // 🔑 Needed for stats

//     QVBoxLayout * tableLayout = new QVBoxLayout( ui->tableLayout );
//     auto heading = new QLabel("Remaining Burst", this);
//     tableLayout->addWidget( heading );


//     if (isLive){
//         ui->label_3->hide();
//         ui->lineEdit_3->hide();
//     }

//     if (!isLive) {
//         resize(800, 200);  // Resize as per your requirement
//         ui->addProcess->hide();
//         ui->label->hide();
//         ui->label_2->hide();
//         ui->label_3->hide();
//         ui->lineEdit->hide();
//         ui->lineEdit_2->hide();
//         ui->lineEdit_3->hide();
//         rrScheduler->runRRLive(stopFlag, fullGanttData, false, rrQuantum);
//         visibleData = fullGanttData;
//         update();
//         return;
//     }

//     remainingTable = new QTableWidget(this);
//     remainingTable->setColumnCount(2);
//     remainingTable->setHorizontalHeaderLabels({"Process","Remaining"});
//     remainingTable->setRowCount(allProcesses.size());
//     tableLayout->addWidget(remainingTable);

//     QThread* schedulerThread = QThread::create([this]() {
//         double avgWT = 0, avgTAT = 0;
//         rrScheduler->runRRLiveUpdated(stopFlag, fullGanttData, true, rrQuantum, avgWT, avgTAT);

//         QMetaObject::invokeMethod(this, [=]() {
//             avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
//             avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
//         }, Qt::QueuedConnection);    });
//     schedulerThread->start();
//     connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);

//     timer = new QTimer(this);
//     connect(timer, &QTimer::timeout, this, &ganttChartWindow::updateGantt);
//     timer->start(1000);
// }

// ganttChartWindow::ganttChartWindow(
//     const QVector<Process*>& processes,
//     const QString& algorithm,
//     bool live,
//     int quantum,
//     QWidget *parent)
//     : QDialog(parent)
//     , ui(new Ui::ganttChartWindow)
//     , selectedAlgorithm(algorithm)
//     , isLive(live)
//     , stopFlag(false)
//     , rrQuantum(quantum)  // Store quantum time
// {
//     ui->setupUi(this);
//     setWindowTitle("Gantt Chart — " + selectedAlgorithm);

//     // Initialize scheduler
//     rrScheduler = new roundRobin();
//     rrScheduler->waitingList = processes;
//     rrScheduler->allProcesses = processes;

//     // Setup stats labels
//     avgWtLabel  = new QLabel("Avg WT: 0", this);
//     avgTatLabel = new QLabel("Avg TAT: 0", this);
//     QVBoxLayout* statsLayout = new QVBoxLayout(ui->statsLayout);
//     statsLayout->addWidget(avgWtLabel);
//     statsLayout->addWidget(avgTatLabel);

//     avgWtLabel->setText("Avg WT: ...");
//     avgTatLabel->setText("Avg TAT: ...");

//     // Setup remaining burst table
//     QVBoxLayout* tableLayout = new QVBoxLayout(ui->tableLayout);
//     QLabel* heading = new QLabel("Remaining Burst", this);
//     tableLayout->addWidget(heading);

//     remainingTable = new QTableWidget(this);
//     remainingTable->setColumnCount(2);
//     remainingTable->setHorizontalHeaderLabels({"Process", "Remaining"});
//     remainingTable->setRowCount(processes.size());
//     tableLayout->addWidget(remainingTable);


//     // Hide input fields not used in live/static view
//     if (!isLive) {
//         resize(800, 200);
//         ui->addProcess->hide();
//         ui->label->hide();
//         ui->label_2->hide();
//         ui->label_3->hide();
//         ui->lineEdit->hide();
//         ui->lineEdit_2->hide();
//         ui->lineEdit_3->hide();

//         // 🔧 Run Round Robin with stats (static mode)
//         double avgWT = 0, avgTAT = 0;
//         rrScheduler->runRRLiveUpdated(stopFlag, fullGanttData, false, rrQuantum, avgWT, avgTAT);
//         visibleData = fullGanttData;

//         // Display stats
//         avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
//         avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));

//         update();  // Refresh GUI
//         return;
//     }

//     // Live mode (threaded scheduling + stats)
//     QThread* schedulerThread = QThread::create([this]() {
//         double avgWT = 0, avgTAT = 0;
//         rrScheduler->runRRLiveUpdated(stopFlag, fullGanttData, true, rrQuantum, avgWT, avgTAT);

//         // Update UI labels safely from GUI thread
//         QMetaObject::invokeMethod(this, [=]() {
//             avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
//             avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
//         }, Qt::QueuedConnection);
//     });

//     schedulerThread->start();
//     connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);

//     // Animate live updates every 1 second
//     timer = new QTimer(this);
//     connect(timer, &QTimer::timeout, this, &ganttChartWindow::updateGantt);
//     timer->start(1000);
// }


ganttChartWindow::ganttChartWindow(
    const QVector<Process*>& processes,
    const QString& algorithm,
    bool live,
    int quantum,
    QWidget *parent)
    : QDialog(parent),
    ui(new Ui::ganttChartWindow),
    selectedAlgorithm(algorithm),
    isLive(live),
    stopFlag(false),
    rrQuantum(quantum)
{
    ui->setupUi(this);
    setWindowTitle("Gantt Chart — " + selectedAlgorithm);

    this->setStyleSheet(
        "QDialog {"
        "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e3a8a, stop:1 #3b82f6);"
        "}"
        "QLabel {"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   letter-spacing: 0.5px;"
        "   text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);"
        "}"
        "QPushButton {"
        "   background-color: #10b981;"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   border: none;"
        "   box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);"
        "}"
        "QPushButton:hover {"
        "   background-color: #059669;"
        "   box-shadow: 0 6px 8px rgba(0, 0, 0, 0.3);"
        "   transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "   background-color: #047857;"
        "   box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);"
        "   transform: translateY(1px);"
        "}"
        "QLineEdit {"
        "   background-color: #ffffff;"
        "   color: #1e3a8a;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   border: 1px solid #3b82f6;"
        "}"
        );

    allProcesses = processes;

    avgWtLabel = new QLabel("Avg WT: 0", this);
    avgTatLabel = new QLabel("Avg TAT: 0", this);
    QVBoxLayout* statsLayout = new QVBoxLayout(ui->statsLayout);
    statsLayout->addWidget(avgWtLabel);
    statsLayout->addWidget(avgTatLabel);

    QVBoxLayout* tableLayout = new QVBoxLayout(ui->tableLayout);
    QLabel* heading = new QLabel("Remaining Burst", this);
    tableLayout->addWidget(heading);

    remainingTable = new QTableWidget(this);
    remainingTable->setColumnCount(2);
    remainingTable->setHorizontalHeaderLabels({"Process", "Remaining"});
    remainingTable->setRowCount(processes.size());
    tableLayout->addWidget(remainingTable);

    if (isLive) {
        hasPrecomputedStats = false;
        ui->label_3->hide();
        ui->lineEdit_3->hide();
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ganttChartWindow::updateGantt);
        timer->start(1000);
    }

    // Round Robin
    if (selectedAlgorithm == "Round Robin") {
        rrScheduler = new roundRobin();
        rrScheduler->waitingList = processes;
        rrScheduler->allProcesses = processes;

        if (!isLive) {
            double avgWT = 0, avgTAT = 0;
            rrScheduler->runRRLiveUpdated(stopFlag, fullGanttData, false, rrQuantum, avgWT, avgTAT);
            visibleData = fullGanttData;
            avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
            avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
            hasPrecomputedStats = true;
            update();
            return;
        }

        QThread* schedulerThread = QThread::create([this]() {
            double avgWT = 0, avgTAT = 0;
            rrScheduler->runRRLiveUpdated(stopFlag, fullGanttData, true, rrQuantum, avgWT, avgTAT);

            QMetaObject::invokeMethod(this, [=]() {
                avgWtLabel->setText(QString("Avg WT: %1").arg(avgWT));
                avgTatLabel->setText(QString("Avg TAT: %1").arg(avgTAT));
            }, Qt::QueuedConnection);
        });
        schedulerThread->start();
        connect(schedulerThread, &QThread::finished, this, &ganttChartWindow::recalcStats);
    }
}


// void ganttChartWindow::updateGantt() {
//     if (currentIndex < fullGanttData.size()) {
//         visibleData.append(fullGanttData[currentIndex]);
//         currentIndex++;
//         update();            // repaint
//     }
//     else if (stopFlag) {
//         timer->stop();       // done
//         // recalcStats();       // ← update averages on screen

//     }

//     if (isLive && remainingTable && fcfsScheduler) {
//         remainingTable->setRowCount(allProcesses.size());
//         // refresh remaining‐burst column
//         for (int row = 0; row < allProcesses.size(); ++row) {
//             QString name = QString::fromStdString(allProcesses[row]->getName());
//             int rem   = fcfsScheduler->remainingBurst.value(name,0);
//             remainingTable->setItem(row,0, new QTableWidgetItem(name));
//             remainingTable->setItem(row,1, new QTableWidgetItem(QString::number(rem)));
//         }
//     }
// }

void ganttChartWindow::updateGantt() {
    if (currentIndex < fullGanttData.size()) {
        visibleData.append(fullGanttData[currentIndex]);
        currentIndex++;
        update();            // repaint
    }
    else if (stopFlag) {
        timer->stop();       // done
        recalcStats();
    }

    // ── live “Remaining Burst” refresh for FCFS or Priority ─────────
    if (isLive && remainingTable) {
        remainingTable->setRowCount(allProcesses.size());
        for (int row = 0; row < allProcesses.size(); ++row) {
            QString name = QString::fromStdString(allProcesses[row]->getName());
            int rem = 0;
            // if Priority scheduling, read from priorityScheduler->remainingBursts
            if (selectedAlgorithm.contains("Priority") && priorityScheduler) {
                rem = priorityScheduler->remainingBursts.value(name, 0);
            }
            // otherwise if FCFS, read from fcfsScheduler->remainingBurst
            else if (selectedAlgorithm == "FCFS" && fcfsScheduler) {
                rem = fcfsScheduler->remainingBurst.value(name, 0);
            }
            else if (selectedAlgorithm.contains("SJF") && sjfScheduler) {
                rem = sjfScheduler->remainingBursts.value(name, 0);
            }
            else if (selectedAlgorithm == "Round Robin" && rrScheduler) {
                rem = rrScheduler->remainingBursts.value(name, 0);
            }

            remainingTable->setItem(row, 0, new QTableWidgetItem(name));
            remainingTable->setItem(row, 1, new QTableWidgetItem(QString::number(rem)));
        }
    }
}


void ganttChartWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int x = 20;
    int y = 50;
    int height = 50;

    painter.setPen(Qt::black);

    // Store the last tick time
    int lastTickTime = 0;

    for (const auto& pair : visibleData) {
        QString processName = pair.first;
        int duration = pair.second;
        int width = duration * 40;

        painter.setBrush(Qt::white);
        painter.drawRect(x, y, width, height);
        painter.drawText(x + width / 2 - 10, y + height / 2 + 5, processName);

        painter.setPen(Qt::white); // For time tick
        painter.drawText(x, y + height + 20, QString::number(x / 40));
        x += width;

        painter.setPen(Qt::black); // Reset for next box
    }

    // Final time tick
    if (!visibleData.isEmpty()) {
        painter.setPen(Qt::white);
        painter.drawText(x, y + height + 20, QString::number(x / 40));
    }

    // Update the last tick time
    lastTickTime = x / 40; // This gives us the last tick based on the current x position

    // Optionally, store this value in a member variable or return it directly
    setLastTickTime(lastTickTime);
}

void ganttChartWindow::recalcStats()
{
    QMap<QString,int> completion;
    QMap<QString,int> burstCount;

    if (hasPrecomputedStats) return;   // ← add this

    int clock = 0;
    // walk the actual chart, summing durations
    for (auto &pr : visibleData) {
        const QString &name = pr.first;
        int dur = pr.second;
        clock += dur;
        // on every appearance, update completion time to end-of-this-block
        completion[name] = clock;
        if (name != "IDLE")
            burstCount[name] += dur;
    }

    double totalWT = 0, totalTAT = 0;
    int n = allProcesses.size();
    for (Process* p : allProcesses) {
        QString name = QString::fromStdString(p->getName());
        int arrival = p->getArrivalTime();
        int burst   = burstCount[name];
        int comp    = completion[name];
        int tat = comp - arrival;
        int wt  = tat - burst;
        totalTAT += tat;
        totalWT  += wt;
    }

    avgWtLabel ->setText(
        QString("Avg WT: %1").arg(n? totalWT/n : 0.0));
    avgTatLabel->setText(
        QString("Avg TAT: %1").arg(n? totalTAT/n : 0.0));
}




void ganttChartWindow::addProcessToBackend(Process* newProcess) {
    // Add the process to the waiting list or ready queue
    // This depends on your backend implementation
    // For example, if using a QVector:

    allProcesses.append(newProcess);


    if (selectedAlgorithm == "Priority - Non-Preemptive" && priorityScheduler) {
        priorityScheduler->waitingList.append(newProcess);
        priorityScheduler->remainingBursts[QString::fromStdString(newProcess->getName())] = newProcess->getBurstTime();
    }
    else if (selectedAlgorithm == "Priority - Preemptive" && priorityScheduler) {
        priorityScheduler->waitingList.append(newProcess);
        priorityScheduler->remainingBursts[QString::fromStdString(newProcess->getName())] = newProcess->getBurstTime();
    }
    if (selectedAlgorithm == "Round Robin" && rrScheduler) {
        // 1) put it into the live‐dispatch queue
        rrScheduler->waitingList.append(newProcess);
        // 2) let the stats routine know about it
        rrScheduler->allProcesses.append(newProcess);
        // 3) initialize its remaining-burst counter
        QString name = QString::fromStdString(newProcess->getName());
        rrScheduler->remainingBursts[name] = newProcess->getBurstTime();
    }
    else if (selectedAlgorithm == "SJF - Non-Preemptive" && sjfScheduler) {
        newProcess->setRemainingTime(newProcess->getBurstTime());
        sjfScheduler->waitingList.append(newProcess);
        sjfScheduler->remainingBursts[QString::fromStdString(newProcess->getName())] = newProcess->getBurstTime();
    }
    else if (selectedAlgorithm == "SJF - Preemptive" && sjfScheduler) {
        newProcess->setRemainingTime(newProcess->getBurstTime());
        sjfScheduler->waitingList.append(newProcess);
        sjfScheduler->remainingBursts[QString::fromStdString(newProcess->getName())] = newProcess->getBurstTime();
    }
    else if (selectedAlgorithm == "FCFS" && fcfsScheduler) {
        fcfsScheduler->waitingList.append(newProcess);
    }

    for (int i=0 ; i<waitingList.size() ; i++){
        qDebug () << "arrival time : " << waitingList[i]->getArrivalTime() << " name : " <<waitingList[i]->getName() << " priority : " <<waitingList[i]->getPriority();
    }

    for (int i = 0; i < allProcesses.size(); ++i) {
        qDebug ()<< "process name : " << allProcesses[i]->getName();
    }

    // If live mode is enabled, you might want to trigger an update
    if (isLive) {
        // immediately make room in the table
        if (remainingTable)
                remainingTable->setRowCount(allProcesses.size());
        updateGantt();
    }


}


void ganttChartWindow::on_addProcess_clicked()
{
    // Get the process name and burst time from the UI
    QString processName = ui->lineEdit->text();  // Assuming processNameLineEdit is a QLineEdit
    int burstTime = ui->lineEdit_2->text().toInt();  // Assuming burstTimeSpinBox is a QSpinBox
    int priority = ui->lineEdit_3->text().toInt();

    if (!processName.isEmpty() && burstTime > 0) {
        int arrivalTime = getLastTickTime() + 1;  // Get the next second as arrival time

        // Create a new process
        Process* newProcess = new Process(QString::fromStdString(processName.toStdString()), arrivalTime, burstTime, priority);

        // Add the new process to the backend
        addProcessToBackend(newProcess);

        // Optionally, update the UI to reflect the added process
        qDebug() << "Added Process: " << processName << " Arrival Time: " << arrivalTime << " Burst Time: " << burstTime << " priority : " << priority;

        // Clear input fields
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
    }

}

ganttChartWindow::~ganttChartWindow()
{
    stopFlag = true;      // signal scheduler to exit its loop  //new
    QThread::msleep(100); // give it a moment
    delete ui;
}
