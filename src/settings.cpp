#include "settings.h"
#include "ui_settings.h"
#include <QSettings>
#include <QUuid>
#include <QFileDialog>

Settings *Settings::s_instance = nullptr;

Settings *Settings::instance()
{
    if (!s_instance)
        s_instance = new Settings;

    return s_instance;
}

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(this, SIGNAL(rejected()), this, SLOT(loadSettings()));
    connect(ui->pbAvatar, SIGNAL(pressed()), this, SLOT(changeAvatar()));

    loadSettings();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::changeEvent(QEvent *e)
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

void Settings::loadSettings()
{
    QSettings config;
    config.beginGroup("main");
    settings["IncomingMessageCommand"] = config.value("IncomingMessageCommand");
    config.endGroup();

    config.beginGroup("user");
    user_info["Nickname"] = config.value("Nickname", QString::fromUtf8(qgetenv("USER")));
    user_info["FirstName"] = config.value("FirstName");
    user_info["LastName"] = config.value("LastName");
    user_info["Location"] = config.value("Location");
    user_info["Avatar"] = config.value("Avatar", QIcon(":/icons/meeting-participant.png").pixmap(128,128).toImage());
    user_info["UUID"] = config.value("UUID", QUuid::createUuid());
    config.endGroup();

    ui->leIncomingMessageCommand->setText(settings["IncomingMessageCommand"].toString());

    ui->leNickname->setText(user_info["Nickname"].toString());
    ui->leFirstName->setText(user_info["FirstName"].toString());
    ui->leLastName->setText(user_info["LastName"].toString());
    ui->leLocation->setText(user_info["Location"].toString());
    avatar = user_info["Avatar"].value<QImage>();
    ui->pbAvatar->setIcon(QIcon(QPixmap::fromImage(avatar)));
}

void Settings::saveSettings()
{
    settings["IncomingMessageCommand"] = ui->leIncomingMessageCommand->text();
    user_info["Nickname"] = ui->leNickname->text();
    user_info["FirstName"] = ui->leFirstName->text();
    user_info["LastName"] = ui->leLastName->text();
    user_info["Location"] = ui->leLocation->text();
    user_info["Avatar"] = avatar;

    QSettings config;
    config.beginGroup("main");
    for(auto it = settings.begin() ; it != settings.end() ; ++it)
        config.setValue(it.key(), it.value());
    config.endGroup();

    config.beginGroup("user");
    for(auto it = user_info.begin() ; it != user_info.end() ; ++it)
        config.setValue(it.key(), it.value());
    config.endGroup();

    emit userInfoUpdated();
}

QVariant Settings::getSetting(const QString &key) const
{
    return settings[key];
}

QVariant Settings::getUserInfo(const QString &key) const
{
    return user_info[key];
}

const QHash<QString, QVariant> &Settings::getUserInfo() const
{
    return user_info;
}

void Settings::setUserInfo(const QString &key, const QVariant &value)
{
    user_info[key] = value;

    saveSettings();
}

void Settings::changeAvatar()
{
    const QString filename = QFileDialog::getOpenFileName(this, tr("Change avatar"), QString(), tr("Images (*.jpg *.png *.gif *.bmp *.webp)"));
    if (filename.isEmpty())
        return;
    avatar = QImage(filename);
    ui->pbAvatar->setIcon(QIcon(QPixmap::fromImage(avatar)));
}
