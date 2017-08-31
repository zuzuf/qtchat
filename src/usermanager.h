#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QUuid>
#include <QHash>
#include <QString>
#include <QList>

class QUdpSocket;
class User;
class QTcpServer;
class QTcpSocket;

class UserManager : public QObject
{
    Q_OBJECT

    enum MessageType
    {
        MSG_TYPE_REQUEST_UPDATE,
        MSG_TYPE_USER_NOTIFICATION
    };

private:
    UserManager();

public:
    const QHash<QUuid, User*> &getUserList() const;
    User *getUser(const QUuid &uuid) const;

signals:
    void usersUpdated();

public slots:
    void notifyTheWorld();
    void refreshUserList();

private slots:
    void readDatagram();
    void handleIncommingConnections();
    void deleteUser(QObject *u);
    void notifyUpdate();

private:
    QUdpSocket *discovery_sock;
    QTcpServer *server;
    QHash<QUuid, User*> users;

private:
    static UserManager *s_instance;

public:
    static UserManager *instance();
};

#endif // USERMANAGER_H
