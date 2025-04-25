#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QDialog>
#include "fcfs_sjf_window.h"
#include "priority_window.h"
#include "rr_window.h"

namespace Ui {
class secondWindow;
}

class secondWindow : public QDialog
{
    Q_OBJECT

public:
    explicit secondWindow(QWidget *parent = nullptr);
    ~secondWindow();

private slots:
    void on_pushButton_clicked();

    void on_Next_secondWindow_clicked();

public:
    bool isLiveSchedulingEnabled() const;


private:
    Ui::secondWindow *ui;
    FCFS_SJF_Window *fcfs_sjf_window;
    Priority_Window *priority_window;
    RR_window *rr_window;
};

#endif // SECONDWINDOW_H
