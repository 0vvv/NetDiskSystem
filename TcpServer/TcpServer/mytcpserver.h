#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
// MyTcpServer继承了QTcpServer
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
};

#endif // MYTCPSERVER_H
