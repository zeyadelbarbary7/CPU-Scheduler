#ifndef RR_WINDOW_H
#define RR_WINDOW_H

#include "ganttchartwindow.h"
#include "process.h"
#include <QDialog>
#include <qlineedit.h>

namespace Ui {
class RR_window;
}

class RR_window : public QDialog
{
    Q_OBJECT

public:
    explicit RR_window(QWidget *parent = nullptr);
    explicit RR_window(int processCount, const QString& algorithm, QWidget *parent = nullptr);
    ~RR_window();
    QVector<Process*> processes;

private slots:
    void on_backToSecondWindow_clicked();

    void on_runButton_clicked();


private:
    Ui::RR_window *ui;
    QList<QLineEdit*> burstInputs;
    QList<QLineEdit*> arrivalInputs;
    QString selectedAlgorithm;
    QWidget *previousWindow;
    ganttChartWindow *ganttchartwindow;
    void onSchedulingFinished(const QVector<QPair<QString, int>>& ganttData, bool isLive);


};

#endif // RR_WINDOW_H
