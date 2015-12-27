/// @file       svr_tcp_client_handler.h
/// @date       2013/12/05 9:56
///
/// @author
///
/// @brief      tcp客户端逻辑处理
///

#ifndef SVR_TCP_CLIENT_HANDLER_H_
#define SVR_TCP_CLIENT_HANDLER_H_

#include "comm_event_handler.h"
#include "comm_socket.h"
#include "svr_proto.h"

class SvrTcpClientHandler : public CommEventHandler
{
public:
    SvrTcpClientHandler();
    SvrTcpClientHandler(int fd);

    int init(int fd);

    /*!
    @brief      取回对应的ZEN_SOCKET 句柄
    @return     ZEN_SOCKET CommEventHandler 对应的ZEN_SOCKET 句柄
    */
    virtual int get_handle(void) const;

    /*!
    @brief      读取事件触发调用函数，用于读取数据，accept成功，（connect失败）处理，
    @return     int 返回0表示句柄处理正常，return -1后，反应器会主动handle_close，帮助结束句柄
    */
    virtual int handle_input();

    /*!
    @brief      写入事件触发调用函数，用于写数据，connect成功，
    @return     int int 返回0表示句柄处理正常，return -1后，反应器会主动handle_close，帮助结束句柄
    */
    virtual int handle_output();

    /*!
    @brief      调用异常，return -1表示调用handle_close
    @return     int
    */
    virtual int handle_exception();

    /*!
    @brief      句柄关闭处理函数，基类函数调用了remove
    @return     int
    */
    virtual int handle_close();

private:

    // 处理上行包
    int handle_recv_pkg(const svr_proto::SvrPkg &pkg);
    int handle_player_login(const svr_proto::PlayerLogin &pkg_body);
    int handle_player_get_axis(const svr_proto::PlayerGetAxis &pkg_body);

    int send_pkg(const svr_proto::SvrPkg &send_pkg);
private:
    // 客户端连接对象
    CommSocket socket_;
    // 最大包长
    static const unsigned int MAX_PKG_LEN = 64*1024;
    char recv_buffer_[MAX_PKG_LEN];
    char send_buffer_[MAX_PKG_LEN];

    svr_proto::SvrPkg recv_pkg_;
    svr_proto::SvrPkg send_pkg_;

};

#endif // SVR_TCP_CLIENT_HANDLER_H_
