/// @file       comm_socket.cpp
/// @date       2013/12/04 19:49
///
/// @author
///
/// @brief      网络类封装
///

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/resource.h>

#include "comm_socket.h"
#include "comm_log.h"
//#include "svr_proto.h"

CommSocket::CommSocket():
    recv_buf_len_(0),
    effective_recv_begin_(0),
    need_send_buf_len_(0),
    effective_send_begin_(0),
    event_(0),
    fd_(-1),
    pkg_len_offset_(0),
    pkg_len_size_(4)
{

#if defined(_WIN32) || defined(_WIN64)
    static bool is_socket_init = false;

    if (!is_socket_init)
    {
        WSADATA ws;
        WSAStartup(MAKEWORD(2, 2), &ws);
        is_socket_init = true;
    }

#endif

	struct rlimit rl;
	rl.rlim_cur = 1024;
	rl.rlim_max = 100000;

}

CommSocket::~CommSocket()
{
    close();
}

int CommSocket::create(SocketType type)
{
    if (type == TCP)
    {
        fd_ = (int)socket(AF_INET, SOCK_STREAM, 0);
    }

    return 0;
}

void CommSocket::close()
{
    if (is_valid())
    {
#if defined(WIN32) || defined(_WIN64)
        ::closesocket(fd_);
#else
        ::close(fd_);
#endif
    }
}

bool CommSocket::is_valid() const
{
    return (fd_ != -1);
}

int CommSocket::listen(const char *ip, unsigned short port)
{
    // 设置reuse
    int opt = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char *)(&opt), sizeof(opt));

    // 转换ip
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int ret = ::bind(fd_, (struct sockaddr *)&addr, sizeof(addr));

    if (ret != 0)
    {
        error_log("bind failed, %s:%d err = %s", ip, port, strerror(errno));
        return -1;
    }

    ret = ::listen(fd_, 1000);

    if (ret != 0)
    {
        error_log("listen failed, err = %s", strerror(errno));
        return -2;
    }

    return 0;
}

int CommSocket::accept(CommSocket *socket)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int fd = (int)::accept(fd_, (struct sockaddr *)&addr, &addr_len);

    if (fd < 0)
    {
        error_log("accept failed, err = %s", strerror(errno));
        return -1;
    }

    socket->set_fd(fd);
    return 0;

}

int CommSocket::get_pkg(unsigned char **game_pkg, unsigned int *game_pkg_len)
{
    if (recv_buf_len_ - effective_recv_begin_ < (pkg_len_offset_+pkg_len_size_))
    {
        // 未有完整包
        return -1;
    }

    // 取得包长
    unsigned int pkg_len = 0;
    if (pkg_len_size_ == sizeof(unsigned short))
    {
        unsigned short net_len = 0;
        memcpy(&net_len, recv_buffer_ + effective_recv_begin_, sizeof(net_len));
        pkg_len = ntohs(net_len);
    }
    else if (pkg_len_size_ == sizeof(unsigned int))
    {
        unsigned int net_len = 0;
        memcpy(&net_len, recv_buffer_ + effective_recv_begin_, sizeof(net_len));
        pkg_len = ntohl(net_len);
    }
    else
    {
        assert(0);
        debug_log("pkg len size is not support.\n");
        return -3;
    }

    if (recv_buf_len_ - effective_recv_begin_ >= pkg_len)
    {
        if (*game_pkg_len < pkg_len)
        {
            error_log("buffer is not enough, game_pkg_len = %u, pkg_len = %u",
                   *game_pkg_len,
                   pkg_len);

            return ERR_SOCKET_BUFFER_NOT_ENOUGH;
        }

        assert(recv_buf_len_ >= 0);
        //memcpy(game_pkg, recv_buffer_ + effective_begin_, pkg_len);
        *game_pkg = recv_buffer_ + effective_recv_begin_;
        *game_pkg_len = pkg_len;

        // 更新有效数据开始位置
        effective_recv_begin_ += pkg_len;

        if (recv_buf_len_ == effective_recv_begin_)
        {
            recv_buf_len_ = 0;
            effective_recv_begin_ = 0;
        }
        
        return ERR_SOCKET_FINISH;
    }

    // 还没有收集一个完整的包
    return ERR_SOCKET_PACKET_NOT_FINISH;

}

