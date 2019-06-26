#include "crashwindow.h"
#include "ui_crashwindow.h"

CrashWindow::CrashWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CrashWindow)
{
    ui->setupUi(this);
}

CrashWindow::~CrashWindow()
{
    delete ui;
}
