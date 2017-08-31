#include "chatroom.h"
#include "ui_chatroom.h"
#include "usermanager.h"
#include "user.h"
#include "messaginglayer.h"
#include "settings.h"
#include "mainwindow.h"
#include <QDateTime>
#include "textedit.h"

QHash<QUuid, ChatRoom *> *ChatRoom::s_instance = nullptr;

ChatRoom *ChatRoom::instance(const QUuid &uuid)
{
    if (!s_instance)
        s_instance = new QHash<QUuid, ChatRoom*>;
    if (!s_instance->contains(uuid))
    {
        ChatRoom *room = new ChatRoom(nullptr, uuid);
        MainWindow::getInstance()->addDockWidget(Qt::LeftDockWidgetArea, room);
        s_instance->insert(uuid, room);
    }
    return s_instance->value(uuid);
}

ChatRoom::ChatRoom(QWidget *parent, const QUuid &uuid) :
    QDockWidget(parent),
    ui(new Ui::ChatRoom),
    uuid(uuid)
{
    ui->setupUi(this);
    teNewMessage = new TextEdit;
    ui->frmNewMessage->setLayout(new QVBoxLayout);
    ui->frmNewMessage->layout()->addWidget(teNewMessage);

    QAction *a_send = new QAction("send");
    a_send->setShortcut(QKeySequence("Ctrl+Return"));
    teNewMessage->addAction(a_send);
    teNewMessage->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(a_send, SIGNAL(triggered(bool)), this, SLOT(sendMessage()));

    if (uuid.isNull())
    {
        ui->lwUsers->hide();
        ui->lblUsers->hide();

        setFeatures(DockWidgetFloatable | DockWidgetMovable);
        setWindowTitle("global");
    }
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
    const QString msg = teNewMessage->toHtml();
    teNewMessage->clear();
    if (msg == teNewMessage->toHtml())
        return;

    pushMessage(Settings::instance()->getUserInfo("UUID").toUuid(), msg);

    for(const QUuid &_u : users)
    {
        User *u = UserManager::instance()->getUser(_u);
        if (u)
            u->getMessagingLayer()->sendTextMessage(uuid, msg);
    }
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

    QString title;

    for(const QUuid &_u : users)
    {
        User *u = UserManager::instance()->getUser(_u);
        if (u)
        {
            const QString &user_name = u->getUserInfo()["Nickname"].toString();
            ui->lwUsers->addItem(user_name);
            if (!title.isEmpty())
                title += ", ";
            title = "<" + user_name + ">";
        }
    }

    if (!uuid.isNull())
        setWindowTitle(title);
}

void ChatRoom::pushMessage(const QUuid &author, const QString &msg)
{
    QString author_name;
    if (author != Settings::instance()->getUserInfo("UUID").toUuid())
    {
        User *u = UserManager::instance()->getUser(author);
        if (!u)     // If author is unknown, discard the message
            return;

        author_name = u->getUserInfo()["Nickname"].toString();
    }
    else
        author_name = Settings::instance()->getUserInfo("Nickname").toString();
    ui->teHistory->append("<table><tr><td><b style=\"color:#3f9fcf\">" + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + " " + author_name + ":</b></td><td>" + msg + "</td></tr></table>");
}
