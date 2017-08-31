#include "usermanager.h"
#include "messaginglayer.h"
#include "settings.h"
#include "user.h"
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QUuid>
#include <QApplication>
#include "chatroom.h"
#include <QTimer>

UserManager *UserManager::s_instance = nullptr;
UserManager *UserManager::instance()
{
    if (!s_instance)
        s_instance = new UserManager;
    return s_instance;
}

UserManager::UserManager()
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(handleIncommingConnections()));
    if (!server->listen(QHostAddress::Any, QTCHAT_PORT))
    {
        QMessageBox::critical(nullptr, tr("QtChat must stop"), tr("QtChat server could not start!"));
        QTimer *timer = new QTimer;
        connect(timer, SIGNAL(timeout()), qApp, SLOT(quit()));
        timer->start(1000);
        return;
    }

    discovery_sock = new QUdpSocket(this);
    discovery_sock->bind(QTCHAT_PORT);
    connect(discovery_sock, SIGNAL(readyRead()), this, SLOT(readDatagram()));

    refreshUserList();
}

void UserManager::readDatagram()
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
        QUuid uuid;
        stream >> msg_type >> uuid;
        // Ignore messages we emitted
        if (uuid == Settings::instance()->getUserInfo("UUID").toUuid())
            continue;

        switch(msg_type)
        {
        case MSG_TYPE_REQUEST_UPDATE:
            notifyTheWorld();
            break;
        case MSG_TYPE_USER_NOTIFICATION:
            {
                QString nickname;
                stream >> nickname;

                if (!users.contains(uuid))
                {
                    User *u = new User(this, src);
                    // Make sure we won't live with zombie pointers
                    connect(u, SIGNAL(destroyed(QObject*)), this, SLOT(deleteUser(QObject*)));
                    connect(u, SIGNAL(infoUpdated()), this, SLOT(notifyUpdate()));

                    users[uuid] = u;
                    ChatRoom::instance()->addUser(uuid);
                    emit usersUpdated();
                }
            }
            break;
        }
    }
}

void UserManager::notifyTheWorld()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);

    stream << qint8(MSG_TYPE_USER_NOTIFICATION)
           << Settings::instance()->getUserInfo("UUID").toUuid()
           << Settings::instance()->getUserInfo("Nickname").toString();

    discovery_sock->writeDatagram(data, QHostAddress::Broadcast, QTCHAT_PORT);
}

void UserManager::refreshUserList()
{
    for(User *u : users)
    {
        ChatRoom::instance()->removeUser(u->getUserInfo()["UUID"].toUuid());
        u->deleteLater();
    }

    users.clear();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint8(MSG_TYPE_REQUEST_UPDATE)
           << Settings::instance()->getUserInfo("UUID").toUuid();

    discovery_sock->writeDatagram(data, QHostAddress::Broadcast, QTCHAT_PORT);
}

void UserManager::handleIncommingConnections()
{
    while(server->hasPendingConnections())
    {
        QTcpSocket *sock = server->nextPendingConnection();
        sock->setSocketOption(QTcpSocket::LowDelayOption, 1);
        sock->setSocketOption(QTcpSocket::KeepAliveOption, 1);

        User *u = new User(sock);
        // Make sure we won't like with zombie pointers
        connect(u, &User::infoUpdated, [=]()
        {
            UserManager::instance()->users[u->getUserInfo()["UUID"].toUuid()] = u;
            ChatRoom::instance()->addUser(u->getUserInfo()["UUID"].toUuid());

            connect(u, SIGNAL(destroyed(QObject*)), UserManager::instance(), SLOT(deleteUser(QObject*)));
            connect(u, SIGNAL(infoUpdated()), UserManager::instance(), SLOT(notifyUpdate()));
            emit usersUpdated();
        });
    }
}

void UserManager::notifyUpdate()
{
    emit usersUpdated();
}

void UserManager::deleteUser(QObject *u)
{
    User *_u = static_cast<User*>(u);
    for(auto it = users.begin() ; it != users.end() ; ++it)
        if (it.value() == _u)
        {
            users.erase(it);
            emit usersUpdated();
            break;
        }
}

const QHash<QUuid, User*> &UserManager::getUserList() const
{
    return users;
}

User *UserManager::getUser(const QUuid &uuid) const
{
    if (users.contains(uuid))
        return users.value(uuid);
    return nullptr;
}
