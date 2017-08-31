#include "userwidget.h"
#include "usermanager.h"
#include "user.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QIcon>
#include <QAction>
#include <QFileDialog>
#include "sendfiledialog.h"

UserWidget::UserWidget(const QUuid &uuid)
    : uuid(uuid)
{
    User *u = UserManager::instance()->getUser(uuid);

    setLayout(new QHBoxLayout);

    QLabel *lbl_icon = new QLabel;
    QIcon icon = QIcon::fromTheme("user-available");
    lbl_icon->setPixmap(icon.pixmap(48,48));
    lbl_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl_icon->setMinimumSize(48, 48);
    lbl_icon->setMaximumSize(48, 48);
    layout()->addWidget(lbl_icon);
    if (u)
    {
        lbl_nickname = new QLabel(u->getUserInfo()["Nickname"].toString());
        layout()->addWidget(lbl_nickname);
        lbl_nickname->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        lbl_nickname->setMinimumHeight(48);
        lbl_nickname->setMaximumHeight(48);
    }
    else
        lbl_nickname = nullptr;

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    setMinimumHeight(48);
    setMaximumHeight(48);

    QAction *action_sendFile = new QAction(tr("Send file"));
    connect(action_sendFile, SIGNAL(triggered(bool)), this, SLOT(sendFile()));
    addAction(action_sendFile);

    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void UserWidget::sendFile()
{
    const QString &filename = QFileDialog::getOpenFileName(this, tr("Send file to ") + (lbl_nickname ? lbl_nickname->text() : QString()));
    if (filename.isEmpty()) // Aborted
        return;

    SendFileDialog *send_dlg = new SendFileDialog(filename, uuid);
    send_dlg->show();
}
