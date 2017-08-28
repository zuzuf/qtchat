#include "chatroom.h"
#include "ui_chatroom.h"

ChatRoom::ChatRoom(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ChatRoom)
{
    ui->setupUi(this);
}

ChatRoom::~ChatRoom()
{
    delete ui;
}

void ChatRoom::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
