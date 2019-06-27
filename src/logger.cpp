#include "logger.h"

Logger::Logger() : file(new QFile()){
    time_t temptime = time(0);

    m_time = localtime(&temptime);
    m_starttime = *localtime(&temptime);

    m_LogFilePath.mkdir("logs");
    m_LogFilePath.cd("./logs");

    QString logfilepath = m_LogFilePath.path() + QString("/%1\\%2\\%3 %4:%5.txt").arg(m_starttime.tm_mon).arg(m_starttime.tm_mday).arg(m_starttime.tm_year + 1900).arg(m_starttime.tm_hour).arg(m_starttime.tm_min);

    file->setFileName(logfilepath);
    file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::NewOnly);
}

QDir Logger::LogPath() {
    return m_LogFilePath;
}
tm Logger::StartingTime() {
    return m_starttime;
}
Logger::~Logger(){
    delete file;
}
