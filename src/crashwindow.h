#ifndef CRASHWINDOW_H
#define CRASHWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>

namespace Ui {
class CrashWindow;
}

class CrashWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CrashWindow(QWidget *parent = nullptr);
    ~CrashWindow() override;

private:
    void closeEvent(QCloseEvent *event) override;
    Ui::CrashWindow *ui;
};

#endif // CRASHWINDOW_H
