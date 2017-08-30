#ifndef USERLIST_H
#define USERLIST_H

#include <QScrollArea>
#include <QUuid>
#include <QHash>

class UserWidget;

class UserList : public QScrollArea
{
    Q_OBJECT
public:
    explicit UserList(QWidget *parent = nullptr);

signals:

public slots:
    void clear();
    void addUser(const QUuid &uuid);
    void removeUser(const QUuid &uuid);

private:
    QHash<QUuid, UserWidget*> users;
};

#endif // USERLIST_H
