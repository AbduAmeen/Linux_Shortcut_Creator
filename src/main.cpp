#include "mainwindow.h"
#include "startupdebug.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StartupDebug debug;
    debug.Run();

    MainWindow w;
    w.show();


    return a.exec();
}

