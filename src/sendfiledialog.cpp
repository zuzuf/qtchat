#include "sendfiledialog.h"
#include "ui_sendfiledialog.h"
#include <QFile>
#include <QMessageBox>
#include <QTimer>

SendFileDialog::SendFileDialog(const QString &filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFileDialog),
    filename(filename),
    uuid(QUuid::createUuid())
{
    ui->setupUi(this);
    ui->pbOk->setVisible(false);

    file = new QFile(filename, this);
    file->open(QIODevice::ReadOnly);
    if (!file->isOpen())
    {
        QMessageBox::warning(this, tr("Could not read file"), tr("Could not open file '%1' for reading!").arg(filename));
        deleteLater();
    }
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