CommSocket::SocketResult CommSocket::send(const unsigned char *buffer, unsigned int buf_len)
{
    if (buffer != NULL && buf_len > 0)
    {
        if (sizeof(send_buffer_) - effective_send_begin_ - need_send_buf_len_ < buf_len)
        {
            error_log("send buffer is not enough, need_send_len = %u, buf_len = %u",
                   need_send_buf_len_,
                   buf_len);

            return ERR_SOCKET_FAIL;
        }

        memcpy(send_buffer_ + effective_send_begin_, buffer, buf_len);
        need_send_buf_len_ += buf_len;
    }

    int ret = ::send(fd_, (char *)send_buffer_ + effective_send_begin_, need_send_buf_len_, 0);
    /*info_log("send successfully, buf_len = %u, need_seed_buf_len = %u, ret = %d",
           buf_len,
           need_send_buf_len_,
           ret);
           */

    if (ret < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // 下次需要继续发送
            return ERR_SOCKET_CONTINUE;
        }

        return ERR_SOCKET_FAIL;
    }

    need_send_buf_len_ -= ret;

    if (need_send_buf_len_ > 0)
    {
        effective_send_begin_ += ret; 
        return ERR_SOCKET_CONTINUE;
    }
    else if (need_send_buf_len_ == 0)
    {
        effective_send_begin_ = 0;
        return ERR_SOCKET_FINISH;
    }
    else
    {
        assert(0);
    }

    return ERR_SOCKET_FINISH;
}

CommSocket::SocketResult CommSocket::recv()
{
    if (if_recv_buffer_will_mommove())
    {
        memmove(recv_buffer_, recv_buffer_ + effective_recv_begin_, recv_buf_len_ - effective_recv_begin_);
        recv_buf_len_ -= effective_recv_begin_;
        effective_recv_begin_ = 0;   
    }

    int ret = ::recv(fd_, (char *)recv_buffer_ + recv_buf_len_, sizeof(recv_buffer_) - recv_buf_len_, 0);
    if (ret <= 0)
    {
        if (ret == EAGAIN || ret == EWOULDBLOCK)
        {
            // 下次需要继续发送
            return ERR_SOCKET_CONTINUE;
        }

        error_log("recv error, ret=%d|%s", ret, strerror(errno));
        return ERR_SOCKET_FAIL;
    }

    recv_buf_len_ += ret;
    return ERR_SOCKET_CONTINUE;
}

int CommSocket::enable_noblock()
{
#if defined(WIN32) || defined(_WIN64)
    u_long nonblock = 1;
    return ::ioctlsocket (fd_, FIONBIO, &nonblock);
#else
    int val = ::fcntl(fd_, F_GETFL, 0);

    if (val == -1)
    {
        return -1;
    }

    // Turn on flags.
# define ZEN_SET_BITS(set_value, bits) (set_value |= (bits))
    ZEN_SET_BITS (val, O_NONBLOCK);

    if (::fcntl(fd_, F_SETFL, val) == -1)
    {
        return -1;
    }

    return 0;
#endif
}

int CommSocket::disable_noblock()
{
#if defined(WIN32) || defined(_WIN64)
    u_long nonblock = 1;
    return ::ioctlsocket(fd_, FIONBIO, &nonblock);
#else
    int val = ::fcntl(fd_, F_GETFL, 0);

    if (val == -1)
    {
        return -1;
    }

    // Turn off flags.
# define ZEN_CLR_BITS(clr_value, bits) (clr_value &= ~(bits))
    ZEN_CLR_BITS(val, O_NONBLOCK);

    if (::fcntl(fd_, F_SETFL, val) == -1)
    {
        return -1;
    }

    return 0;
#endif
}

int CommSocket::get_peer_ip()
{
    struct sockaddr_in addr;
    socklen_t sock_len = sizeof(addr);
    getpeername(fd_, (struct sockaddr *)&addr, &sock_len);
    return addr.sin_addr.s_addr;
}

int
CommSocket::set_pkg_info(unsigned int pkg_len_size, unsigned int pkg_len_offset)
{
    if (pkg_len_size != sizeof(unsigned short)
        && pkg_len_size != sizeof(unsigned int))
    {
        error_log("pkg len size is not support.\n");
        return ERR_SOCKET_INVALID_PARAM;
    }

    pkg_len_offset_ = pkg_len_offset;
    pkg_len_size_ = pkg_len_size;

    return ERR_SOCKET_FINISH;
}

int
CommSocket::connect(const char *ip, unsigned short port)
{
    sockaddr_in ipv4_addr;
    socklen_t addrlen = sizeof(sockaddr_in);
    ipv4_addr.sin_family = AF_INET;
    ipv4_addr.sin_port = htons(port);
    ipv4_addr.sin_addr.s_addr = ::inet_addr(ip);
#if defined(WIN32) || defined(WIN64)
    int result = ::connect (fd_,
        (sockaddr*)&ipv4_addr,
        addrlen);

    //将错误信息设置到errno，详细请参考上面ZEN_OS名字空间后面的解释
    if (-1 == result)
    {
        errno = ::WSAGetLastError ();
    }

    return result;
#else
    return ::connect (fd_,
        (sockaddr*)&ipv4_addr,
        addrlen);
#endif
}

