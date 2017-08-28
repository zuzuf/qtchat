#include "messaginglayer.h"
#include <QTcpSocket>
#include <QDataStream>

MessagingLayer::MessagingLayer(QTcpSocket *sock)
	: QObject(sock),
	  sock(sock)
{
	connect(sock, SIGNAL(readyRead()), this, SLOT(readData()));
}

void MessagingLayer::readData()
{
	while(sock->bytesAvailable() > 0)
		switch(state)
		{
		case STATE_READ_SIZE:
			if (sock->bytesAvailable() < sizeof(qint64))
				return;
			sock->read((char*)&message_size, sizeof(message_size));
			msg.clear();
			msg.reserve(message_size);
			state = STATE_READ_DATA;
			break;

		case STATE_READ_DATA:
			const qint64 expected_data_size = message_size - msg.size();
			const qint64 to_be_read = std::min(expected_data_size, sock->bytesAvailable());
			msg += sock->read(to_be_read);
			if (msg.size() ==  message_size)
			{
				state = STATE_READ_SIZE;
				processMessage(msg);
			}
			break;
		}
}

void MessagingLayer::processMessage(const QByteArray &msg)
{
	QDataStream stream(msg);
    stream.setVersion(QDataStream::Qt_5_2);

	qint32 msg_type;
	stream >> msg_type;
	switch(msg_type)
	{
	case MSG_TYPE_TEXT:
		{
			QString text;
			stream >> text;
			emit newTextMessage(text);
		}
		break;

	case MSG_TYPE_USER_INFO:
		{
			QHash<QString, QVariant> user_info;
			stream >> user_info;
			emit newUserInfo(user_info);
		}
		break;
	}
}

void MessagingLayer::sendTextMessage(QString msg)
{
	QByteArray buffer;
	QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
	stream << qint32(MSG_TYPE_TEXT) << msg;

	sendRawMessage(buffer);
}

void MessagingLayer::sendUserInfo(const QHash<QString, QVariant> &user_info)
{
	QByteArray buffer;
	QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
	stream << qint32(MSG_TYPE_USER_INFO) << user_info;

	sendRawMessage(buffer);
}

void MessagingLayer::sendRawMessage(const QByteArray &msg)
{
	const qint64 msg_size = msg.size();
	sock->write((const char*)&msg_size, sizeof(qint64));
	sock->write(msg);
}
