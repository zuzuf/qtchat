#ifndef USER_H
#define USER_H

#include <QObject>
#include <qglobal.h>
#include <QString>
#include <QHash>
#include <QVariant>

class QTcpSocket;
class MessagingLayer;

class User : public QObject
{
	Q_OBJECT
public:
	explicit User(QTcpSocket *sock);

	MessagingLayer *getMessagingLayer() const;

signals:
	void infoUpdated();

public slots:

private slots:
	void updateUserInfo(const QHash<QString, QVariant> &user_info);

private:
	QString name;
	QTcpSocket *sock;
	MessagingLayer *ml;

	QHash<QString, QVariant> user_info;
};

#endif // USER_H
