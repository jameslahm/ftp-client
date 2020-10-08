#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setAttribute(Qt::AA_DontShowIconsInMenus);

    MainWindow w;
    w.show();
    w.centerAndResize();

    w.setWindowIcon(QIcon(":/images/ftp.png"));

    return a.exec();
}
