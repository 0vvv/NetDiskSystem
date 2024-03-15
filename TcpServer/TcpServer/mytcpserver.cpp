#include "mytcpserver.h"
#include <QDebug>
MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}
// 接受连接的时候自动调用incomingConnection
void MyTcpServer::incomingConnection(qintptr socketDescripor)
{
    qDebug()<<"new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescripor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)),
            this, SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::resend(const char *pername, PDU *pdu)
{
    if(NULL==pername || NULL==pdu){
        return;
    }
    QString strName=pername;
    for(int i=0;i<m_tcpSocketList.size();i++){
        if(strName==m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write((char*)pdu, pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mySocket)
{
    QList<MyTcpSocket*>::Iterator iter = m_tcpSocketList.begin();
    for(;iter!=m_tcpSocketList.end();iter++){
        if(mySocket == *iter){
            delete *iter;
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }
    for(int i=0;i<m_tcpSocketList.size();i++){
        qDebug()<<m_tcpSocketList.at(i)->getName();
    }
}
