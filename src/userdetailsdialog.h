#ifndef USERDETAILSDIALOG_H
#define USERDETAILSDIALOG_H

#include <QDialog>
#include <QUuid>

namespace Ui {
class UserDetailsDialog;
}

class UserDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDetailsDialog(const QUuid &uuid, QWidget *parent = 0);
    ~UserDetailsDialog();

protected:
    void changeEvent(QEvent *e);

private slots:
    void updateInfo();

private:
    Ui::UserDetailsDialog *ui;
    const QUuid uuid;
};

#endif // USERDETAILSDIALOG_H
