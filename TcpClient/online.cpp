#include "online.h"
#include "ui_online.h"

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUser(PDU *pdu)
{
    if(pdu==NULL){
        return ;
    }
    // 用户个数
    uint uiSize = pdu->uiMsgLen/32;
    char caTmp[32];
    ui->online_lw->clear();
    for(int i=0;i<uiSize;i++){
        memcpy(caTmp, (char*)(pdu->caMsg)+i*32, 32);
        ui->online_lw->addItem(caTmp);
    }
}
