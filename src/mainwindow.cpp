#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dronesim.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_dronesim = new DroneSim(this);
    m_dronesim->start();

    qDebug("MainWindow::Done");
}

MainWindow::~MainWindow()
{
    delete ui;
}
