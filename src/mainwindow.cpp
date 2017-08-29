#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "user.h"
#include "usermanager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mainToolBar->addAction(ui->action_Settings);

    connect(ui->action_Exit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(ui->pbRefresh, SIGNAL(pressed()), UserManager::instance(), SLOT(refreshUserList()));

    connect(ui->action_Settings, SIGNAL(triggered(bool)), Settings::instance(), SLOT(open()));

    connect(UserManager::instance(), SIGNAL(usersUpdated()), this, SLOT(updateUserList()));
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

void MainWindow::updateUserList()
{
    ui->lwUsers->clear();
    for(User *u : UserManager::instance()->getUserList())
        ui->lwUsers->addItem(u->getUserInfo()["Nickname"].toString());
}
