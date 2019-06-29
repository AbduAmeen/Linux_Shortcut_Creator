#ifndef CRASHWINDOW_H
#define CRASHWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <memory>
#include <QFileInfo>

#include "logger.h"

namespace Ui {
class CrashWindow;
}

class CrashWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CrashWindow(std::shared_ptr<Logger::Logger> ptr, QFileInfo log, QWidget *parent = nullptr);
    ~CrashWindow() override;

private:
    void closeEvent(QCloseEvent *event) override;
    std::shared_ptr<Logger::Logger> m_logger_ptr;
    QFileInfo m_Log;
    Ui::CrashWindow *ui;
};

#endif // CRASHWINDOW_H
