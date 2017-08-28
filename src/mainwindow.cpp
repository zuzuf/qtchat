#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>
#include <QTcpServer>
#include "settings.h"
#include "user.h"
#include "messaginglayer.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(handleIncommingConnections()));
    if (!server->listen(QHostAddress::Any, QTCHAT_PORT))
    {
        QMessageBox::critical(nullptr, tr("QtChat must stop"), tr("QtChat server could not start!"));
        qApp->quit();
        return;
    }

	discovery_sock = new QUdpSocket(this);
    discovery_sock->bind(QTCHAT_PORT);
	connect(discovery_sock, SIGNAL(readyRead()), this, SLOT(readDatagram()));

    connect(ui->action_Exit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(ui->pbRefresh, SIGNAL(pressed()), this, SLOT(refreshUserList()));

    refreshUserList();
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
    QByteArray data;
    QHostAddress src;
    quint16 port;
    while(discovery_sock->hasPendingDatagrams())
	{
        data.resize(discovery_sock->pendingDatagramSize());

        if (discovery_sock->readDatagram(data.data(), data.size(), &src, &port) == -1)
            continue;

        if (port != QTCHAT_PORT) // oO ?
            continue;

        QDataStream stream(data);
        stream.setVersion(QDataStream::Qt_5_2);
        qint8 msg_type;
        stream >> msg_type;
        switch(msg_type)
        {
        case MSG_TYPE_REQUEST_UPDATE:
            notifyTheWorld();
            break;
        case MSG_TYPE_USER_NOTIFICATION:
            {
                QString nickname;
                stream >> nickname;

                bool b_exist = false;
                for(User *u : users)
                    if (u->getAddress() == src)
                    {
                        b_exist = true;
                        break;
                    }

                if (!b_exist)
                {
                    User *u = new User(this, src);
                    // Make sure we won't like with zombie pointers
                    connect(u, SIGNAL(destroyed(QObject*)), this, SLOT(deleteUser(QObject*)));

                    users << u;
                    updateUserList();
                }
            }
            break;
        }
	}
}

void MainWindow::notifyTheWorld()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);

    stream << qint8(MSG_TYPE_USER_NOTIFICATION)
           << Settings::instance()->getSetting("Nickname").toString();

    discovery_sock->writeDatagram(data, QHostAddress::Broadcast, QTCHAT_PORT);
}

void MainWindow::refreshUserList()
{
    for(User *u : users)
        u->deleteLater();

    users.clear();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint8(MSG_TYPE_REQUEST_UPDATE);

    discovery_sock->writeDatagram(data, QHostAddress::Broadcast, QTCHAT_PORT);
}

void MainWindow::updateUserList()
{
    ui->lwUsers->clear();
    for(User *u : users)
        ui->lwUsers->addItem(u->getUserInfo()["Nickname"].toString());
}

void MainWindow::handleIncommingConnections()
{
    while(server->hasPendingConnections())
    {
        QTcpSocket *sock = server->nextPendingConnection();

        User *u = new User(sock);
        // Make sure we won't like with zombie pointers
        connect(u, SIGNAL(destroyed(QObject*)), this, SLOT(deleteUser(QObject*)));

        users << u;
        updateUserList();
    }
}

void MainWindow::deleteUser(QObject *u)
{
    users.removeAll(static_cast<User*>(u));
}
