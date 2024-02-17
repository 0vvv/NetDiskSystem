#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QFile>
#include <QTcpSocket> // pro文件的QT+=network后才可以使用

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QMainWindow
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    // 加载配置资源
    void loadConfig();
    // 槽函数：与信号（signal）关联的普通C++成员函数。
    // 信号和槽函数的结合使得对象能够进行解耦的通信
public slots:
    void showConnect();

private:
    Ui::TcpClient *ui;
    // IP字符串和port端口号
    QString m_strIP;
    quint16 m_usPort;
    // QTcpsocket用于连接服务器，与服务器数据交互
    QTcpSocket m_tcpSocket;
};
#endif // TCPCLIENT_H
