#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Alsweider");
    a.setApplicationName("Keleust");
    a.setWindowIcon(QIcon(":/bilder/icon.ico"));

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
