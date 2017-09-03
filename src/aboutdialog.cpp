#include "aboutdialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "mainwindow.h"

AboutDialog *AboutDialog::s_instance = nullptr;

AboutDialog *AboutDialog::instance()
{
    if (!s_instance)
        s_instance = new AboutDialog(MainWindow::getInstance());

    return s_instance;
}

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("About QtChat"));

    setLayout(new QVBoxLayout);
    layout()->addWidget(new QLabel(tr("QtChat\n"
                                      "------\n"
                                      "\n"
                                      "QtChat is a simple chat program which doesn't rely on a server.\n"
                                      "It uses broadcasting over local network (LAN) to discover other users.\n"
                                      "\n"
                                      "Features include:\n"
                                      "- global chat room\n"
                                      "- private chat\n"
                                      "- avatars\n"
                                      "- user status\n"
                                      "- rich text messages\n"
                                      "- drag & drop of images and text files\n"
                                      "- snapshots\n"
                                      "- sending/receiving files\n"
                                      "- transparent compression\n"
                                      "- support for translations\n"
                                      "\n"
                                      "Custom messaging protocol was designed for Qt 5.2 and above.")));

    QPushButton *pbOk = new QPushButton(QIcon(":/icons/dialog-ok-apply.png"), tr("OK"));
    layout()->addWidget(pbOk);

    connect(pbOk, SIGNAL(pressed()), this, SLOT(close()));
}
