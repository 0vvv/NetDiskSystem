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
}
