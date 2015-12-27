/// @file       comm_refactor.cpp
/// @date       2013/12/04 21:04
///
/// @author
///
/// @brief      select事件发生器
///

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <map>
#include "comm_refactor.h"
#include "comm_log.h"

CommRefactor::CommRefactor():
    max_fd_(0),
    max_size_(1000)
{
    memset(&read_fds_, 0, sizeof(read_fds_));
    memset(&write_fds_, 0, sizeof(write_fds_));
    memset(&error_fds_, 0, sizeof(error_fds_));
}

CommRefactor::~CommRefactor()
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

int CommRefactor::proc( unsigned int wait_mil_second /*= 10*/ )
{
    fd_set read_fds;
    fd_set write_fds;
    fd_set error_fds;
    memcpy(&read_fds, &read_fds_, sizeof(read_fds));
    memcpy(&write_fds, &write_fds_, sizeof(write_fds));
    memcpy(&error_fds, &error_fds_, sizeof(error_fds));

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000 * wait_mil_second;
    // int ret = select(max_fd_ + 1, &read_fds, &write_fds, NULL, &tv);
    int ret = select(max_fd_ + 1, &read_fds, &write_fds, &error_fds, &tv);

    if (ret < 0)
    {
        error_log("select failed, ret = %d, err = %s", ret, strerror(errno));
        return -1;
    }

    int count = 0;

    for (unsigned int i = 0; i <= max_fd_ && count < ret; ++i)
    {
        if (FD_ISSET(i, &read_fds))
        {
            on_recv(i);
            count ++;
        }

        if (FD_ISSET(i, &write_fds))
        {
            on_send(i);
            count ++;
        }

        if (FD_ISSET(i, &error_fds))
        {
            on_error(i);
            count ++;
        }
    }

    return 0;
}

int CommRefactor::on_recv(int fd)
{
    CommEventHandler *handler = get_event_handler(fd);

    if (handler == NULL)
    {
        return -2;
    }

    //game_proto::GamePkg pkg;
    if (handler->handle_input() != 0)
    {
        on_error(fd);
        return 0;
    }

    return 0;
}

int CommRefactor::on_send(int fd)
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

int CommRefactor::regist(CommEventHandler *handler, int e)
{
    if (e == 0)
    {
        error_log("bad event");
        return -1;
    }
    // 添加事件
    if (event_handler_map_.size() >= max_size_)
    {
        error_log("refactor size is full. curr size=%u", event_handler_map_.size());
        return -1;
    }

    CommEventHandler *old_handler = get_event_handler(handler->get_handle());
    if (old_handler != NULL && old_handler != handler)
    {
        error_log("handler exist. check your code");
        return -2;
    }

    event_handler_map_.insert(std::make_pair(handler->get_handle(), handler));

    mod_event(handler->get_handle(), e);

    return 0;
}

int CommRefactor::remove(CommEventHandler *handler)
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

int CommRefactor::on_error(int fd)
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

CommEventHandler *CommRefactor::get_event_handler(int fd) const
{
    std::map<unsigned int, CommEventHandler *>::const_iterator iter = event_handler_map_.find(fd);

    if (iter != event_handler_map_.end())
    {
        return iter->second;
    }

    return NULL;
}

CommRefactor *CommRefactor::instance()
{
    static CommRefactor s_net_handler;
    return &s_net_handler;
}

void
CommRefactor::mod_event(int fd, int e)
{
    if (e & CommEventHandler::READ_MASK)
    {
        FD_SET(fd, &read_fds_);
    }
    else
    {
        FD_CLR(fd, &read_fds_);
    }

    if (e & CommEventHandler::WRITE_MASK)
    {
        FD_SET(fd, &write_fds_);
    }
    else
    {
        FD_CLR(fd, &write_fds_);
    }

    if (e & CommEventHandler::EXCEPT_MASK)
    {
        FD_SET(fd, &error_fds_);
    }
    else
    {
        FD_CLR(fd, &error_fds_);
    }

    if (e != 0)
    {
        if ((int)max_fd_ < fd)
        {
            max_fd_ = fd;
        }
    }
    else
    {
        if ((int)max_fd_ == fd)
        {
            // 找到第二大的
            for (int i = (int)(max_fd_) - 1; i >= 0; --i)
            {
                if (FD_ISSET(i, &read_fds_)
                    || FD_ISSET(i, &write_fds_)
                    || FD_ISSET(i, &error_fds_))
                {
                    max_fd_ = i;
                    break;
                }
            }
        }
    }
}

int
CommRefactor::init(unsigned int max_event)
{
    max_size_ = max_event;
    return 0;
}

