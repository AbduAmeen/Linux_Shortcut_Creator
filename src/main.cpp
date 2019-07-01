#include "theapp.h"
#include "mainwindow.h"
#include "startupdebug.h"
#include "logger.h"
#include "config.h"

int main(int argc, char *argv[])
{
    auto logger = std::make_shared<Logger::Logger>();
    StartupDebug::StartupDebug debug(logger);
    TheApp a(argc, argv, logger);

    switch (debug.Run()) {
    case QDialog::Accepted:
        logger->WriteToLog(Logger::Info, "User chose to close app from crash window");
        a.quit();
        return 0;
    case QDialog::Rejected:
        logger->WriteToLog(Logger::Info, "User chose to start application");
        break;
    case 2:
        a.quit();
        return 0;
    default:
        logger->WriteToLog(Logger::Info, "User chose to close app from crash window");
        a.quit();
        return 0;
    }
    Config cfg(logger);
    MainWindow w;
    w.show();

    return a.exec();
}

