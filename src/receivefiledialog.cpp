#include "receivefiledialog.h"
#include "ui_receivefiledialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include "usermanager.h"
#include "user.h"
#include "messaginglayer.h"

ReceiveFileDialog::ReceiveFileDialog(const QString &suggested_filename,
                                     const QUuid &transfer_uuid,
                                     const QUuid &sender_uuid,
                                     const qint64 file_size,
                                     QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReceiveFileDialog),
    uuid(transfer_uuid),
    sender_uuid(sender_uuid),
    filename(suggested_filename),
    file(nullptr),
    file_size(file_size)
{
    ui->setupUi(this);
    ui->pbOk->setVisible(false);
    ui->pbTransfer->setMaximum(file_size);

    User *u = UserManager::instance()->getUser(sender_uuid);
    if (!u)
    {
        deleteLater();
        return;
    }

    connect(u->getMessagingLayer(), SIGNAL(newFileChunk(QUuid,QByteArray)), this, SLOT(writeChunk(QUuid,QByteArray)));
    connect(u->getMessagingLayer(), SIGNAL(newFileCancel(QUuid)), this, SLOT(cancelTransfer(QUuid)));

    const int ret = QMessageBox::question(this, tr("%1 wants to send you a file").arg(u->getUserInfo()["Nickname"].toString()), tr("Do you accept this file transfer ?"));
    if (ret == QMessageBox::Yes)
        startTransfer();
    else
        cancelTransfer();
}

ReceiveFileDialog::~ReceiveFileDialog()
{
    delete ui;
}

void ReceiveFileDialog::changeEvent(QEvent *e)
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

void ReceiveFileDialog::cancelTransfer(const QUuid &transfer_uuid)
{
    if (transfer_uuid != uuid)
        return;
    cancelTransfer();
}

void ReceiveFileDialog::cancelTransfer()
{
    User *u = UserManager::instance()->getUser(sender_uuid);
    if (u)
        u->getMessagingLayer()->sendFileCancel(uuid);
    deleteLater();
}

void ReceiveFileDialog::startTransfer()
{
    User *u = UserManager::instance()->getUser(sender_uuid);
    if (!u)
    {
        QMessageBox::warning(this, tr("Error"), tr("Could not start file transfer: sender has disconnected!"));
        deleteLater();
        return;
    }
    filename = QFileDialog::getSaveFileName(this, tr("Save received file"), filename);
    if (filename.isEmpty())
    {
        cancelTransfer();
        return;
    }
    file = new QFile(filename, this);
    file->open(QIODevice::WriteOnly);
    if (!file->isOpen())
    {
        QMessageBox::warning(this, tr("Error"), tr("Could not open file for writing! Aborting file transfer."));
        cancelTransfer();
        return;
    }
    show();
    u->getMessagingLayer()->sendFileACK(uuid);
}

void ReceiveFileDialog::writeChunk(const QUuid &transfer_uuid, const QByteArray &data)
{
    if (transfer_uuid != uuid)
        return;

    if (!file)      // This should not happen unless sender doesn't respect the REQUEST/ACK sequence
        return;

    file->write(data);
    ui->pbTransfer->setValue(file->pos());

    if (file->pos() == file_size)       // We are done;
    {
        file->deleteLater();
        file = nullptr;

        ui->pbCancel->setVisible(false);
        ui->pbOk->setVisible(true);
    }
}
