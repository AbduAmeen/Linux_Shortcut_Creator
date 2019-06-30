#include "startupdebug.h"

using namespace std;
namespace StartupDebug {

StartupDebug::StartupDebug(std::shared_ptr<Logger::Logger> ptr) : m_logger_ptr(ptr) {
    m_LogDir = m_logger_ptr->GetLogPath();
}

bool StartupDebug::CheckForCrashes() {
    QFileInfo lastlogfile = GetLastLogFile(m_LogDir.entryInfoList());

    if (lastlogfile.exists()) {
        QFile file(lastlogfile.filePath());

        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);

            while (!stream.atEnd()) {
                QString tempstr = stream.readLine();

                if (tempstr.contains("[Error]")) {
                    m_logger_ptr->WriteToLog(Logger::Info, "Found log with crash");
                    m_lastlogfile = lastlogfile;
                    file.close();
                    return true;
                }
            }
            file.close();
        }
    }
    return false;
}

void StartupDebug::CleanLogFiles(){
    QStringList list(m_LogDir.entryList());
    list.removeAt(0);
    list.removeAt(0);

    while (list.count() > 20) {
        QString str;
        for (int p = 0; p < list.count(); p++) {
            str.clear();
            QDateTime temptime = ParseLogName(list.at(p));
            if (temptime.isNull()) {
                str = list.at(p);
                list.removeAt(p);
                break;
            }
        }

        if (!str.isEmpty()){
            m_LogDir.remove(str);
        }
        else {
            auto temp = GetOldestLogFile(m_LogDir.entryInfoList());
            m_LogDir.remove(temp.fileName());
            list.removeOne(temp.fileName());
        }
    }
}

int StartupDebug::Run() {
    CleanLogFiles();

    if (!CheckForCrashes()) {
        return 0;
    }

    CrashWindow* window = new CrashWindow(m_logger_ptr, m_lastlogfile);
    int exitcode = 0;

    if (window != nullptr) {
        m_logger_ptr->WriteToLog(Logger::LogFlag::Info, "Created Crash Handler Window Sucessfully");
        exitcode = window->exec();
    }
    else {
        m_logger_ptr->WriteToLog(Logger::LogFlag::Error, "Crash Handler Window Creation Failed");
        return 2;
    }

    delete window;
    return exitcode;
}

void StartupDebug::Crash(){
    m_logger_ptr->WriteToLog(Logger::Error, "Crash");
}

QDateTime StartupDebug::ParseLogName(QString name) {
    QDateTime datetime;
    bool flag;
    bool flag2;
    int minute;
    int second;
    int whitespace = name.indexOf(" ");
    int firstcolon = name.indexOf(":",whitespace);
    int secondcolon = name.indexOf(":",firstcolon + 1);
    int firsthyphen = name.indexOf("-");
    int secondhyphen = name.indexOf("-",firsthyphen + 1);

    int hour = name.mid(whitespace, firstcolon-whitespace).toInt(&flag);

    if (flag) {
        minute = name.mid(firstcolon + 1, 2).toInt(&flag);

        if (flag) {
            second = name.mid(secondcolon + 1, 2).toInt(&flag);

            if (flag) {
                datetime.setTime(QTime(hour,minute,second));
            }
        }
    }

    int month = name.left(firsthyphen).toInt(&flag2);

    if (flag2) {
        int day = name.mid(firsthyphen + 1, 2).toInt(&flag2);
        if (flag2) {
            int year = name.mid(secondhyphen + 1, 4).toInt(&flag2);
            if (flag2) {
                datetime.setDate(QDate(year,month,day));

            }
        }
    }

    if (datetime.date().isValid() && datetime.time().isValid()) {
        return datetime;
    }
    return datetime;
}

