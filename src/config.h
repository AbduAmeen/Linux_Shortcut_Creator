#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QObject>
#include <memory>

#include "logger.h"
class Config : public QSettings
{
    Q_OBJECT
public:
    Config(QString configname,QSettings::Format scope, std::shared_ptr<Logger::Logger> ptr, QObject* parent = nullptr);
    void CheckAndLogStatus();
private:
    std::shared_ptr<Logger::Logger> m_logger_ptr;
};

#endif // CONFIG_H
