#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;
#define REGIST_OK "register ok"
#define REGIST_FAILED "register failed: name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed: name error or pwd error or relogin"
#define SEARCH_USER_NO "no such people"
#define SEARCH_USER_ONLINE "online"
#define SEARCH_USER_OFFLINE "offline"
#define UNKNOWN_ERROR "unknown error"
#define EXISTED_FRIEND "friend exist"
#define ADD_FRIEND_OFFLINE "user offline"
#define ADD_FRIEND_NOEXIST "user not exist"
#define DEL_FRIEND_OK "delete friend ok"

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,   // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   // 注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,    // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,    // 登录回复
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST, // 所有在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND, // 所有在线用户回复
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST, // 搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND, // 搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND, // 添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,     // 同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REJECT, // 拒绝添加好友
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,     // 刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND, // 刷新好友回复
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,     // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, // 删除好友回复
    ENUM_MSG_TYPE_MAX=0x00ffffff,
};

struct PDU
{
    uint uiPDULen;      // 总的协议数据单元大小
    uint uiMsgType;     // 消息类型
    char caData[64];    // 文件名
    uint uiMsgLen;     // 实际消息长度【变长部分】
    int caMsg[];        // 实际消息
};

PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
