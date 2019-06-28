#ifndef THEAPP_H
#define THEAPP_H

#include <QObject>
#include <QApplication>
#include <memory>

#include "logger.h"

class TheApp : public QApplication {
    Q_OBJECT
public:
    TheApp(int &argc, char **argv, std::shared_ptr<Logger::Logger> ptr);
    void Setlogger(std::shared_ptr<Logger::Logger> ptr);
private:
    std::shared_ptr<Logger::Logger> m_logger_ptr;
};

#endif // THEAPP_H
