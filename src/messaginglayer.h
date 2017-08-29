#ifndef MESSAGINGLAYER_H
#define MESSAGINGLAYER_H

#include <QObject>
#include <qglobal.h>
#include <QHash>
#include <QVariant>

class QTcpSocket;

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
	};

public:
	explicit MessagingLayer(QTcpSocket *sock);

signals:
	void newTextMessage(QString);
	void newUserInfo(QHash<QString, QVariant> user_info);

public slots:
	void sendTextMessage(QString msg);
	void sendUserInfo(const QHash<QString, QVariant> &user_info);

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
