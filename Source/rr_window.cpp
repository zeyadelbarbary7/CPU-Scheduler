#include "rr_window.h"
#include "ganttchartwindow.h"
#include "roundrobin.h"
#include "secondwindow.h"
#include "ui_rr_window.h"
#include <qvalidator.h>
#include <QThread>

RR_window::RR_window(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RR_window)
{
    ui->setupUi(this);
}

RR_window::RR_window(int processCount, const QString &algorithm, QWidget *parent)
    : QDialog(parent),previousWindow(parent), selectedAlgorithm(algorithm), ui(new Ui::RR_window)

{
    ui->setupUi(this);
    QVBoxLayout *rowsLayout = new QVBoxLayout;
    ui->processContainerWidget->setLayout(rowsLayout);  // container is a QWidget


    for (int i = 0; i < processCount; ++i) {
        QHBoxLayout *row = new QHBoxLayout;

        QLabel *label = new QLabel(QString("P%1").arg(i + 1));

        QLineEdit *arrivalInput = new QLineEdit;
        QLineEdit *burstInput = new QLineEdit;

        burstInput->setValidator(new QIntValidator(0, 999, this));

        arrivalInput->setValidator(new QIntValidator(0, 999, this));

        // Store the inputs
        burstInputs.append(burstInput);
        arrivalInputs.append(arrivalInput);

        row->addWidget(label);
        row->addWidget(arrivalInput);
        row->addWidget(burstInput);


        rowsLayout->addLayout(row);
    }
}

RR_window::~RR_window()
{
    delete ui;
}

void RR_window::on_backToSecondWindow_clicked()
{
    this->hide();
    if(previousWindow)
        previousWindow->show();
}




void RR_window::on_runButton_clicked()
{
    processes.clear();

    int quantumTime = ui->quantumTime->text().toInt();

    for (int i = 0; i < burstInputs.size(); ++i) {
        QString name = "P" + QString::number(i + 1);
        int burst = burstInputs[i]->text().toInt();
        int arrival = arrivalInputs[i]->text().toInt();

        Process* p = new Process(name, arrival, burst);
        processes.append(p);
    }

    QVector<QPair<QString, int>> ganttData;

    for (Process* p : processes) {
        qDebug() << QString::fromStdString(p->getName())
        << "arrival:" << p->getArrivalTime()
        << "burst:" << p->getBurstTime();
    }

    // Get "Live Scheduling" status from SecondWindow
    bool isLive = false;
    secondWindow* sw = qobject_cast<secondWindow*>(previousWindow);
    if (sw) {
        isLive = sw->isLiveSchedulingEnabled(); // This method must be implemented in SecondWindow
    }

    if (!isLive && selectedAlgorithm == "Round Robin") {
        roundRobin rr;
        RRResult result = rr.runRRWithStats(processes, quantumTime);
        ganttchartwindow = new ganttChartWindow(result.gantt, result.averageWT, result.averageTAT, false, this);
        ganttchartwindow->setProcessList(processes); // create this setter method
    }else{
        ganttchartwindow = new ganttChartWindow(processes,selectedAlgorithm ,isLive ,quantumTime, this);

    }

    for (const auto& pair : ganttData) {
        qDebug() << "Process: " << pair.first << " Duration: " << pair.second;
    }

    // Create and show the Gantt chart window
    ganttchartwindow->show();
    this->hide();
    return;
}




