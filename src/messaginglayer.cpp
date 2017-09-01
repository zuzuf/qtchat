#include "messaginglayer.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QUuid>

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
            if (sock->bytesAvailable() < qint64(sizeof(qint64)))
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
                processMessage(qUncompress(msg));
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
            QUuid chatroom_uuid;
            stream >> chatroom_uuid >> text;
            emit newTextMessage(chatroom_uuid, text);
		}
		break;

	case MSG_TYPE_USER_INFO:
		{
			QHash<QString, QVariant> user_info;
			stream >> user_info;
			emit newUserInfo(user_info);
		}
		break;
    case MSG_TYPE_FILE_CHUNK:
        {
            QUuid transfer_uuid;
            QByteArray data;
            stream >> transfer_uuid >> data;
            emit newFileChunk(transfer_uuid, data);
        }
        break;
    case MSG_TYPE_FILE_REQUEST:
        {
            QUuid transfer_uuid;
            QString filename;
            qint64 file_size;
            stream >> transfer_uuid >> filename >> file_size;
            emit newFileRequest(transfer_uuid, filename, file_size);
        }
        break;
    case MSG_TYPE_FILE_ACK:
        {
            QUuid transfer_uuid;
            stream >> transfer_uuid;
            emit newFileACK(transfer_uuid);
        }
        break;
    case MSG_TYPE_FILE_CANCEL:
        {
            QUuid transfer_uuid;
            stream >> transfer_uuid;
            emit newFileCancel(transfer_uuid);
        }
        break;
    case MSG_TYPE_USER_LEAVE:
        emit userLeft();
        break;
    }
}

void MessagingLayer::sendTextMessage(const QUuid &chatroom_uuid, const QString &msg)
{
	QByteArray buffer;
	QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint32(MSG_TYPE_TEXT) << chatroom_uuid << msg;

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
    const QByteArray &data = qCompress(msg);
    const qint64 msg_size = data.size();
	sock->write((const char*)&msg_size, sizeof(qint64));
    sock->write(data);
}

void MessagingLayer::sendFileChunk(const QUuid &transfer_uuid, const QByteArray &data)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint32(MSG_TYPE_FILE_CHUNK) << transfer_uuid << data;

    sendRawMessage(buffer);
}

void MessagingLayer::sendFileRequest(const QUuid &transfer_uuid, const QString &filename, qint64 file_size)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint32(MSG_TYPE_FILE_REQUEST) << transfer_uuid << filename << file_size;

    sendRawMessage(buffer);
}

void MessagingLayer::sendFileACK(const QUuid &transfer_uuid)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint32(MSG_TYPE_FILE_ACK) << transfer_uuid;

    sendRawMessage(buffer);
}

void MessagingLayer::sendFileCancel(const QUuid &transfer_uuid)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint32(MSG_TYPE_FILE_CANCEL) << transfer_uuid;

    sendRawMessage(buffer);
}

void MessagingLayer::sendUserLeft()
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_2);
    stream << qint32(MSG_TYPE_USER_LEAVE);

    sendRawMessage(buffer);
}
