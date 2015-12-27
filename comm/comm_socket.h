/// @file       comm_socket.h
/// @date       2013/12/04 19:03
///
/// @author
///
/// @brief      socket处理类
///

#ifndef COMM_SOCKET_H_
#define COMM_SOCKET_H_

#include "comm_define.h"
//#include "svr_proto.h"

class CommSocket
{
public:
    CommSocket();
    ~CommSocket();

    typedef enum
    {
        TCP = 1,
    } SocketType;

    typedef enum
    {
        SOCKET_EVENT_READ = 1,
        SOCKET_EVENT_WRITE = 2,
        SOCKET_EVENT_ERROR = 4,
    } SocketEvent;

    typedef enum
    {
        ERR_SOCKET_FINISH = 0, // 读写成功完成
        ERR_SOCKET_FAIL = 1, // 读写失败
        ERR_SOCKET_CONTINUE, // 需要继续读或者写
        ERR_SOCKET_PACKET_NOT_FINISH,
        ERR_SOCKET_BUFFER_NOT_ENOUGH,
        ERR_SOCKET_INVALID_PARAM,
        ERR_SOCKET_TIMEOUT, // 超时
    } SocketResult;

    // tcp or udp
    int create(SocketType type);

    void set_fd(int fd)
    {
        fd_ = fd;
    }

    int get_fd() const
    {
        return fd_;
    }

    void close();

    // fd是否用效
    bool is_valid() const ;

    // listen前要create成功了
    int listen(const char *ip, unsigned short port);
    int accept(CommSocket *socket);

    int connect(const char *ip, unsigned short port);

    // 设置包分片信息，仅支持带包长的包
    int set_pkg_info(unsigned int pkg_len_size, unsigned int pkg_len_offset);

    // 从已读到的缓冲区中取出一个完整的包，成功返回0 或者返回-1
    int get_pkg(unsigned char **game_pkg, unsigned int *game_pkg_len);

    void get_recv_buffer(unsigned char **buffer, unsigned int *recv_len);

    // 发送数据包，如果不能发完，则放到缓冲区中， buffer可以为空，如果buffer为空的话，则发送缓冲区中的数据
    // 缓冲区数据发送完成则返回ERR_SOCKET_FINISH
    SocketResult send(const unsigned char *buffer, unsigned int buf_len);
    // 读取缓冲区数据
    SocketResult recv();

    int sendfile(int file_fd, unsigned int offset, size_t *len);

    int wait(bool *read, bool *write, unsigned int wait_mil_sec);

    int get_event()
    {
        return event_;
    }

    void set_event(int event)
    {
        event_ = event;
    }

    int enable_noblock();
    int disable_noblock();
    int get_peer_ip();

    int enable_cork();
    int disable_cork();

    int enbale_quickack();

    int set_recv_buffer_size(unsigned int recv_buffer_size);

    int if_recv_buffer_will_mommove()
    {
        return ((recv_buf_len_ - effective_recv_begin_) <= (1024)) 
            && ((recv_buf_len_ - effective_recv_begin_) > 0);
    }

private:
    static const unsigned int MAX_SEND_BUFFER_LEN = 64 * 1024;
    static const unsigned int MAX_RECV_BUFFER_LEN = 64 * 1024;

    // 发送数据缓存
    unsigned char send_buffer_[MAX_SEND_BUFFER_LEN];
    // 接收数据缓存
    unsigned char recv_buffer_[MAX_RECV_BUFFER_LEN];

    // 接收到的数据长度
    unsigned int recv_buf_len_;
    // 有效数据开始位置, 之前的数据已经被处理了
    unsigned int effective_recv_begin_;   

    // 还需要发送的数据长度
    unsigned int need_send_buf_len_;
    // 发送数据偏移位置
    unsigned int effective_send_begin_; 

    int event_;
    int fd_;

    // 包长字段在包中的偏移位置
    unsigned int pkg_len_offset_;
    // 包长字段的字节数
    unsigned int pkg_len_size_;
};

#endif // COMM_SOCKET_H_
