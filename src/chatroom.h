#ifndef CHATROOM_H
#define CHATROOM_H

#include <QDockWidget>
#include <QUuid>
#include <QSet>

namespace Ui {
class ChatRoom;
}

class TextEdit;
class UserList;

class ChatRoom : public QDockWidget
{
    Q_OBJECT

public:
    explicit ChatRoom(QWidget *parent = 0, const QUuid &uuid = QUuid::createUuid());
    ~ChatRoom();

    const QUuid &getUUID() const;

    void addUser(const QUuid &uuid);
    void removeUser(const QUuid &uuid);

public slots:
    void pushMessage(const QUuid &author, const QString &msg);

protected slots:
    void sendMessage();
    void updateTitle();
    void takeScreenshot();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ChatRoom *ui;
    QUuid uuid;
    QSet<QUuid> users;
    TextEdit *teNewMessage;
    UserList *user_list;

private:
    static QHash<QUuid, ChatRoom *> *s_instance;
public:
    static ChatRoom *instance(const QUuid &uuid = QUuid());
};

#endif // CHATROOM_H
