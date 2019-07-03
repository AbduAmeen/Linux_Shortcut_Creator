#include "messagenode.h"
#include "ui_messagenode.h"

MessageNode::MessageNode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageNode)
{
    ui->setupUi(this);
}

MessageNode::~MessageNode()
{
    delete ui;
}
