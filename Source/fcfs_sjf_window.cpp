#include "fcfs_sjf_window.h"
#include "secondwindow.h"
#include "ui_fcfs_sjf_window.h"
#include "process.h"
#include "ganttchartwindow.h"
#include "fcfs.h"
#include "sjf.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <qvalidator.h>
#include <QString>
#include <QThread>


FCFS_SJF_Window::FCFS_SJF_Window(QWidget *parent)
    : QDialog(parent)
{
    // optional: ui->setupUi(this);
}


FCFS_SJF_Window::FCFS_SJF_Window(int processCount, const QString &algorithm, QWidget *parent)
    : QDialog(parent), previousWindow(parent), selectedAlgorithm(algorithm), ui(new Ui::FCFS_SJF_Window)

{
    ui->setupUi(this);
    QVBoxLayout *rowsLayout = new QVBoxLayout;
    ui->processContainerWidget->setLayout(rowsLayout);  // container is a QWidget
    setWindowTitle(selectedAlgorithm + " Scheduling Input"); // Optional: Update window title



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




FCFS_SJF_Window::~FCFS_SJF_Window()
{
    delete ui;
}




void FCFS_SJF_Window::on_backToSecondWindow_clicked()
{
    this->hide();
    if(previousWindow)
        previousWindow->show();
}


void FCFS_SJF_Window::on_runButton_clicked()
{
    processes.clear();

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

    bool isLive = false;
    secondWindow* sw = qobject_cast<secondWindow*>(previousWindow);
    if (sw) {
        isLive = sw->isLiveSchedulingEnabled();
    }

    if (!isLive) {
        if (selectedAlgorithm == "FCFS") {
            FCFS fcfsObj;
            FCFSResult result = fcfsObj.FCFSRunWithStats(processes);
            ganttData = result.gantt;
            ganttchartwindow = new ganttChartWindow(result.gantt, result.averageWT, result.averageTAT, false, this);

        } else if (selectedAlgorithm == "SJF - Non-Preemptive") {
            SJF sjf;
            // call the stats‐computing version
                SJFResult result = sjf.runNonPreemptiveWithStats(processes);
            // pass gantt and averages into the 4-arg constructor
                ganttchartwindow = new ganttChartWindow(
                        result.gantt,
                        result.averageWT,
                        result.averageTAT,
                        false,
                        this);
        } else if (selectedAlgorithm == "SJF - Preemptive") {
            SJF sjf;
            SJFResult result = sjf.runPreemptiveWithStats(processes);
            ganttchartwindow = new ganttChartWindow(result.gantt,result.averageWT,result.averageTAT,false,this);

        }

    } else {
        // Live mode: pass everything to general constructor
        ganttchartwindow = new ganttChartWindow(
            processes,
            selectedAlgorithm,
            true,
            this);
    }

    // Create and show the Gantt chart window
    ganttchartwindow->show();
    this->hide();
}
