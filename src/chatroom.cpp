#include "chatroom.h"
#include "ui_chatroom.h"
#include "usermanager.h"
#include "user.h"
#include "messaginglayer.h"
#include "settings.h"
#include "mainwindow.h"
#include <QDateTime>
#include "textedit.h"
#include "userlist.h"
#include <QProcess>
#include <QScreen>
#include <QWindow>

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

        if (!uuid.isNull())
            MainWindow::getInstance()->tabifyDockWidget(instance(), room);
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
    ui->frmNewMessage->layout()->setContentsMargins(0,0,0,0);
    ui->frmNewMessage->layout()->addWidget(teNewMessage);

    user_list = new UserList;
    ui->frmUsers->setLayout(new QVBoxLayout);
    ui->frmUsers->layout()->setContentsMargins(0,0,0,0);
    ui->frmUsers->layout()->addWidget(user_list);

    connect(ui->pbRefresh, SIGNAL(pressed()), UserManager::instance(), SLOT(refreshUserList()));

    QAction *a_send = new QAction(QIcon(":/icons/fork.png"), "send", teNewMessage);
    a_send->setShortcutContext(Qt::WidgetShortcut);
    a_send->setShortcut(QKeySequence("Ctrl+Return"));
    teNewMessage->addAction(a_send);
    teNewMessage->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(a_send, SIGNAL(triggered(bool)), this, SLOT(sendMessage()));

    QAction *a_screenshot = new QAction(QIcon(":/icons/ksnapshot.png"), "screenshot", teNewMessage);
    connect(a_screenshot, SIGNAL(triggered(bool)), this, SLOT(takeScreenshot()));
    teNewMessage->addAction(a_screenshot);

    connect(ui->cbEmotes, SIGNAL(activated(int)), this, SLOT(insertEmote(int)));
    connect(ui->cbStatus, SIGNAL(activated(int)), this, SLOT(updateStatus(int)));
    connect(Settings::instance(), SIGNAL(userInfoUpdated()), this, SLOT(updateStatus()));

    if (uuid.isNull())
    {
        setFeatures(NoDockWidgetFeatures);
        setWindowTitle(tr("Global chat"));
    }
    else
        ui->pbRefresh->setVisible(false);
    connect(ui->pbSend, SIGNAL(pressed()), this, SLOT(sendMessage()));
    connect(UserManager::instance(), SIGNAL(usersUpdated()), this, SLOT(updateTitle()));
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
    user_list->addUser(uuid);
    updateTitle();
}

void ChatRoom::removeUser(const QUuid &uuid)
{
    if (users.remove(uuid))
    {
        user_list->removeUser(uuid);
        updateTitle();
    }
}

void ChatRoom::updateTitle()
{
    if (uuid.isNull())
        return;

    QString title;

    for(const QUuid &_u : users)
    {
        User *u = UserManager::instance()->getUser(_u);
        if (u)
        {
            const QString &user_name = u->getUserInfo()["Nickname"].toString();
            if (!title.isEmpty())
                title += ", ";
            title = "<" + user_name + ">";
        }
    }
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

        const QString &cmd_on_incoming_messages = Settings::instance()->getSetting("IncomingMessageCommand").toString();
        if (!cmd_on_incoming_messages.isEmpty())
        {
            QProcess *child = new QProcess(this);
            connect(child, SIGNAL(finished(int)), child, SLOT(deleteLater()));
            connect(child, SIGNAL(errorOccurred(QProcess::ProcessError)), child, SLOT(deleteLater()));

            child->start(cmd_on_incoming_messages);
        }
    }
    else
        author_name = Settings::instance()->getUserInfo("Nickname").toString();
    ui->teHistory->append("<table><tr><td><b style=\"color:#3f9fcf;\">" + QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) + " " + author_name + ":</b></td><td>" + msg + "</td></tr></table>");
}

void ChatRoom::takeScreenshot()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle())
        screen = window->screen();
    if (!screen)
        return;

    const QImage screenshot = screen->grabWindow(0).toImage();
    teNewMessage->dropImage(screenshot.scaled(screenshot.width() / 2, screenshot.height() / 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void ChatRoom::insertEmote(int id)
{
    teNewMessage->dropImage(ui->cbEmotes->itemIcon(id).pixmap(24, 24).toImage());
}

void ChatRoom::updateStatus(int status)
{
    const char *status_table[] = { "online", "away", "busy", "offline", "invisible" };
    Settings::instance()->setUserInfo("Status", status_table[status]);
}

void ChatRoom::updateStatus()
{
    const char *status_table[] = { "online", "away", "busy", "offline", "invisible" };
    const QString &status = Settings::instance()->getUserInfo("Status").toString();
    int status_id = 0;
    for(int i = 0 ; i < sizeof(status_table) / sizeof(status_table[0]) ; ++i)
        if (status == status_table[i])
        {
            status_id = i;
            break;
        }
    ui->cbStatus->setCurrentIndex(status_id);
}
