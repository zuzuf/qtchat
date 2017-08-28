#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QUdpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void readDatagram();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
	QUdpSocket *discovery_sock;
};

#endif // MAINWINDOW_H
