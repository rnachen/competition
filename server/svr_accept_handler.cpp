/// @file       svr_accept_handler.cpp
/// @date       2013/12/09 9:37
///
/// @author
///
/// @brief      gamesvr 网络处理模块
///

#include "svr_accept_handler.h"
#include <errno.h>
#include <assert.h>
#include <map>
#include "comm_log.h"
#include "comm_refactor.h"
#include "svr_tcp_client_handler.h"

SvrAcceptHandler::SvrAcceptHandler()
{
}

SvrAcceptHandler::~SvrAcceptHandler()
{
}

int SvrAcceptHandler::start_service(const char *ip, unsigned short port)
{
    socket_.create(CommSocket::TCP);

    int ret = socket_.listen(ip, port);
    if (ret != 0)
    {
        error_log("start %s:%d fail. ret=%d", ip, port, ret);
        return -2;
    }

    info_log("start %s:%d succ.", ip, port);
    return 0;
}

int
SvrAcceptHandler::get_handle(void) const
{
    return socket_.get_fd();
}

int
SvrAcceptHandler::handle_input()
{
    CommSocket client_socket;
    int ret = socket_.accept(&client_socket);
    if (ret != 0)
    {
        error_log("accept fail. ret=%d", ret);
        return -1;
    }

    SvrTcpClientHandler *client_handler = new SvrTcpClientHandler(client_socket.get_fd());

    ret = CommRefactor::instance()->regist(client_handler,
        CommEventHandler::READ_MASK|CommEventHandler::EXCEPT_MASK);
    if (ret != 0)
    {
        error_log("new client add fail. ");
        return 0;
    }

    debug_log("accept new client: fd= %d", client_socket.get_fd());

    // 要设置到-1，否则client_socket析构会关闭fd
    client_socket.set_fd(-1);

    return 0;
}

int
SvrAcceptHandler::handle_exception()
{
    error_log("accept handler recv exception.");
    return 0;
}

int
SvrAcceptHandler::handle_close()
{
    error_log("accept handler is closed");
    return 0;
}

