#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
// 同时处理多个socket的消息
#include <QTcpSocket>
#include <protocol.h>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();

public slots:
    void recvMsg();
};

#endif // MYTCPSOCKET_H
