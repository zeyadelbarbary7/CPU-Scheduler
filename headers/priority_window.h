#ifndef PRIORITY_WINDOW_H
#define PRIORITY_WINDOW_H

#include <QDialog>
#include <QLineEdit>
#include "process.h"
#include "ganttchartwindow.h"
#include <QVector>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>


namespace Ui {
class Priority_Window;
}

class Priority_Window : public QDialog
{
    Q_OBJECT

public:
    explicit Priority_Window(QWidget *parent = nullptr);
    explicit Priority_Window(int processCount, const QString& algorithm, QWidget *parent = nullptr);
    ~Priority_Window();
    // QVector<Process*> processes;
    QVector<Process*> getProcesses() const { return processes; } // new

private slots:
    void on_backToSecondWindow_clicked();

    void on_runButton_clicked();


private:
    Ui::Priority_Window *ui;
    QList<QLineEdit*> burstInputs;
    QList<QLineEdit*> arrivalInputs;
    QList<QLineEdit*> priorityInputs;
    QString selectedAlgorithm;
    QWidget *previousWindow;
    ganttChartWindow *ganttchartwindow;
    QVector<Process*> processes;
};

#endif // PRIORITY_WINDOW_H
