#include "user.h"
#include "messaginglayer.h"
#include <QTcpSocket>

User::User(QTcpSocket *sock)
	: QObject(sock),
	  sock(sock),
	  ml(new MessagingLayer(sock))
{
	connect(ml, SIGNAL(newUserInfo(QHash<QString,QVariant>)), this, SLOT(updateUserInfo(QHash<QString,QVariant>)));
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
