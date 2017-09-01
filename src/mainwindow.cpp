#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "user.h"
#include "usermanager.h"
#include "chatroom.h"
#include "userlist.h"
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMenu>

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

    loadGeometry();

    QSystemTrayIcon *trayicon = new QSystemTrayIcon(this);
    QMenu *tray_menu = new QMenu(this);
    trayicon->setContextMenu(tray_menu);
    trayicon->setIcon(windowIcon());
    tray_menu->addAction(ui->action_Settings);
    tray_menu->addAction(ui->action_Exit);

    connect(trayicon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(show()));

    trayicon->show();
}

MainWindow::~MainWindow()
{
    saveGeometry();
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

void MainWindow::loadGeometry()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    setGeometry(settings.value("geometry", geometry()).toRect());
    restoreState(settings.value("windowState", saveState()).toByteArray());
}

void MainWindow::saveGeometry()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", geometry());
    settings.setValue("windowState", saveState());
}
