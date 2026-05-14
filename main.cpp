#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Alsweider");
    a.setApplicationName("Keleust");

    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
