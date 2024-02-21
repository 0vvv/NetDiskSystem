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
    // bytesAvailable当前可读的数据大小
    qDebug()<< this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU * pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    qDebug()<<pdu->uiMsgType<<(char*)pdu->caMsg;
}
