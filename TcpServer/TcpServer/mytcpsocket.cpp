#include "mytcpsocket.h"
#include <QDebug>
#include "tcpserver.h"

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
        // 搜索用户
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
    {
        int res = OpeDB::getInstance().handleSearchUser(pdu->caData);
        PDU* respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(res==1){
            strcpy(respdu->caData,SEARCH_USER_ONLINE);
        }else if(res==0){
            strcpy(respdu->caData,SEARCH_USER_OFFLINE);
        }else{
            strcpy(respdu->caData,SEARCH_USER_NO);
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
        // 加好友请求
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char pername[32] = {'\0'};
        char name[32] = {'\0'};
        strncpy(pername, pdu->caData, 32);
        strncpy(name, pdu->caData+32, 32);
        int res = OpeDB::getInstance().handleAddFriend(pername,name);
        PDU* respdu=mkPDU(0);
        if(res==-1){
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,UNKNOWN_ERROR);
            write((char*)respdu, respdu->uiPDULen);
        }else if(res==0){
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,EXISTED_FRIEND);
            write((char*)respdu, respdu->uiPDULen);
        }else if(res==1){
            // 转发添加好友请求给对方
            MyTcpServer::getInstance().resend(pername, pdu);
        }else if(res==2){
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
            write((char*)respdu, respdu->uiPDULen);
        }else{
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_NOEXIST);
            write((char*)respdu, respdu->uiPDULen);
        }
        free(respdu);
        respdu = NULL;
        break;
    }
        // 刷新好友请求
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
    {
        char name[32] = {'\0'};
        strncpy(name, pdu->caData, 32);
        QStringList res = OpeDB::getInstance().handleFlushFriend(name);
        uint uiMsgLen = res.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        for(int i=0;i<res.size();i++){
            memcpy((char*)(respdu->caMsg)+i*32, res.at(i).toStdString().c_str(), res.at(i).size());
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
        // 删除好友请求
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        char name[32] = {'\0'}, friendName[32] = {'\0'};
        strncpy(name, pdu->caData, 32);
        strncpy(friendName, pdu->caData+32, 32);
        bool res = OpeDB::getInstance().handleDelFriend(name, friendName);
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        strcpy(respdu->caData, DEL_FRIEND_OK);
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        // A删除B，把这个消息转发通知给B
        MyTcpServer::getInstance().resend(friendName, pdu);
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
