#ifndef STARTUPDEBUG_H
#define STARTUPDEBUG_H

#include <QWidget>
#include <QDir>
#include <memory>

#include "crashwindow.h"
#include "logger.h"

class StartupDebug
{
public:
    StartupDebug(std::shared_ptr<Logger> ptr);
    CrashWindow* CreateDebugWindow();
    bool CheckForCrashes();
    bool CheckForLogsDir();
    void Run();

private:
    QDir m_AppPath;
    QDir m_LogDir;
    std::shared_ptr<Logger> m_logger_ptr;
};

#endif // STARTUPDEBUG_H
