#include "user.h"
#include "messaginglayer.h"
#include <QTcpSocket>
#include "settings.h"
#include <QDebug>
#include "chatroom.h"

User::User(QTcpSocket *sock)
	: QObject(sock),
      address(sock->peerAddress()),
	  sock(sock),
	  ml(new MessagingLayer(sock))
{
	connect(ml, SIGNAL(newUserInfo(QHash<QString,QVariant>)), this, SLOT(updateUserInfo(QHash<QString,QVariant>)));
    onConnection();
}

User::User(QObject *parent, const QHostAddress &addr)
    : QObject(parent),
      address(addr),
      sock(new QTcpSocket(parent)),
      ml(new MessagingLayer(sock))
{
    connect(sock, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
    connect(sock, SIGNAL(connected()), this, SLOT(onConnection()));
    connect(ml, SIGNAL(newUserInfo(QHash<QString,QVariant>)), this, SLOT(updateUserInfo(QHash<QString,QVariant>)));
    connect(ml, SIGNAL(newTextMessage(QUuid,QString)), this, SLOT(publishMessage(QUuid,QString)));

    sock->setSocketOption(QTcpSocket::LowDelayOption, 1);
    sock->setSocketOption(QTcpSocket::KeepAliveOption, 1);

    sock->connectToHost(address, QTCHAT_PORT);
}

MessagingLayer *User::getMessagingLayer() const
{
	return ml;
}

void User::updateUserInfo(const QHash<QString, QVariant> &user_info)
{
    this->user_info = user_info;
	emit infoUpdated();
}

const QHash<QString, QVariant> &User::getUserInfo() const
{
    return user_info;
}

const QHostAddress &User::getAddress() const
{
    return address;
}

void User::onConnection()
{
    ml->sendUserInfo(Settings::instance()->getUserInfo());
}

void User::publishMessage(const QUuid &chatroom_uuid, const QString &msg)
{
    const QUuid &author = user_info["UUID"].toUuid();
    ChatRoom *room = ChatRoom::instance(chatroom_uuid);
    room->addUser(author);
    room->pushMessage(author, msg);
}
