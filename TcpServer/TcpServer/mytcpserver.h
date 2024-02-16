#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
// MyTcpServer继承了QTcpServer
class MyTcpServer : public QTcpServer
{
    Q_OBJECT  // 支持信号传输
public:
    MyTcpServer();
    // 单例模式
    static MyTcpServer &getInstance();
    void incomingConnection(qintptr socketDescription);
};

#endif // MYTCPSERVER_H
