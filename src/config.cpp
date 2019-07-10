#include "config.h"

Config::Config(QString configname,QSettings::Format scope, std::shared_ptr<Logger::Logger> ptr, QObject* parent) : QSettings(configname, scope, parent), m_logger_ptr(ptr)
{

}

void Config::CheckAndLogStatus() {
    switch (status()){
    case Config::Status::NoError:
        m_logger_ptr->WriteToLog(Logger::Info,"No problems with config");
        break;
    case Config::Status::AccessError:
        m_logger_ptr->WriteToLog(Logger::Warning,"AccessError with config");
        break;
    case::Config::Status::FormatError:
        m_logger_ptr->WriteToLog(Logger::Warning,"FormatError with config");
        break;
    }

}
