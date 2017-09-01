#include "userwidget.h"
#include "usermanager.h"
#include "user.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QIcon>
#include <QAction>
#include <QFileDialog>
#include "sendfiledialog.h"
#include "chatroom.h"

UserWidget::UserWidget(const QUuid &uuid)
    : uuid(uuid)
{
    QHBoxLayout *layout = new QHBoxLayout;
    setLayout(layout);

    layout->setContentsMargins(0,0,0,0);

    lbl_icon = new QLabel;
    lbl_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl_icon->setMinimumSize(64, 64);
    lbl_icon->setMaximumSize(64, 64);
    layout->addWidget(lbl_icon);

    lbl_nickname = new QLabel;
    lbl_nickname->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    lbl_nickname->setMinimumHeight(48);
    lbl_nickname->setMaximumHeight(48);
    layout->addWidget(lbl_nickname);

    layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding));

    updateInfo();

    QAction *action_sendFile = new QAction(tr("Send file"));
    connect(action_sendFile, SIGNAL(triggered(bool)), this, SLOT(sendFile()));
    addAction(action_sendFile);

    QAction *action_privateChat = new QAction(tr("Private chat"));
    connect(action_privateChat, SIGNAL(triggered(bool)), this, SLOT(startPrivateChat()));
    addAction(action_privateChat);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(UserManager::instance(), SIGNAL(usersUpdated()), this, SLOT(updateInfo()));
}

void UserWidget::sendFile()
{
    const QString &filename = QFileDialog::getOpenFileName(this, tr("Send file to ") + (lbl_nickname ? lbl_nickname->text() : QString()));
    if (filename.isEmpty()) // Aborted
        return;

    SendFileDialog *send_dlg = new SendFileDialog(filename, uuid);
    send_dlg->show();
}

void UserWidget::startPrivateChat()
{
    ChatRoom::instance(QUuid::createUuid())->addUser(uuid);
}

void UserWidget::updateInfo()
{
    User *u = UserManager::instance()->getUser(uuid);
    QIcon icon;
    const QString status = u ? u->getUserInfo()["Status"].toString() : QString("offline");
    setVisible(status != "invisible");
    if (status == "away")
    {
        setToolTip(tr("away"));
        icon = QIcon::fromTheme(":/icons/im-user-away.png");
    }
    else if (status == "busy")
    {
        setToolTip(tr("busy"));
        icon = QIcon::fromTheme(":/icons/im-user-busy.png");
    }
    else if (status == "offline")
    {
        setToolTip(tr("offline"));
        icon = QIcon::fromTheme(":/icons/im-user-offline.png");
    }
    else if (status == "invisible")
    {
        setToolTip(tr("invisible"));
        icon = QIcon::fromTheme(":/icons/im-invisible-user.png");
    }
    else
    {
        setToolTip(tr("online"));
        icon = QIcon::fromTheme(":/icons/im-user.png");
    }
    lbl_icon->setPixmap(icon.pixmap(48,48));
    if (u)
        lbl_nickname->setText(u->getUserInfo()["Nickname"].toString());
}

void UserWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    setBackgroundRole(QPalette::Background);
    setAutoFillBackground(false);
}

void UserWidget::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    setBackgroundRole(QPalette::Highlight);
    setAutoFillBackground(true);
}
