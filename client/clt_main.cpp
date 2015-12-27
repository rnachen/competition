/// @file       clt_main.cpp
/// @date       2013/12/06 21:29
///
/// @author
///
/// @brief      客户端主函数入口
///

#include <stdio.h>
#include <errno.h>
#include <vector>
#include "comm_string_util.h"
#include "comm_log.h"
#include "comm_socket.h"
#include "clt_game.h"

void
usage(const char *bin_path)
{
    assert(bin_path != NULL);
    debug_log("usage:%s svr_ip port", bin_path);
}

int
main(int argc, const char **argv)
{
    if (argc != 3)
    {
        usage(argv[0]);
        return -1;
    }

    CommLog::init_log("./log/client.log", LOG_INFO);

    CommSocket client_socket;
    client_socket.create(CommSocket::TCP);
    if (client_socket.connect(argv[1], atoi(argv[2])) != 0)
    {
        debug_log("connect to %s:%s fail. errno=%s",
            argv[1], argv[2], strerror(errno));
        return -1;
    }

    info_log("connect succ. ");

    bool run = true;
    int ret = 0;
    while (run)
    {
		ret = do_game(client_socket);
		if (ret != 0)
		{
			error_log("game fail. exit");
			break;
		}
    }

    return 0;
}

