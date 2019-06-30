#include <QDesktopWidget>
#include <QButtonGroup>
#include "crashwindow.h"
#include "ui_crashwindow.h"

CrashWindow::CrashWindow(std::shared_ptr<Logger::Logger> ptr, QFileInfo log, QWidget *parent) :
    QDialog(parent),
    m_logger_ptr(ptr),
    m_Log(log),
    ui(new Ui::CrashWindow)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(QDialogButtonBox::accepted), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(QDialogButtonBox::rejected), this, SLOT(reject()));

    QFile file(log.filePath());
    QTextStream stream(&file);

    if (file.open(QIODevice::ReadOnly)){
        while (!stream.atEnd()) {
            ui->textBrowser->append(stream.readLine());
        }
    }
}

CrashWindow::~CrashWindow()
{
    delete ui;
}

void CrashWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Are you sure?", "Are you sure you want to close the application?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No){
        event->ignore();
    }
    else {
        accept();
    }
}
