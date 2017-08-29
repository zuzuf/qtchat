#ifndef CHATROOM_H
#define CHATROOM_H

#include <QDockWidget>
#include <QUuid>
#include <QList>

namespace Ui {
class ChatRoom;
}

class ChatRoom : public QDockWidget
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
