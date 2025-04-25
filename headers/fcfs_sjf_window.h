#ifndef FCFS_SJF_WINDOW_H
#define FCFS_SJF_WINDOW_H

#include <QDialog>
#include "ganttchartwindow.h"
#include "process.h"
#include <QVector>
#include <qlineedit.h>


namespace Ui {
class FCFS_SJF_Window;
}

class FCFS_SJF_Window : public QDialog
{
    Q_OBJECT

public:
    explicit FCFS_SJF_Window(QWidget *parent = nullptr);
    explicit FCFS_SJF_Window(int processCount, QWidget *parent = nullptr);
    explicit FCFS_SJF_Window(int processCount, const QString &algorithm, QWidget *parent = nullptr);
    QVector<Process*> processes;

    ~FCFS_SJF_Window();

private slots:

    void on_backToSecondWindow_clicked();

    void on_runButton_clicked();

private:
    Ui::FCFS_SJF_Window *ui;
    QWidget *previousWindow;
    QString selectedAlgorithm;
    ganttChartWindow *ganttchartwindow;

    QVector<QLineEdit*> burstInputs;
    QVector<QLineEdit*> arrivalInputs;
    int tickTime;  // Tracks the current time in ticks (seconds or units)

};

#endif // FCFS_SJF_WINDOW_H
