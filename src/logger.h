#ifndef LOGGER_H
#define LOGGER_H

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>


using namespace std;
namespace Logger {
enum LogFlag {
    None,
    Info,
    Warning,
    Error
};


class Logger
{
public:
    Logger();
    ~Logger();
    //Adds [date|time] to beginning of every write
    void WriteToLog(LogFlag flag,QString message);
    QDir GetLogPath();
    QDir GetLogFilePath();
    QTime GetStartingTime();
    QDate GetStartingDate();
private:
    //sets m_logfilepath
    QString CreateLogFileName();
    QString CurrentTimeString();
    QDir m_LogFolderPath;
    QDir m_LogFilePath;
    QFile* m_File;
    QDateTime m_StartTime;
};

}

#endif // LOGGER_H
