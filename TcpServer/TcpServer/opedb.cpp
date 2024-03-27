#include "opedb.h"
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("E:\\Project\\NetDiskSystem\\TcpServer\\TcpServer\\cloud.db");
    if(m_db.open()){
        QSqlQuery query;
        query.exec("select * from userInfo");
        while(query.next()){
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }else{
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

OpeDB::~OpeDB()
{
    m_db.close();
}

bool OpeDB::handleRegist(const char *name,  const char *pwd)
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    QString data = QString("insert into userInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    // qDebug() << data;
    QSqlQuery query;
    // exec返回的是bool
    return query.exec(data);
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if(name == NULL || pwd == NULL){
        qDebug()<<"name or pwd is NULL";
        return false;
    }
    // online=0表示目前没登录
    QString data = QString("select * from userInfo where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    // query.next()可依次获取query执行的每一条结果
    if(query.next()){
        // 把online更新为1，表示已登录
        data = QString("update userInfo set online=1 where name=\'%1\' and pwd=\'%2\'").arg(name).arg(pwd);
        // qDebug() << data;
        QSqlQuery query;
        query.exec(data);
        return true;
    }else{
        return false;
    }
}

void OpeDB::handleOffline(const char *name)
{
    if(name == NULL){
        qDebug()<<"name is NULL";
        return;
    }
    QString data = QString("update userInfo set online=0 where name=\'%1\'").arg(name);
    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
}

QStringList OpeDB::handleAllOnline()
{
    QString data = QString("select name from userInfo where online=1");
    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();
    while(query.next()){
        result.append(query.value(0).toString());
    }
    return result;
}

int OpeDB::handleSearchUser(const char *name)
{
    if(NULL == name){
        return -1;
    }
    QString data = QString("select online from userInfo where name=\'%1\'").arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
       int res = query.value(0).toInt();
       if(res==1){
           // 在线返回1
            return 1;
       }else{
           // 不在线返回0
            return 0;
       }
    }else{
        // 不存在返回-1
        return -1;
    }
}

int OpeDB::handleAddFriend(const char *pername, const char *name)
{
    if(NULL==pername || NULL==name){
        return -1;
    }
    // pername和name是否已经是好友
    QString data = QString("select * from friendInfo where(id=(select id from userInfo where name=\'%1\') and friendId=(select id from userInfo where name=\'%2\')) "
                           "or (id=(select id from userInfo where name=\'%3\') and friendId=(select id from userInfo where name=\'%4\'))")
                       .arg(name).arg(pername).arg(pername).arg(name);
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        // 已经是好友返回0
        return 0;
    }else{
        data = QString("select online from userInfo where name=\'%1\'").arg(pername);
        query.exec(data);
        if(query.next()){
            int res = query.value(0).toInt();
            if(res==1){
                // 用户在线返回1
                return 1;
            }else{
                // 用户不在线返回2
                return 2;
            }
        }else{
            // 不存在这个用户返回-1
            return 3;
        }
    }
}

QStringList OpeDB::handleFlushFriend(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear();
    if(name == NULL){
        return strFriendList;
    }
    QString data = QString("select id from userInfo where online=1 and id =(select id from friendInfo where friendId=(select id from userInfo where name=\'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(data);
    while(query.next()){
        strFriendList.append(query.value(0).toString());
    }
    data = QString("select id from userInfo where online=1 and id =(select friendId from friendInfo where id=(select id from userInfo where name=\'%1\'))").arg(name);
    query.exec(data);
    while(query.next()){
        strFriendList.append(query.value(0).toString());
    }
    return strFriendList;
}
