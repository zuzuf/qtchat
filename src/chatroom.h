#ifndef CHATROOM_H
#define CHATROOM_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

namespace Ui {
	class ChatRoom;
}

class ChatRoom : public QWidget
{
	Q_OBJECT

public:
	explicit ChatRoom(QWidget *parent = 0);
	~ChatRoom();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::ChatRoom *ui;
};

#endif // CHATROOM_H
