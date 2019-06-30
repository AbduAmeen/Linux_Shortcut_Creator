#include "logger.h"

namespace Logger {
Logger::Logger() : m_File(new QFile()), m_StartTime(QDateTime::currentDateTime()){
    m_LogFolderPath.mkdir("logs");
    m_LogFolderPath.cd("./logs");

    m_File->setFileName(CreateLogFileName());
    m_File->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::NewOnly);

    WriteToLog(LogFlag::None, "Started Logging");
}

void Logger::WriteToLog(LogFlag flag, QString message) {
    QTextStream TextStream(m_File);
    switch (flag) {
    case LogFlag::None:
        TextStream << CurrentTimeString().toUtf8() << " " << message.toUtf8() << "\n";
        break;
    case LogFlag::Info:
        TextStream << CurrentTimeString().toUtf8() << "[Info] " <<  message.toUtf8() << "\n";
        break;
    case LogFlag::Warning:
        TextStream << CurrentTimeString().toUtf8() << "[Warning] " << message.toUtf8() << "\n";
        break;
    case LogFlag::Error:
        TextStream << CurrentTimeString().toUtf8() << "[Error] " << message.toUtf8() << "\n";
        break;
    }
    TextStream.flush();
}

QString Logger::CurrentTimeString() {
    QDateTime time = QDateTime::currentDateTime();

    if (time.time().minute() < 10){
        return QString("[%1-%2-%3][%4:0%5]").arg(time.date().month()).arg(time.date().day()).arg(time.date().year()).arg(time.time().hour()).arg(time.time().minute());
    }
    return QString("[%1-%2-%3][%4:%5]").arg(time.date().month()).arg(time.date().day()).arg(time.date().year()).arg(time.time().hour()).arg(time.time().minute());
}

QString Logger::CreateLogFileName() {
    QString string = m_LogFolderPath.path();

    if (m_StartTime.time().minute() <= 9) {
        if (m_StartTime.time().second() <= 9) {
            string += QString("/%1-%2-%3 %4:0%5:0%6.txt").arg(m_StartTime.date().month())
                                                        .arg(m_StartTime.date().day())
                                                        .arg(m_StartTime.date().year())
                                                        .arg(m_StartTime.time().hour())
                                                        .arg(m_StartTime.time().minute())
                                                        .arg(m_StartTime.time().second());
        }
        string += QString("/%1-%2-%3 %4:0%5:%6.txt").arg(m_StartTime.date().month())
                                                    .arg(m_StartTime.date().day())
                                                    .arg(m_StartTime.date().year())
                                                    .arg(m_StartTime.time().hour())
                                                    .arg(m_StartTime.time().minute())
                                                    .arg(m_StartTime.time().second());
    }
    else {
        if (m_StartTime.time().second() <= 9) {
            string += QString("/%1-%2-%3 %4:%5:0%6.txt").arg(m_StartTime.date().month())
                                                        .arg(m_StartTime.date().day())
                                                        .arg(m_StartTime.date().year())
                                                        .arg(m_StartTime.time().hour())
                                                        .arg(m_StartTime.time().minute())
                                                        .arg(m_StartTime.time().second());
        }
        string += QString("/%1-%2-%3 %4:%5:%6.txt").arg(m_StartTime.date().month())
                                                    .arg(m_StartTime.date().day())
                                                    .arg(m_StartTime.date().year())
                                                    .arg(m_StartTime.time().hour())
                                                    .arg(m_StartTime.time().minute())
                                                    .arg(m_StartTime.time().second());
    }
    m_LogFilePath.setPath(string);
    return string;
}

QDir Logger::GetLogPath() {
    return m_LogFolderPath;
}

QDir Logger::GetLogFilePath() {
    return m_LogFilePath;
}

QTime Logger::GetStartingTime() {
    return m_StartTime.time();
}

QDate Logger::GetStartingDate(){
    return m_StartTime.date();
}

Logger::~Logger(){
    m_File->close();
    delete m_File;
}
}
