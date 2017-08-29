#ifndef USER_H
#define USER_H

#include <QObject>
#include <qglobal.h>
#include <QString>
#include <QHash>
#include <QVariant>
#include <QHostAddress>

class QTcpSocket;
class MessagingLayer;

class User : public QObject
{
	Q_OBJECT
public:
    explicit User(QTcpSocket *sock);
    explicit User(QObject *parent, const QHostAddress &addr);

	MessagingLayer *getMessagingLayer() const;

    const QHash<QString, QVariant> &getUserInfo() const;

    const QHostAddress &getAddress() const;

signals:
	void infoUpdated();

public slots:

private slots:
	void updateUserInfo(const QHash<QString, QVariant> &user_info);
    void onConnection();

private:
    const QHostAddress address;
    QTcpSocket *sock;
	MessagingLayer *ml;

	QHash<QString, QVariant> user_info;
};

#endif // USER_H
