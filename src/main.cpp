#include "theapp.h"
#include "mainwindow.h"
#include "startupdebug.h"
#include "logger.h"

int main(int argc, char *argv[])
{
    auto logger = std::make_shared<Logger::Logger>();
    TheApp a(argc, argv, logger);
    StartupDebug::StartupDebug debug(logger);
    debug.Run();

    MainWindow w;
    w.show();


    return a.exec();
}