QFileInfo StartupDebug::GetLastLogFile(QFileInfoList list) {
    int size = static_cast<int>(list.size()) - 1;
    list.removeAt(0);
    list.removeAt(0);
    list.removeOne(m_logger_ptr->GetLogFilePath().path());

    if (size == 0) {
        auto tmp = ParseLogName(list.first().fileName());

        if (tmp.isNull()) {
            m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, QString("This (%1.txt) file was renamed. Please don't rename the log files").arg(list.first().fileName()));
        }
        return list.first();
    }
    else if (size == -1 ) {
        m_logger_ptr->WriteToLog(Logger::LogFlag::Info,"No previous log file found");
        return QFileInfo();
    }

    for (int i = 0; i < size; i++) {
        auto first = ParseLogName(list.first().fileName());
        auto last = ParseLogName(list.last().fileName());

        if (first.isValid() && last.isValid()) {
            if (first.date().year() < last.date().year()) {
                list.removeOne(list.first());
            }
            else if (first.date().year() > last.date().year()) {
                list.removeOne(list.last());
            }
            else {
                if (first.date().month() < last.date().month()) {
                    list.removeOne(list.first());
                }
                else if (first.date().month() > last.date().month()) {
                    list.removeOne(list.last());
                }
                else {
                    if (first.date().day() < last.date().day()) {
                        list.removeOne(list.first());
                    }
                    else if (first.date().day() > last.date().day()) {
                        list.removeOne(list.last());
                    }
                    else {
                        if (first.time().hour() < last.time().hour()) {
                            list.removeOne(list.first());
                        }
                        else if (first.time().hour() > last.time().hour()) {
                            list.removeOne(list.last());
                        }
                        else {
                            if (first.time().minute() < last.time().minute()) {
                                list.removeOne(list.first());
                            }
                            else if (first.time().minute() > last.time().minute()) {
                                list.removeOne(list.last());
                            }
                            else {
                                if (first.time().second() < last.time().second()) {
                                    list.removeOne(list.first());
                                }
                                else if (first.time().second() > last.time().second()) {
                                    list.removeOne(list.last());
                                }
                                else {
                                    if (list.size() == 1) {
                                        return list.first();
                                    }
                                    else {
                                        m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, "Exception: GetLastLogFile()");
                                        break;
                                    }
                                }
                            }
                        }
                    }
               }
            }
        }
        else {
            if (first.isNull()){
                m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, QString("This (%1.txt) file was renamed. Please don't rename the log files").arg(list.first().fileName()));
                list.removeOne(list.first());
            }
            else if (last.isNull()) {
                m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, QString("This (%1.txt) file was renamed. Please don't rename the log files").arg(list.last().fileName()));
                list.removeOne(list.last());
            }
        }
    }

    m_logger_ptr->WriteToLog(Logger::Warning, "list.size() was larger than one after parsing: See StartupDebug::FindMostRecentCreatedFile()");
    return QFileInfo();
}

QFileInfo StartupDebug::GetOldestLogFile(QFileInfoList list) {
    int size = static_cast<int>(list.size()) - 1;

    list.removeAt(0);
    list.removeAt(0);
    list.removeOne(m_logger_ptr->GetLogFilePath().path());

    if (size == 0) {
        auto tmp = ParseLogName(list.first().fileName());
        if (tmp.date().isNull() || tmp.time().isNull()) {
            m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, QString("This (%1.txt) file was renamed. Please don't rename the log files").arg(list.first().fileName()));
        }
        return list.first();
    }
    else if (size == -1 ) {
        return QFileInfo();
    }

    for (int i = 0; i < size; i++) {
        auto first = ParseLogName(list.first().fileName());
        auto last = ParseLogName(list.last().fileName());

        if (first.isValid() && last.isValid()) {
            if (first.date().year() > last.date().year()) {
                list.removeOne(list.first());
            }
            else if (first.date().year() < last.date().year()) {
                list.removeOne(list.last());
            }
            else {
                if (first.date().month() > last.date().month()) {
                    list.removeOne(list.first());
                }
                else if (first.date().month() < last.date().month()) {
                    list.removeOne(list.last());
                }
                else {
                    if (first.date().day() > last.date().day()) {
                        list.removeOne(list.first());
                    }
                    else if (first.date().day() < last.date().day()) {
                        list.removeOne(list.last());
                    }
                    else {
                        if (first.time().hour() > last.time().hour()) {
                            list.removeOne(list.first());
                        }
                        else if (first.time().hour() < last.time().hour()) {
                            list.removeOne(list.last());
                        }
                        else {
                            if (first.time().minute() > last.time().minute()) {
                                list.removeOne(list.first());
                            }
                            else if (first.time().minute() < last.time().minute()) {
                                list.removeOne(list.last());
                            }
                            else {
                                if (first.time().second() > last.time().second()) {
                                    list.removeOne(list.first());
                                }
                                else if (first.time().second() < last.time().second()) {
                                    list.removeOne(list.last());
                                }
                                else {
                                    m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, "Exception: GetOldestLogFile()");
                                    break;
                                }
                            }
                        }
                    }
               }
            }
        }
        else {
            if (first.isNull()){
                m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, QString("This (%1.txt) file was renamed. Please don't rename the log files").arg(list.first().fileName()));
                list.removeOne(list.first());
            }
            else if (last.isNull()) {
                m_logger_ptr->WriteToLog(Logger::LogFlag::Warning, QString("This (%1.txt) file was renamed. Please don't rename the log files").arg(list.last().fileName()));
                list.removeOne(list.last());
            }
        }
    }

    if (list.size() == 1) {
        return list.first();
    }

    m_logger_ptr->WriteToLog(Logger::Warning, "list.size() was larger than one after parsing: See StartupDebug::GetOldestLogFile()");
    return QFileInfo();
}

}//end namespace StartupDebug
