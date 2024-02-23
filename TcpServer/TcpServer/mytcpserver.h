#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"

// MyTcpServer继承了QTcpServer且包含 Q_OBJECT
class MyTcpServer : public QTcpServer
{
    // 支持信号传输
    Q_OBJECT
public:
    MyTcpServer();
    // 单例模式
    static MyTcpServer &getInstance();
    // incomingConnection是重写了QTcpServer里的一个函数
    void incomingConnection(qintptr socketDescription);

public slots:
    void deleteSocket(MyTcpSocket *mySocket);

private:
    QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
