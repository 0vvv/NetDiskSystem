#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

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
    // ui->online_lw->clear();
    for(int i=0;i<uiSize;i++){
        memcpy(caTmp, (char*)(pdu->caMsg)+i*32, 32);
        ui->online_lw->addItem(caTmp);
    }
}

void Online::on_addFriend_pb_clicked()
{
    QListWidgetItem *pItem = ui->online_lw->currentItem();
    // 要加的好友名字以及自己的名字
    QString strPerUserName = pItem->text();
    QString strLoginName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, strPerUserName.toStdString().c_str(), strPerUserName.size());
    memcpy(pdu->caData+32, strLoginName.toStdString().c_str(), strLoginName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

