/// @file       comm_epoll.h
/// @date       2015/12/11 23:11
///
/// @author     rnachen	
///
/// @brief      epoll 事件发生器
///


#include <errno.h>
#include <assert.h>
#include <string.h>
#include <map>
#include "comm_epoll.h"
#include "comm_log.h"

CommEpoll::CommEpoll():
    max_fd_(0),
    max_size_(1000000)
{
}

CommEpoll::~CommEpoll()
{
    // 清理
    std::map<unsigned int, CommEventHandler *>::iterator iter = event_handler_map_.begin();

    for (; iter != event_handler_map_.end(); ++iter)
    {
        // 通知连接关闭
        iter->second->handle_close();
    }

    event_handler_map_.clear();
}

int CommEpoll::proc( unsigned int wait_mil_second /*= 10*/ )
{
	nfds=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);  
    if(nfds==-1)  
    {  
        perror("start epoll_wait failed");  
        exit(EXIT_FAILURE);  
    }  
    
    for(int i=0;i<nfds;i++)  
    {  
    	if(events[i].events&EPOLLOUT)//有数据待发送，写socket
    	{
			int fd = events[i].data.fd;
			on_send(fd);
		}
		else if(events[i].events&EPOLLRDHUP)
		{
			int fd = events[i].data.fd;
			on_error(fd);
		}
		else
		{
			int fd = events[i].data.fd;
			on_recv(fd);
		}
     }  
    return 0;
}

int CommEpoll::on_recv(int fd)
{
    CommEventHandler *handler = get_event_handler(fd);

    if (handler == NULL)
    {
        return -2;
    }

    //game_proto::GamePkg pkg;
    if (handler->handle_input_ex() != 0)
    {
        on_error(fd);
        return 0;
    }

    return 0;
}

int CommEpoll::on_send(int fd)
{
    CommEventHandler *handler = get_event_handler(fd);

    if (handler == NULL)
    {
        return -1;
    }

    //game_proto::GamePkg pkg;
    if (handler->handle_output() != 0)
    {
        on_error(fd);
        return 0;
    }

    return 0;
}

int CommEpoll::regist(CommEventHandler *handler, bool is_need_mod)
{ 
    // 添加事件
    if (event_handler_map_.size() >= max_size_)
    {
        error_log("epoll size is full. curr size=%u", event_handler_map_.size());
        return -1;
    }

    CommEventHandler *old_handler = get_event_handler(handler->get_handle());
    if (old_handler != NULL && old_handler != handler)
    {
        error_log("handler exist. check your code");
        return -2;
    }

    event_handler_map_.insert(std::make_pair(handler->get_handle(), handler));

	if(is_need_mod)
    	mod_event(handler->get_handle(), EPOLLIN|EPOLLET);

    return 0;
}
/*
int CommEpoll::remove(CommEventHandler *handler)
{
    std::map<unsigned int, CommEventHandler *>::iterator iter
        = event_handler_map_.find((int)handler->get_handle());
    if (iter != event_handler_map_.end())
    {
        event_handler_map_.erase(iter);
    }

    handler->set_mask(0);

    mod_event(handler->get_handle(), 0);

    return 0;
}
*/
int CommEpoll::on_error(int fd)
{
    CommEventHandler *handler = get_event_handler(fd);
    if (handler != NULL)
    {
        remove(handler);
        handler->handle_close();
        return 0;
    }

    return 0;
}

CommEventHandler *CommEpoll::get_event_handler(int fd) const
{
    std::map<unsigned int, CommEventHandler *>::const_iterator iter = event_handler_map_.find(fd);

    if (iter != event_handler_map_.end())
    {
        return iter->second;
    }

    return NULL;
}

CommEpoll *CommEpoll::instance()
{
    static CommEpoll s_net_handler;
    return &s_net_handler;
}

void
CommEpoll::mod_event(int fd, int e)
{  	
	ev.data.fd = fd;
    ev.events=EPOLLIN|EPOLLET;
	int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&ev);
    if(ret == -1)
    {
	 	error_log("epoll mod event fail. ret=%d", ret);
    }
}

int CommEpoll::init_epoll(int fd)
{
    epoll_fd=epoll_create(MAX_EVENTS); 
    if(epoll_fd==-1)  
    {  
        error_log("epoll_create failed", epoll_fd);  
        exit(EXIT_FAILURE); 
    } 
	return 0;
}

int
CommEpoll::init(unsigned int max_event)
{
    max_size_ = max_event;

    return 0;
}

int
CommEpoll::remove(CommEventHandler *handler)
{
	std::map<unsigned int, CommEventHandler *>::iterator iter
       = event_handler_map_.find((int)handler->get_handle());
    if (iter != event_handler_map_.end())
    {
        event_handler_map_.erase(iter);
    }

    handler->set_mask(0);

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, handler->get_handle(), &ev);
}


