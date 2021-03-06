#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QHash>
#include <QString>
#include <QVariant>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    QVariant getSetting(const QString &key) const;
    QVariant getUserInfo(const QString &key) const;
    const QHash<QString, QVariant> &getUserInfo() const;
    void setUserInfo(const QString &key, const QVariant &value);

signals:
    void userInfoUpdated();

protected:
    void changeEvent(QEvent *e);

protected slots:
    void loadSettings();
    void saveSettings();
    void changeAvatar();

private:
    Ui::Settings *ui;
    QHash<QString, QVariant> settings;
    QHash<QString, QVariant> user_info;
    QImage avatar;

private:
    static Settings *s_instance;

public:
    static Settings *instance();
};

#endif // SETTINGS_H
