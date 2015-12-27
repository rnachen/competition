/// @file       svr_accept_handler.h
/// @date       2013/12/09 9:37
///
/// @author
///
/// @brief      侦听处理
///

#ifndef SVR_ACCEPT_HANDLER_H_
#define SVR_ACCEPT_HANDLER_H_

#include <map>
#include "comm_event_handler.h"
#include "comm_socket.h"

class SvrAcceptHandler : public CommEventHandler
{

public:
    SvrAcceptHandler();
    ~SvrAcceptHandler();

    int start_service(const char *ip, unsigned short port);

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
    CommSocket socket_;
};

#endif // SVR_ACCEPT_HANDLER_H_
