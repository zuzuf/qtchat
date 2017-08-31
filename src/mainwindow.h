#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class UserList;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void loadGeometry();
    void saveGeometry();

private:
    Ui::MainWindow *ui;

private:
    static MainWindow *s_instance;
public:
    static MainWindow *getInstance();
};

#endif // MAINWINDOW_H
