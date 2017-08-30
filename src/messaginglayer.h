#ifndef MESSAGINGLAYER_H
#define MESSAGINGLAYER_H

#include <QObject>
#include <qglobal.h>
#include <QHash>
#include <QVariant>

class QTcpSocket;
class QUuid;

#define QTCHAT_PORT  ('Q' * 256 + 'C')

class MessagingLayer : public QObject
{
	Q_OBJECT

	enum STATE
	{
		STATE_READ_SIZE,
		STATE_READ_DATA
	};

public:
	enum MessageType
	{
		MSG_TYPE_TEXT,
		MSG_TYPE_USER_INFO,
        MSG_TYPE_FILE_REQUEST,
        MSG_TYPE_FILE_ACK,
        MSG_TYPE_FILE_CANCEL,
        MSG_TYPE_FILE_CHUNK
	};

public:
	explicit MessagingLayer(QTcpSocket *sock);

signals:
    void newTextMessage(const QUuid &chatroom_uuid, const QString &msg);
    void newFileChunk(const QUuid &transfer_uuid, const QByteArray &data);
    void newFileRequest(const QUuid &transfer_uuid, const QString &filename, qint64 file_size);
    void newFileACK(const QUuid &transfer_uuid);
    void newFileCancel(const QUuid &transfer_uuid);
    void newUserInfo(QHash<QString, QVariant> user_info);

public slots:
    void sendTextMessage(const QUuid &chatroom_uuid, const QString &msg);
	void sendUserInfo(const QHash<QString, QVariant> &user_info);
    void sendFileRequest(const QUuid &transfer_uuid, const QString &filename, qint64 file_size);
    void sendFileACK(const QUuid &transfer_uuid);
    void sendFileCancel(const QUuid &transfer_uuid);
    void sendFileChunk(const QUuid &transfer_uuid, const QByteArray &data);

private slots:
	void readData();
	void processMessage(const QByteArray &msg);
	void sendRawMessage(const QByteArray &msg);

private:
	QTcpSocket *sock;
    STATE state = STATE_READ_SIZE;
	qint64 message_size;
	QByteArray msg;
};

#endif // MESSAGINGLAYER_H
