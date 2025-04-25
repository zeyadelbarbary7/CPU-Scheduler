#include "secondwindow.h"
#include "ui_secondwindow.h"
#include "fcfs_sjf_window.h"
#include "priority_window.h"
#include "rr_window.h"

int numProcesses;
secondWindow::secondWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::secondWindow)
{
    ui->setupUi(this);
    setWindowTitle("Algorithm Choosing");

    this->setStyleSheet(
        "QDialog {"
        "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e3a8a, stop:1 #3b82f6);"
        "}"
        "QLabel {"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 16px;"
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
        "QComboBox {"
        "   background-color: #ffffff;"
        "   color: #1e3a8a;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   border: 1px solid #3b82f6;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #ffffff;"
        "   color: #1e3a8a;"
        "   selection-background-color: #3b82f6;"
        "   selection-color: #ffffff;"
        "}"
        "QSpinBox {"
        "   background-color: #ffffff;"
        "   color: #1e3a8a;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   border: 1px solid #3b82f6;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "   background-color: #3b82f6;"
        "   border: none;"
        "}"
        "QCheckBox {"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 14px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 16px;"
        "   height: 16px;"
        "   border: 2px solid #3b82f6;"
        "   background-color: #ffffff;"
        "   border-radius: 4px;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: #10b981;"
        "   border: 2px solid #059669;"
        "}"
        );

    // Optional: Set placeholder text or labels for clarity (if not already set in the UI file)
    ui->comboBox->setCurrentText("Select Algorithm");
    ui->spinBox->setMinimum(1); // Ensure at least 1 process


}

secondWindow::~secondWindow()
{
    delete ui;
}

void secondWindow::on_pushButton_clicked()
{
    this->hide();
    parentWidget()->show();
}


bool secondWindow::isLiveSchedulingEnabled() const {
    return ui->checkBox->isChecked();  // Assuming that’s the checkbox name
}



void secondWindow::on_Next_secondWindow_clicked()
{
    numProcesses = ui->spinBox->value();
    QString selectedAlgo = ui->comboBox->currentText();

    if(selectedAlgo=="FCFS"||selectedAlgo=="SJF - Preemptive"||selectedAlgo=="SJF - Non-Preemptive"){
        fcfs_sjf_window = new FCFS_SJF_Window(numProcesses, selectedAlgo, this);
        fcfs_sjf_window->show();
        this->hide();
    }else if(selectedAlgo=="Priority - Preemptive"||selectedAlgo=="Priority - Non-Preemptive"){
        priority_window = new Priority_Window(numProcesses, selectedAlgo, this);
        priority_window->show();
        this->hide();
    }else if(selectedAlgo=="Round Robin"){
        rr_window = new RR_window(numProcesses, selectedAlgo, this);
        rr_window->show();
        this->hide();
    }
}

