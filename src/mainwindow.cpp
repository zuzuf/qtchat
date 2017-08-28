#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	discovery_sock = new QUdpSocket(this);
	connect(discovery_sock, SIGNAL(readyRead()), this, SLOT(readDatagram()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::readDatagram()
{
	while(discovery_sock->hasPendingDatagrams())
	{
		discovery_sock->readDatagram
	}
}
