#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "secondwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->setStyleSheet(
        "QMainWindow {"
        "   background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #1e3a8a, stop:1 #3b82f6);"
        "}"
        "QLabel {"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 28px;"
        "   font-weight: bold;"
        "   text-align: center;"
        "   letter-spacing: 1px;"
        "   text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);"
        "}"
        "QPushButton {"
        "   background-color: #10b981;"
        "   color: #ffffff;"
        "   font-family: 'Segoe UI', sans-serif;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border-radius: 10px;"
        "   padding: 10px 20px;"
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
        );

    // Assuming the "Welcome to Quantum Scheduler" text is a QLabel
    ui->label->setAlignment(Qt::AlignCenter);

    // Assuming the "Start" button is a QPushButton
    ui->pushButton->setText("Start");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    secondWindow = new class secondWindow(this);
    secondWindow->show();
    this->hide();
}

