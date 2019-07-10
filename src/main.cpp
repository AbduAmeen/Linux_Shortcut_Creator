#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkSession>
#include <QtCore/QSettings>

#include "network.h"
#include "chatdialog.h"
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
        logger->WriteToLog(Logger::Info, "Starting main application");
        break;
    case 2:
        a.quit();
        return 0;
    default:
        logger->WriteToLog(Logger::Info, "User chose to close app from crash window");
        a.quit();
        return 0;
    }
    Config cfg("config",QSettings::Format::IniFormat,logger);
    cfg.CheckAndLogStatus();
    QNetworkConfigurationManager manager;

   if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
       // Get saved network configuration
       cfg.beginGroup(QLatin1String("Network"));
       const QString id = cfg.value(QLatin1String("DefaultNetworkConfiguration")).toString();
       cfg.endGroup();

       // If the saved network configuration is not currently discovered use the system default
       QNetworkConfiguration config = manager.configurationFromIdentifier(id);
       if ((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered) {
           config = manager.defaultConfiguration();
       }

       QNetworkSession* networkSession = new QNetworkSession(config, &a);
       networkSession->open();
       networkSession->waitForOpened();

       if (networkSession->isOpen()) {
           // Save the used configuration
           QNetworkConfiguration config = networkSession->configuration();
           QString id;
           if (config.type() == QNetworkConfiguration::UserChoice) {
               id = networkSession->sessionProperty(
                       QLatin1String("UserChoiceConfiguration")).toString();
           } else {
               id = config.identifier();
           }

           cfg.beginGroup(QLatin1String("Network"));
           cfg.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
           cfg.endGroup();
       }
   }


   MainWindow window;
   window.show();
   ChatDialog dialog;
   dialog.show();

   return a.exec();
}

