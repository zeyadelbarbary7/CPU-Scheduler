#include "priority_window.h"
#include "ui_priority_window.h"
#include <qvalidator.h>
#include "ganttchartwindow.h"
#include "secondwindow.h"
#include "priority.h"

Priority_Window::Priority_Window(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Priority_Window)
{
    ui->setupUi(this);
}

Priority_Window::Priority_Window(int processCount, const QString& algorithm, QWidget *parent)
    : QDialog(parent),
    previousWindow(parent),
    ui(new Ui::Priority_Window),
    selectedAlgorithm(algorithm)  // Store the algorithm name
{
    ui->setupUi(this);
    setWindowTitle(selectedAlgorithm + " Scheduling Input"); // Optional: Update window title

    QVBoxLayout *mainLayout = new QVBoxLayout(ui->processContainerWidget);

    for (int i = 0; i < processCount; ++i) {
        QHBoxLayout *rowLayout = new QHBoxLayout;

        // Process label (e.g., P1, P2)
        QLabel *processLabel = new QLabel(QString("P%1").arg(i + 1));

        // Input fields
        QLineEdit *arrivalInput = new QLineEdit;
        QLineEdit *burstInput = new QLineEdit;
        QLineEdit *priorityInput = new QLineEdit;

        // Set validators
        arrivalInput->setValidator(new QIntValidator(0, 999, this));
        burstInput->setValidator(new QIntValidator(0, 999, this));
        priorityInput->setValidator(new QIntValidator(-999, 999, this)); // Allow negative priorities

        // Store references for later access
        arrivalInputs.append(arrivalInput);
        burstInputs.append(burstInput);
        priorityInputs.append(priorityInput);

        // Add widgets to the row
        rowLayout->addWidget(processLabel);
        // rowLayout->addWidget(new QLabel("Arrival:"));
        rowLayout->addWidget(arrivalInput);
        // rowLayout->addWidget(new QLabel("Burst:"));
        rowLayout->addWidget(burstInput);
        // rowLayout->addWidget(new QLabel("Priority:"));
        rowLayout->addWidget(priorityInput);

        mainLayout->addLayout(rowLayout);
    }
}

Priority_Window::~Priority_Window()
{
    delete ui;
}

void Priority_Window::on_backToSecondWindow_clicked()
{
    this->hide();
    if(previousWindow)
        previousWindow->show();
}


void Priority_Window::on_runButton_clicked()
{
    processes.clear();

    for (int i = 0; i < burstInputs.size(); ++i) {
        QString name = "P" + QString::number(i + 1);
        int burst = burstInputs[i]->text().toInt();
        int arrival = arrivalInputs[i]->text().toInt();
        int priority = priorityInputs[i]->text().toInt();

        Process* p = new Process(name, arrival, burst, priority);
        processes.append(p);
    }

    // figure out Live vs Static
    bool isLive = false;
    secondWindow* sw = qobject_cast<secondWindow*>(previousWindow);
    if (sw) isLive = sw->isLiveSchedulingEnabled();

    if (isLive) {
        // dynamic: hand raw Process* list into the Gantt window
        ganttchartwindow = new ganttChartWindow(processes, selectedAlgorithm, /*live=*/isLive, this);
    }
    else {
        // static: run the scheduler here, get back a QPair timeline
        QVector<QPair<QString,int>> ganttData;
        if (selectedAlgorithm == "Priority - Non-Preemptive") {
            Priority p;
            bool dummyStop = false;
            p.waitingList = processes;
            PriorityResult result = p.runNonPreemptiveWithStats(processes);
            ganttchartwindow = new ganttChartWindow(
                result.gantt,
                result.averageWT,
                result.averageTAT,
                false,
                this);
        }
        else if (selectedAlgorithm == "Priority - Preemptive") {
            Priority p;
            bool dummyStop = false;
            p.waitingList = processes;
            PriorityResult result = p.runPreemptiveWithStats(processes);
            ganttchartwindow = new ganttChartWindow(
                result.gantt,
                result.averageWT,
                result.averageTAT,
                false,
                this);
        }

        }
    ganttchartwindow->show();
    this->hide();
        // hand the finished timeline into the Gantt window
}



