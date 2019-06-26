#include "startupdebug.h"


StartupDebug::StartupDebug() {

}

CrashWindow* StartupDebug::CreateDebugWindow() {
    CrashWindow* window = new CrashWindow;
    return window;
}

bool StartupDebug::CheckForCrashes() {
    return true;
}

bool StartupDebug::CheckForLogsDir() {
    QString path = AppPath.path();

    path.chop(path.lastIndexOf("Shortcut"));
    AppPath = path;
    path.append("\\logs");
    LogDir = path;

    if (!LogDir.exists()){
        AppPath.mkdir("Logs");
        //return false;
    }
    return true;
}

void StartupDebug::Run() {
    if (!CheckForLogsDir()) {return;}

    if (!CheckForCrashes()) {return;}

    CrashWindow* window = CreateDebugWindow();
    window->exec();

}
