#ifndef RECEIVEFILEDIALOG_H
#define RECEIVEFILEDIALOG_H

#include <QDialog>
#include <QUuid>

namespace Ui {
class ReceiveFileDialog;
}

class QFile;

class ReceiveFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveFileDialog(const QString &suggested_filename,
                               const QUuid &transfer_uuid,
                               const QUuid &sender_uuid,
                               const qint64 file_size,
                               QWidget *parent = 0);
    ~ReceiveFileDialog();

protected:
    void changeEvent(QEvent *e);

private slots:
    void startTransfer();
    void cancelTransfer();
    void cancelTransfer(const QUuid &transfer_uuid);
    void writeChunk(const QUuid &transfer_uuid, const QByteArray &data);

private:
    Ui::ReceiveFileDialog *ui;
    const QUuid uuid;
    const QUuid sender_uuid;
    QString filename;
    QFile *file;
    const qint64 file_size;
};

#endif // RECEIVEFILEDIALOG_H
