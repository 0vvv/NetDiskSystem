#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    // 监听信号
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    // 优化点：用JSON解析配置文件会更快
    // 文件前缀/文件名称
    QFile file(":/server.config");
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
