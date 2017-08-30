#ifndef RECEIVEFILEDIALOG_H
#define RECEIVEFILEDIALOG_H

#include <QDialog>

namespace Ui {
class ReceiveFileDialog;
}

class ReceiveFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveFileDialog(QWidget *parent = 0);
    ~ReceiveFileDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ReceiveFileDialog *ui;
};

#endif // RECEIVEFILEDIALOG_H
