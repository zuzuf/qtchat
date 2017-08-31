#include "sendfiledialog.h"
#include "ui_sendfiledialog.h"
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include "usermanager.h"
#include "user.h"
#include "messaginglayer.h"
#include <QTimer>

#define CHUNK_SIZE  0x100000

SendFileDialog::SendFileDialog(const QString &filename, const QUuid &receiver_uuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFileDialog),
    filename(filename),
    uuid(QUuid::createUuid()),
    receiver_uuid(receiver_uuid)
{
    timer = nullptr;

    ui->setupUi(this);
    ui->pbOk->setVisible(false);

    file = new QFile(filename, this);
    file->open(QIODevice::ReadOnly);
    if (!file->isOpen())
    {
        QMessageBox::warning(this, tr("Could not read file"), tr("Could not open file '%1' for reading!").arg(filename));
        deleteLater();
        return;
    }

    ui->pbTransfer->setMaximum(file->size());

    connect(this, SIGNAL(rejected()), this, SLOT(cancelTransfer()));
    connect(this, SIGNAL(accepted()), this, SLOT(deleteLater()));

    User *u = UserManager::instance()->getUser(receiver_uuid);
    if (!u)
    {
        QMessageBox::warning(this, tr("Could not send file"), tr("User is not connected!"));
        deleteLater();
        return;
    }
    connect(u->getMessagingLayer(), SIGNAL(newFileACK(QUuid)), this, SLOT(startTransfer(QUuid)));
    connect(u->getMessagingLayer(), SIGNAL(newFileCancel(QUuid)), this, SLOT(cancelTransfer()));

    u->getMessagingLayer()->sendFileRequest(uuid, QFileInfo(filename).fileName(), file->size());
}

SendFileDialog::~SendFileDialog()
{
    delete ui;
}

void SendFileDialog::changeEvent(QEvent *e)
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

void SendFileDialog::startTransfer(const QUuid &transfer_uuid)
{
    if (transfer_uuid != uuid)
        return;

    if (timer)
        return;

    timer = new QTimer(this);
    timer->setInterval(100);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(transferChunk()));
    timer->start();
}

void SendFileDialog::transferChunk()
{
    if (file->pos() >= file->size())    // Transfer finished
    {
        timer->stop();
        ui->pbCancel->setVisible(false);
        ui->pbOk->setVisible(true);
        return;
    }

    const QByteArray &chunk_data = file->read(CHUNK_SIZE);
    User *u = UserManager::instance()->getUser(receiver_uuid);
    if (!u)     // User disconnected ?
        return;

    ui->pbTransfer->setValue(file->pos());
    u->getMessagingLayer()->sendFileChunk(uuid, chunk_data);
}

void SendFileDialog::cancelTransfer()
{
    User *u = UserManager::instance()->getUser(receiver_uuid);
    if (u)     // User connected ?
        u->getMessagingLayer()->sendFileCancel(uuid);
    deleteLater();
}

void SendFileDialog::cancelTransfer(const QUuid &transfer_uuid)
{
    if (transfer_uuid != uuid)
        return;

    cancelTransfer();
}
