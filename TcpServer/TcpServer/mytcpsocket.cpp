#include "mytcpsocket.h"
#include <QDebug>

// 哪个收到信号就哪个socket处理该信号
MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()),
            this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected()),
            this, SLOT(clientOffline()));
}

QString MyTcpSocket::getName()
{
    return m_strName;
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
        // 注册请求
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
        // 登录请求
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        char caName[32] = {'\0'};
        char caPwd[32] = {'\0'};
        strncpy(caName, pdu->caData, 32);
        strncpy(caPwd, pdu->caData+32, 32);
        bool  ret = OpeDB::getInstance().handleLogin(caName, caPwd);
        PDU * resPdu = mkPDU(0);
        resPdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
        if(ret){
            strcpy(resPdu->caData, LOGIN_OK);
            // 记录socket的名字
            m_strName = caName;
        }else{
            strcpy(resPdu->caData, LOGIN_FAILED);
        }
        write((char*)resPdu, resPdu->uiPDULen);
        free(resPdu);
        resPdu = NULL;
        break;
    }
        // 显示所有在线用户
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        QStringList res=OpeDB::getInstance().handleAllOnline();
        // 消息长度
        uint uiMsgLen = res.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        for(int i=0;i<res.size();i++){
            // 先转成(char*)
            memcpy((char*)(respdu->caMsg)+32*i,
                   res.at(i).toStdString().c_str(),
                   res.at(i).size());
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}

void MyTcpSocket::clientOffline()
{
    // 修改数据库中的online字段
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());
    // 删除对应的socket
    emit offline(this);

}
