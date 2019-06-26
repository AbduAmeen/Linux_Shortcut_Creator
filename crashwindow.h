#ifndef CRASHWINDOW_H
#define CRASHWINDOW_H

#include <QDialog>

namespace Ui {
class CrashWindow;
}

class CrashWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CrashWindow(QWidget *parent = 0);
    ~CrashWindow();

private:
    Ui::CrashWindow *ui;
};

#endif // CRASHWINDOW_H
