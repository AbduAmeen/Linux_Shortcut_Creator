#ifndef STARTUPDEBUG_H
#define STARTUPDEBUG_H

#include <QWidget>
#include <QDir>

class StartupDebug
{
public:
    StartupDebug();
    QWidget* CreateDebugWindow();
    bool CheckForCrashes();

private:
    QDir AppPath;
};

#endif // STARTUPDEBUG_H