int
CommSocket::wait(bool *wait_read, bool *wait_write, unsigned int wait_mil_sec)
{

    assert (wait_mil_sec > 0);
    // 不能啥事件都不wait
    assert (wait_read != NULL || wait_write != NULL);

    fd_set read_fdset;
    fd_set write_fdset;
    fd_set* p_read_fdset = NULL;
    fd_set* p_write_fdset = NULL;
    if (wait_read != NULL)
    {
        FD_ZERO(&read_fdset);
        FD_SET(fd_, &read_fdset);
        p_read_fdset = &read_fdset;
    }

    if (wait_write != NULL)
    {
        FD_ZERO(&write_fdset);
        FD_SET(fd_, &write_fdset);
        p_write_fdset = &write_fdset;
    }

    // tsocket_select并不能知道有没有超时，所以这里直接使用了select
    struct timeval tv;
    tv.tv_sec = wait_mil_sec/1000;
    tv.tv_usec = (wait_mil_sec%1000)*1000;
    int ret = select (fd_+1, p_read_fdset, p_write_fdset, NULL, &tv);
    if (ret < 0)
    {
        // 出错,错误码在sockerrno
        return ERR_SOCKET_FAIL;
    }
    else if (ret == 0)
    {
        // 超时
        return ERR_SOCKET_TIMEOUT;
    }

    // 有事件返回
    if (p_read_fdset != NULL)
    {
        *wait_read = (FD_ISSET(fd_, p_read_fdset) == 0);
    }
    if (p_write_fdset != NULL)
    {
        *wait_write = (FD_ISSET(fd_, p_write_fdset) == 0);
    }
    return ERR_SOCKET_FINISH;
}

int 
CommSocket::enable_cork()
{
#if (defined(WIN32) || defined(_WIN64))
	return 0;
#else
    int on = 1;
    ::setsockopt(fd_, SOL_TCP, TCP_CORK, &on, sizeof(on));
    return 0;
#endif
}

int 
CommSocket::disable_cork()
{
#if (defined(WIN32) || defined(_WIN64))
	return 0;
#else
    int on = 0;
    ::setsockopt(fd_, SOL_TCP, TCP_CORK, &on, sizeof(on));
    return 0;
#endif
}

int 
CommSocket::enbale_quickack()
{
#if (defined(WIN32) || defined(_WIN64))
	return 0;
#else
    int on = 1;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_QUICKACK, &on, sizeof(on));
    return 0;
#endif
}

int 
CommSocket::set_recv_buffer_size(unsigned int recv_buffer_size)
{
#if (defined(WIN32) || defined(_WIN64))
	return 0;
#else
    unsigned int old_recv_buffer_size;
    socklen_t len = sizeof(old_recv_buffer_size);

    ::getsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &old_recv_buffer_size, &len);
    info_log("get old recv buffer size %u", old_recv_buffer_size);

    ::setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &recv_buffer_size, sizeof(recv_buffer_size));
    info_log("set new recv buffer size %u", recv_buffer_size);
    return 0;
#endif
}

int 
CommSocket::sendfile(int file_fd, unsigned int offset, size_t *len)
{
#if (defined(WIN32) || defined(_WIN64))
	// 不支持sendfile
	error_log("not support sendfile in windows.");
	return 0;
#else
    off_t off = offset;
    int ret = ::sendfile(fd_, file_fd, &off, *len);

    if (ret < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // 下次再发送吧
            *len = 0;
            return ERR_SOCKET_CONTINUE;
        }

        return ERR_SOCKET_FAIL;
    }

    if (ret < *len)
    {
        *len = ret;
        return ERR_SOCKET_CONTINUE;
    }

    *len = ret; 
    return ERR_SOCKET_FINISH;
#endif
}

void 
CommSocket::get_recv_buffer(unsigned char **buffer, unsigned int *recv_len)
{
    *buffer = recv_buffer_ + effective_recv_begin_;
    unsigned int data_len = recv_buf_len_ - effective_recv_begin_;

    if (*recv_len > data_len)
    {
        *recv_len = data_len;
    }

    effective_recv_begin_ += *recv_len;

    if (recv_buf_len_ == effective_recv_begin_)
    {
        recv_buf_len_ = 0;
        effective_recv_begin_ = 0;
    }
}
