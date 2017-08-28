#include "settings.h"
#include "ui_settings.h"
#include <QSettings>

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
    values.clear();

    QSettings settings;
    settings.beginGroup("main");
    settings.endGroup();

    settings.beginGroup("user");
    values["Nickname"] = settings.value("Nickname", QString::fromUtf8(qgetenv("USER")));
    settings.endGroup();

    ui->leNickname->setText(values["Nickname"].toString());
}

void Settings::saveSettings()
{
    QSettings settings;
    settings.beginGroup("main");
    settings.endGroup();

    settings.beginGroup("user");
    settings.setValue("Nickname", ui->leNickname->text());
    settings.endGroup();
}

QVariant Settings::getSetting(const QString &key) const
{
    if (values.contains(key))
        return values[key];
    return QVariant();
}
