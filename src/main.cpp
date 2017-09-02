#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Qtchat");
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    // look up e.g. :/i18n/qtchat_en.qm
    if (translator.load(QLocale(), QLatin1String("qtchat"), QLatin1String("_"), QLatin1String(":/i18n")))
        a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
