#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("PlayQt");
    MainWindow mw;
    mw.show();
    return a.exec();
}
