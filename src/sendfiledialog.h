#ifndef SENDFILEDIALOG_H
#define SENDFILEDIALOG_H

#include <QDialog>
#include <QUuid>

namespace Ui {
class SendFileDialog;
}

class QFile;

class SendFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendFileDialog(const QString &filename, QWidget *parent = 0);
    ~SendFileDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SendFileDialog *ui;
    const QString filename;
    QFile *file;
    const QUuid uuid;
};

#endif // SENDFILEDIALOG_H
