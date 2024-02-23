#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
// 同时处理多个socket的消息
#include <QTcpSocket>
#include <protocol.h>
#include <opedb.h>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
signals:
    void offline(MyTcpSocket *mySocket);

public slots:
    void recvMsg();
    void clientOffline();

private:
    QString m_strName;
};

#endif // MYTCPSOCKET_H
