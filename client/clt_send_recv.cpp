/// @file       clt_send_recv.h
/// @date       2013/12/06 21:29
///
/// @author
///
/// @brief      客户端收发包
///

#include "clt_send_recv.h"
#include <sys/types.h>
#include <sys/stat.h>
#if defined(WIN32) || defined(WIN64)
#else
#include <unistd.h>
#endif
#include "comm_log.h"

unsigned long long
get_file_size(const char *file_path)
{
    struct stat st;
    if (stat(file_path, &st) != 0)
    {
        error_log("can't open %s to upload.", file_path);
        return -1;
    }

    return st.st_size;
}

static const unsigned int MAX_PKG_LEN = 64*1024;

int
send_pkg(CommSocket &client_socket, const svr_proto::SvrPkg &pkg)
{
    char buffer[MAX_PKG_LEN];
    size_t use_len = 0;
    int ret = pkg.pack(buffer, sizeof(buffer), &use_len);
    if (ret != 0)
    {
        error_log("send pkg fail, pack fail. ret=%d", ret);
        return -1;
    }

    ret = client_socket.send((unsigned char *)buffer, use_len);
    if (ret == CommSocket::ERR_SOCKET_FAIL)
    {
        error_log("upload send fail. ret=%d", ret);
        return -2;
    }

    if (ret != CommSocket::ERR_SOCKET_FINISH)
    {
        error_log("send fail. ret=%d", ret);
        return -3;
    }

    // 发送成功
    return 0;
}

int
read_pkg(CommSocket &client_socket, svr_proto::SvrPkg &pkg)
{
    int ret = 0;
    unsigned char *buffer;
    unsigned int pkg_len = 64 * 1024;
    if (client_socket.get_pkg(&buffer, &pkg_len) == 0)
    {
        ret = pkg.unpack((char *)buffer, pkg_len);
        if (ret != 0)
        {
            error_log("unpack fail. ret=%d, pkg_len=%u", ret, pkg_len);
            return -1;
        }

        // 读到一个包，并且解出来了
        return 0;
    }

    do
    {
        bool wait_read = true;
        ret = client_socket.wait(&wait_read, NULL, 100000000);
        if (ret == CommSocket::ERR_SOCKET_TIMEOUT)
        {
            // 超时
            error_log("read timeout");
            return -1;
        }

        ret = client_socket.recv();
        if (ret == CommSocket::ERR_SOCKET_FAIL)
        {
            error_log("read fail. ret=%d", ret);
            return -2;
        }

        pkg_len = 64 * 1024;
        if (client_socket.get_pkg(&buffer, &pkg_len) == 0)
        {
            ret = pkg.unpack((char *)buffer, pkg_len);
            if (ret != 0)
            {
                error_log("unpack fail. ret=%d", ret);
                return -3;
            }

            // 读到一个包，并且解出来了
            return 0;
        }
        // 还未读取到一个完整包

    } while(ret != CommSocket::ERR_SOCKET_FAIL);

    // 读取包失败
    error_log("read pkg error");
    return -4;
}

