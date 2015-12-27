/// @file       comm_epoll.h
/// @date       2015/12/11 23:11
///
/// @author     rnachen	
///
/// @brief      epoll 事件发生器
///
#ifndef COMM_EPOLL_H_
#define COMM_EPOLL_H_

#include "comm_event_handler.h"
#include "comm_socket.h"
#include <stdio.h>     
#include <sys/types.h>     
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h>   
#include <stdlib.h>  
#include <string.h>  
#include <sys/epoll.h>

#include <map>

#define BUFFER_SIZE 40  
#define MAX_EVENTS 1000000


class CommEpoll
{
public:
    int regist(CommEventHandler *handler, bool is_need_mod);

    int remove(CommEventHandler *handler);

    int init(unsigned int max_event);

    int proc(unsigned int wait_mil_second = 10);

	int init_epoll(int fd);

    static CommEpoll *instance();

private:
    // 只能单例使用
    CommEpoll();
    virtual ~CommEpoll();

    int on_recv(int fd);
    int on_send(int fd);
    int on_error(int fd);

    CommEventHandler *get_event_handler(int fd) const;

    void mod_event(int fd, int e);

private:
	int epoll_fd;//
	int nfds;// epoll监听事件发生的个数
	struct epoll_event ev;// epoll事件结构体  
	struct epoll_event events[MAX_EVENTS];// 事件监听队列  
    std::map<unsigned int, CommEventHandler *> event_handler_map_;

    unsigned int max_fd_;
    // 允许最多的连接数
    unsigned int max_size_;

	
};

#endif // COMM_EPOLL_H_
