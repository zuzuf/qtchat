#include "userlist.h"
#include "userwidget.h"
#include <QVBoxLayout>

UserList::UserList(QWidget *parent) : QScrollArea(parent)
{
    setWidget(new QWidget);
    setWidgetResizable(true);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    widget()->setLayout(new QVBoxLayout);
}

void UserList::clear()
{
    for(UserWidget *uw : users)
        uw->deleteLater();
    users.clear();
}

void UserList::addUser(const QUuid &uuid)
{
    if (users.contains(uuid))   // Already in ?
        return;

    UserWidget *uw = new UserWidget(uuid);
    widget()->layout()->addWidget(uw);
    users[uuid] = uw;
}

void UserList::removeUser(const QUuid &uuid)
{
    if (!users.contains(uuid))  // Can't remove it if it's not there
        return;

    users[uuid]->deleteLater();
    users.remove(uuid);
}
