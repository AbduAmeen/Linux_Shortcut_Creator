#include "startupdebug.h"
#include <QFile>

StartupDebug::StartupDebug()
{

}

bool StartupDebug::CheckForCrashes(){
    QFile file;
    QString path = AppPath.path();

    path.chop(path.lastIndexOf("Shortcut"));
    path.append("\\logs");
    return file.exists(path);
}
