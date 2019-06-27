#ifndef LOGGER_H
#define LOGGER_H

#include <QDir>
#include <QFile>
#include <ctime>

using namespace std;

class Logger
{
public:
    Logger();
    ~Logger();
    QDir LogPath();
    tm StartingTime();
private:
    QDir m_LogFilePath;
    QFile* file;
    tm m_starttime;
    tm* m_time;
};



#endif // LOGGER_H
