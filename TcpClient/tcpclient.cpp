#include "tcpclient.h"
#include "./ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpClient::TcpClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    // 信号发送方，发送信号，信号接收方，每次用哪个函数处理
    connect(&m_tcpSocket, SIGNAL(connected()),
            this, SLOT(showConnect()));

    // 连接服务器，默认是读写模式
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    // 优化点：用JSON解析配置文件会更快
    // 文件前缀/文件名称
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        //qDebug() << strData;  //打印出读取到的内容，显示出："127.0.0.1\r\n8888"
        file.close();
        strData.replace("\r\n"," ");
        //qDebug() << strData;  //打印出把\r\n替换后的结果："127.0.0.1 8888"
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "ip:"<< m_strIP << "port:" << m_usPort;
    }else{
        QMessageBox::critical(this,"open config","open config failed");
    }
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "连接服务器","连接服务器成功");
}