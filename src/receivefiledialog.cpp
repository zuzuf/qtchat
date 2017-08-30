#include "receivefiledialog.h"
#include "ui_receivefiledialog.h"

ReceiveFileDialog::ReceiveFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReceiveFileDialog)
{
    ui->setupUi(this);
    ui->pbOk->setVisible(false);
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
