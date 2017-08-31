#include "user.h"
#include "messaginglayer.h"
#include <QTcpSocket>
#include "settings.h"
#include <QDebug>
#include "chatroom.h"
#include "receivefiledialog.h"

User::User(QTcpSocket *sock)
	: QObject(sock),
      address(sock->peerAddress()),
	  sock(sock),
	  ml(new MessagingLayer(sock))
{
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
    connect(ml, SIGNAL(newUserInfo(QHash<QString,QVariant>)), this, SLOT(updateUserInfo(QHash<QString,QVariant>)));
    connect(ml, SIGNAL(newTextMessage(QUuid,QString)), this, SLOT(publishMessage(QUuid,QString)));
    connect(ml, SIGNAL(newFileRequest(QUuid,QString,qint64)), this, SLOT(handleNewFileRequest(QUuid,QString,qint64)));

    sock->setSocketOption(QTcpSocket::LowDelayOption, 1);
    sock->setSocketOption(QTcpSocket::KeepAliveOption, 1);

    ml->sendUserInfo(Settings::instance()->getUserInfo());
}

void User::publishMessage(const QUuid &chatroom_uuid, const QString &msg)
{
    const QUuid &author = user_info["UUID"].toUuid();
    ChatRoom *room = ChatRoom::instance(chatroom_uuid);
    room->addUser(author);
    room->pushMessage(author, msg);
    room->show();
}

void User::handleNewFileRequest(const QUuid &transfer_uuid, const QString &filename, qint64 file_size)
{
    new ReceiveFileDialog(filename, transfer_uuid, user_info["UUID"].toUuid(), file_size);
}
