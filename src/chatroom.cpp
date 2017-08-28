#include "chatroom.h"
#include "ui_chatroom.h"

ChatRoom::ChatRoom(QWidget *parent) :
	QWidget(parent),
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
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
