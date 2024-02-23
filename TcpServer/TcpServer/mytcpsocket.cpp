#include "mytcpsocket.h"
#include <QDebug>

// 哪个收到信号就哪个socket处理该信号
MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()),
            this, SLOT(recvMsg()));
}

void MyTcpSocket::recvMsg()
{
    // 1. 收数据
    // bytesAvailable当前可读的数据大小
    qDebug()<< this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU * pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    // qDebug()<<pdu->uiMsgType<<(char*)pdu->caMsg;

    // 2. 判断数据类型并处理
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        strncpy(caPwd, pdu->caData+32, 32);
        // qDebug() << caName << caPwd << pdu->uiMsgType;
        bool ret = OpeDB::getInstance().handleRegist(caName, caPwd);
        PDU * resPdu = mkPDU(0);
        resPdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret){
            strcpy(resPdu->caData, REGIST_OK);
        }else{
            strcpy(resPdu->caData, REGIST_FAILED);
        }
        write((char*)resPdu, resPdu->uiPDULen);
        free(resPdu);
        resPdu = NULL;
        break;
    }
    default:
            break;
    }
    free(pdu);
    pdu = NULL;
}
