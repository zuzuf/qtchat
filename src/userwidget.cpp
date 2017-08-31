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
    setLayout(new QHBoxLayout);

    lbl_icon = new QLabel;
    lbl_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl_icon->setMinimumSize(48, 48);
    lbl_icon->setMaximumSize(48, 48);
    layout()->addWidget(lbl_icon);

    lbl_nickname = new QLabel;
    lbl_nickname->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    lbl_nickname->setMinimumHeight(48);
    lbl_nickname->setMaximumHeight(48);
    layout()->addWidget(lbl_nickname);

    updateInfo();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    setMinimumHeight(48);
    setMaximumHeight(48);

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
    QIcon icon = QIcon::fromTheme("user-available");
    lbl_icon->setPixmap(icon.pixmap(48,48));
    if (u)
        lbl_nickname->setText(u->getUserInfo()["Nickname"].toString());
    else
        lbl_nickname->setText(QString());
}
