#include "tcpclient.h"

#include <QApplication>
#include "opewidget.h"
// #include "friend.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // TcpClient w;
    // w.show();
    OpeWidget w;
    w.show();
    // Friend w;
    // w.show();
    return a.exec();
}
