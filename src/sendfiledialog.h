#ifndef SENDFILEDIALOG_H
#define SENDFILEDIALOG_H

#include <QDialog>
#include <QUuid>

namespace Ui {
class SendFileDialog;
}

class QFile;
class QTimer;

class SendFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendFileDialog(const QString &filename,
                            const QUuid &receiver_uuid,
                            QWidget *parent = 0);
    ~SendFileDialog();

public slots:
    void cancelTransfer();

private slots:
    void startTransfer(const QUuid &transfer_uuid);
    void cancelTransfer(const QUuid &transfer_uuid);
    void transferChunk();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SendFileDialog *ui;
    const QString filename;
    QFile *file;
    const QUuid uuid;
    const QUuid receiver_uuid;
    QTimer *timer;
};

#endif // SENDFILEDIALOG_H
