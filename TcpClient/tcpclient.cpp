#include "tcpclient.h"
#include "./ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

// TcpClient继承QWidget，不是QMainWindow，不然不能给最外层加layout
TcpClient::TcpClient(QWidget *parent): QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    resize(500,200);
    loadConfig();
    // 发送信号，参数分别是：信号发送方，发送信号，信号接收方，每次用哪个函数处理
    connect(&m_tcpSocket, SIGNAL(connected()),
                this, SLOT(showConnect()));
    connect(&m_tcpSocket, SIGNAL(readyRead()),
                this, SLOT(recvMsg()));
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
        // 第二个参数是title，第三个参数是text
        QMessageBox::critical(this,"open config","open config failed");
    }
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return m_strLoginName;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    // 和server端一样的处理过程，不过是由socket来接受和处理消息
    // 1. 收数据
    // bytesAvailable当前可读的数据大小
    qDebug()<< m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU * pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));

    // 2. 判断数据类型并处理
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        if(strcmp(pdu->caData, REGIST_OK) == 0){
            QMessageBox::information(this,"注册",REGIST_OK);
        }else if(strcmp(pdu->caData, REGIST_FAILED)==0){
            QMessageBox::warning(this,"注册",REGIST_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
    {
        if(strcmp(pdu->caData, LOGIN_OK) == 0){
            QMessageBox::information(this,"登录",LOGIN_OK);
            // 跳转到登录后的操作界面
            OpeWidget::getInstance().show();
            // 隐藏登录界面
            this->hide();
        }else if(strcmp(pdu->caData, LOGIN_FAILED)==0){
            QMessageBox::warning(this,"登录",LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUser(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
    {
        QString tips;
        QString name = OpeWidget::getInstance().getFriend()->m_strSearchName;
        if(strcmp(pdu->caData,SEARCH_USER_NO)==0){
            tips = QString("%1 : not exist").arg(name);
        }else if(strcmp(pdu->caData,SEARCH_USER_ONLINE)==0){
            tips = QString("%1 : online").arg(name);
        }else{
            tips = QString("%1 : offline").arg(name);
        }
        QMessageBox::information(this, "搜索",tips);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this, "添加好友", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName, pdu->caData+32, 32);
        int res = QMessageBox::information(this,"添加好友", QString("%1 want to add you as friend?").arg(caName),
                                 QMessageBox::Yes, QMessageBox::No);
        PDU* respdu=mkPDU(0);
        memcpy(respdu->caData, pdu->caData,32);
        if(QMessageBox::Yes==res){
            // 同意加好友
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }else{
            // 不同意加好友
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REJECT;
        }
        m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
        char caName[32]={'\0'};
        memcpy(caName, pdu->caData,32);
        QMessageBox::information(this, "删除好友", QString("%1 删除了你的好友").arg(caName));
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:{
        QMessageBox::information(this, "删除好友", "删除好友成功");
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu = NULL;
}
#if 0
void TcpClient::on_send_pb_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if(!strMsg.isEmpty()){
        PDU *pdu = mkPDU(strMsg.size());
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(),strMsg.size());
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::warning(this,"信息发送","发送的信息不能为空");
    }
}
#endif

void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();

    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty()){
        m_strLoginName = strName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData+32, strPwd.toStdString().c_str(), 32);
        // 把用户名和密码通过socket发送给服务器
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::critical(this,"登录","登录失败，用户名或密码为空");
    }
}


void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty()){
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData+32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::critical(this,"注册","注册失败，用户名或密码为空");
    }
}


void TcpClient::on_cancel_pb_clicked()
{

}

