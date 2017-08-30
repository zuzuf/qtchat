#ifndef SENDFILEDIALOG_H
#define SENDFILEDIALOG_H

#include <QDialog>

namespace Ui {
class SendFileDialog;
}

class SendFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendFileDialog(QWidget *parent = 0);
    ~SendFileDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SendFileDialog *ui;
};

#endif // SENDFILEDIALOG_H
