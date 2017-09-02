#include "userdetailsdialog.h"
#include "ui_userdetailsdialog.h"
#include "usermanager.h"
#include "user.h"

UserDetailsDialog::UserDetailsDialog(const QUuid &uuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDetailsDialog),
    uuid(uuid)
{
    ui->setupUi(this);

    connect(UserManager::instance(), SIGNAL(usersUpdated()), this, SLOT(updateInfo()));
    connect(this, SIGNAL(accepted()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(rejected()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

    updateInfo();
}

UserDetailsDialog::~UserDetailsDialog()
{
    delete ui;
}

void UserDetailsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UserDetailsDialog::updateInfo()
{
    User *u = UserManager::instance()->getUser(uuid);
    if (!u)
        return;
    ui->lblNickname->setText(u->getUserInfo()["Nickname"].toString());
    ui->lblFirstName->setText(u->getUserInfo()["FirstName"].toString());
    ui->lblLastName->setText(u->getUserInfo()["LastName"].toString());
    ui->lblLocation->setText(u->getUserInfo()["Location"].toString());

    ui->lblAvatar->setPixmap(QPixmap::fromImage(u->getUserInfo()["Avatar"].value<QImage>().scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}
