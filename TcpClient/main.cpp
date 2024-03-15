#include "tcpclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 单例模式：只有一个tcpclient的instance
    TcpClient::getInstance().show();

    return a.exec();
}
