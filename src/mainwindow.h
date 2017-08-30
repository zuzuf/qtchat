#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateUserList();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

private:
    static MainWindow *s_instance;
public:
    static MainWindow *getInstance();
};

#endif // MAINWINDOW_H
