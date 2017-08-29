#include "chatroom.h"
#include "ui_chatroom.h"
#include "usermanager.h"
#include "user.h"
#include "messaginglayer.h"
#include "settings.h"

QHash<QUuid, ChatRoom *> *ChatRoom::s_instance = nullptr;

ChatRoom *ChatRoom::instance(const QUuid &uuid)
{
    if (!s_instance)
        s_instance = new QHash<QUuid, ChatRoom*>;
    if (!s_instance->contains(uuid))
        s_instance->insert(uuid, new ChatRoom(nullptr, uuid));
    return s_instance->value(uuid);
}

ChatRoom::ChatRoom(QWidget *parent, const QUuid &uuid) :
    QDockWidget(parent),
    ui(new Ui::ChatRoom),
    uuid(uuid)
{
    ui->setupUi(this);
    connect(ui->pbSend, SIGNAL(pressed()), this, SLOT(sendMessage()));
    connect(UserManager::instance(), SIGNAL(usersUpdated()), this, SLOT(updateUserList()));
    show();
}

ChatRoom::~ChatRoom()
{
    delete ui;
}

void ChatRoom::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ChatRoom::sendMessage()
{
    const QString msg = ui->teNewMessage->toHtml();
    pushMessage(Settings::instance()->getUserInfo("UUID").toUuid(), msg);

    ui->teNewMessage->clear();
    for(const QUuid &u : users)
        UserManager::instance()->getUserList()[u]->getMessagingLayer()->sendTextMessage(uuid, msg);
}

const QUuid &ChatRoom::getUUID() const
{
    return uuid;
}

void ChatRoom::addUser(const QUuid &uuid)
{
    if (users.contains(uuid))
        return;

    users.insert(uuid);
    updateUserList();
}

void ChatRoom::removeUser(const QUuid &uuid)
{
    if (users.remove(uuid))
        updateUserList();
}

void ChatRoom::updateUserList()
{
    ui->lwUsers->clear();

    for(const QUuid &u : users)
        ui->lwUsers->addItem(UserManager::instance()->getUserList()[u]->getUserInfo()["Nickname"].toString());
}

void ChatRoom::pushMessage(const QUuid &author, const QString &msg)
{
    ui->teHistory->append(msg);
}
