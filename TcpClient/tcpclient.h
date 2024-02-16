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
    void loadConfig();  // 加载配置资源
    // 槽函数（？
public slots:
    void showConnect();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;
    // 连接服务器，与服务器数据交互
    QTcpSocket m_tcpSocket;
};
#endif // TCPCLIENT_H
