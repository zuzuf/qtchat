#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName(APP_NAME);
    QApplication::setOrganizationName(ORG_NAME);
    QApplication::setApplicationVersion(APP_VERSION);

    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    // look up e.g. :/i18n/qtchat_en.qm
    if (translator.load(QLocale(), QLatin1String("qtchat"), QLatin1String("_"), QLatin1String(":/i18n")))
        a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
