/// @file       comm_refactor.h
/// @date       2013/12/04 20:07
///
/// @author
///
/// @brief      select事件发生器
///
#ifndef COMM_REFACTOR_H_
#define COMM_REFACTOR_H_

#include "comm_event_handler.h"
#include "comm_socket.h"
#include <map>

class CommRefactor
{
public:
    int regist(CommEventHandler *handler, int e);

    int remove(CommEventHandler *handler);

    int init(unsigned int max_event);

    int proc(unsigned int wait_mil_second = 10);

    static CommRefactor *instance();

private:
    // 只能单例使用
    CommRefactor();
    virtual ~CommRefactor();

    int on_recv(int fd);
    int on_send(int fd);
    int on_error(int fd);

    CommEventHandler *get_event_handler(int fd) const;

    void mod_event(int fd, int e);

private:
    std::map<unsigned int, CommEventHandler *> event_handler_map_;
    fd_set read_fds_;
    fd_set write_fds_;
    fd_set error_fds_;

    unsigned int max_fd_;
    // 允许最多的连接数
    unsigned int max_size_;
};

#endif // COMM_REFACTOR_H_
