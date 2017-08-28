#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QUdpSocket;
class User;
class QTcpServer;
class QTcpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum MessageType
    {
        MSG_TYPE_REQUEST_UPDATE,
        MSG_TYPE_USER_NOTIFICATION
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void notifyTheWorld();
    void refreshUserList();

private slots:
	void readDatagram();
    void updateUserList();
    void handleIncommingConnections();
    void deleteUser(QObject *u);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
	QUdpSocket *discovery_sock;
    QTcpServer *server;
    QList<User*> users;
};

#endif // MAINWINDOW_H
