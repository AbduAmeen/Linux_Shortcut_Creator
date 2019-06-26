#ifndef STARTUPDEBUG_H
#define STARTUPDEBUG_H

#include <QWidget>
#include <QDir>

#include "crashwindow.h"

class StartupDebug
{
public:
    StartupDebug();
    CrashWindow* CreateDebugWindow();
    bool CheckForCrashes();
    bool CheckForLogsDir();
    void Run();

private:
    QDir AppPath;
    QDir LogDir;
};

#endif // STARTUPDEBUG_H
