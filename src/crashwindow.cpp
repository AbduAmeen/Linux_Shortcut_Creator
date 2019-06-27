#include <QDesktopWidget>

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

void CrashWindow::closeEvent(QCloseEvent *event){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Are you sure?", "Are you sure you want to close the application?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No){
        event->ignore();
    }
}
