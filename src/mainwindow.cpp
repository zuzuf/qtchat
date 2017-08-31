#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "user.h"
#include "usermanager.h"
#include "chatroom.h"
#include "userlist.h"

MainWindow *MainWindow::s_instance = nullptr;

MainWindow *MainWindow::getInstance()
{
    return s_instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    s_instance = this;

    ui->setupUi(this);

    // menu file
    connect(ui->action_Exit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(ui->action_Settings, SIGNAL(triggered(bool)), Settings::instance(), SLOT(open()));

    // menu help
    connect(ui->action_About_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));

    ChatRoom::instance();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
