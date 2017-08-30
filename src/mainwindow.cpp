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

    ui->mainToolBar->addAction(ui->action_Settings);

    user_list = new UserList;
    ui->frmUsers->setLayout(new QVBoxLayout);
    ui->frmUsers->layout()->addWidget(user_list);

    connect(ui->action_Exit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(ui->pbRefresh, SIGNAL(pressed()), UserManager::instance(), SLOT(refreshUserList()));

    connect(ui->action_Settings, SIGNAL(triggered(bool)), Settings::instance(), SLOT(open()));

    connect(UserManager::instance(), SIGNAL(usersUpdated()), this, SLOT(updateUserList()));

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

void MainWindow::updateUserList()
{
    user_list->clear();
    for(const QUuid &u : UserManager::instance()->getUserList().keys())
        user_list->addUser(u);
}
