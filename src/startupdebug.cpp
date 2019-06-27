#include "startupdebug.h"


StartupDebug::StartupDebug(std::shared_ptr<Logger> ptr) : m_logger_ptr(ptr){

}

CrashWindow* StartupDebug::CreateDebugWindow() {
    CrashWindow* window = new CrashWindow;
    return window;
}

bool StartupDebug::CheckForCrashes() {
    return true;
}

bool StartupDebug::CheckForLogsDir() {
    m_LogDir = m_logger_ptr->LogPath();

    if (!m_LogDir.exists()){
        return false;
    }
    return true;
}

void StartupDebug::Run() {
    if (!CheckForLogsDir()) {return;}

    if (!CheckForCrashes()) {return;}

    CrashWindow* window = CreateDebugWindow();
    window->exec();
}
