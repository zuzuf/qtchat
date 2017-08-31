#include "settings.h"
#include "ui_settings.h"
#include <QSettings>
#include <QUuid>

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
    settings.clear();
    user_info.clear();

    QSettings config;
    config.beginGroup("main");
    settings["IncomingMessageCommand"] = config.value("IncomingMessageCommand");
    config.endGroup();

    config.beginGroup("user");
    user_info["Nickname"] = config.value("Nickname", QString::fromUtf8(qgetenv("USER")));
    user_info["UUID"] = config.value("UUID", QUuid::createUuid());
    config.endGroup();

    ui->leIncomingMessageCommand->setText(settings["IncomingMessageCommand"].toString());

    ui->leNickname->setText(user_info["Nickname"].toString());
}

void Settings::saveSettings()
{
    settings["IncomingMessageCommand"] = ui->leIncomingMessageCommand->text();
    user_info["Nickname"] = ui->leNickname->text();

    QSettings config;
    config.beginGroup("main");
    for(auto it = settings.begin() ; it != settings.end() ; ++it)
        config.setValue(it.key(), it.value());
    config.endGroup();

    config.beginGroup("user");
    for(auto it = user_info.begin() ; it != user_info.end() ; ++it)
        config.setValue(it.key(), it.value());
    config.endGroup();
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
