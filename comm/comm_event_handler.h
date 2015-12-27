/// @file       comm_event_handler.h
/// @date       2013/12/04 21:03
///
/// @author
///
/// @brief      事件处理类
///
#ifndef COMM_EVENT_HANDLER_H_
#define COMM_EVENT_HANDLER_H_

#include "comm_define.h"

class CommRefactor;
class CommEventHandler
{
public:

    ///IO触发事件
    enum EVENT_MASK
    {
        NULL_MASK    = 0,
        ///读取事件，但句柄变为可读是，调用handle_input
        READ_MASK    = (1 << 1),
        ///写事件，当句柄变为可写是，handle_output
        WRITE_MASK   = (1 << 2),
        ///异常事件，触发后调用handle_exception
        EXCEPT_MASK  = (1 << 3),
        ///异步CONNECT成功后，调用handle_output，异步CONNECT失败后，调用handle_input，
        CONNECT_MASK = (1 << 4),
        ///ACCEPT事件，当一个ACCEPT端口可以链接后，调用handle_input，
        ACCEPT_MASK  = (1 << 5),
    };

public:

    /*!
    @brief      构造函数
    */
    CommEventHandler();
    /*!
    @brief      构造函数，同时设置香港的反应器指针
    @param      reactor 句柄相关的反应器指针
    */
    CommEventHandler(CommRefactor *reactor);
    /*!
    @brief      析构函数
    */
    virtual ~CommEventHandler();

public:

    /*!
    @brief      取回对应的ZEN_SOCKET 句柄
    @return     ZEN_SOCKET CommEventHandler 对应的ZEN_SOCKET 句柄
    */
    virtual int get_handle(void) const = 0;

    /*!
    @brief      读取事件触发调用函数，用于读取数据，accept成功，（connect失败）处理，
    @return     int 返回0表示句柄处理正常，return -1后，反应器会主动handle_close，帮助结束句柄
    */
    virtual int handle_input();

    /*!
    @brief      写入事件触发调用函数，用于写数据，connect成功，
    @return     int int 返回0表示句柄处理正常，return -1后，反应器会主动handle_close，帮助结束句柄
    */
    virtual int handle_output();

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

    /*!
    @brief      取得当前的标志位
    @return     int 返回的当前的MASK值
    */
    inline int get_mask();

    /*!
    @brief      设置当前标志位
    @param      mask 设置的MASK值
    */
    inline void set_mask(int mask);

    /*!
    @brief      enable mask所带的标志位
    @param      en_mask 打开的MASK的值
    */
    inline void enable_mask(int en_mask);

    /*!
    @brief      disable mask所带的标志位
    @param      dis_mask 关闭的MASK值
    */
    inline void disable_mask(int dis_mask);

    /*!
    @brief      设置反应器
    @param      reactor
    */
    virtual void reactor (CommRefactor *reactor);

    /*!
    @brief      取得自己所属的反应器
    @return     *
    */
    virtual CommRefactor *reactor (void) const;

    //超时处理，最后考虑再三，没有在反应器里面集成定时器的处理
    //virtual int handle_timeout (const ZEN_Time_Value &tv, const void *arg = 0);

protected:

    ///反应器
    CommRefactor       *reactor_;

    ///这个句柄对应要处理的事件MASK
    int                event_mask_;
};

//取得当前的标志位
inline int CommEventHandler::get_mask()
{
    return event_mask_;
}

//设置当前标志位
inline void CommEventHandler::set_mask(int mask)
{
    event_mask_ = mask;
}

//enable mask所带的标志位
inline void CommEventHandler::enable_mask(int en_mask)
{
    event_mask_ |= en_mask;
}
//disable mask所带的标志位
inline void CommEventHandler::disable_mask(int dis_mask)
{
    event_mask_ &= (~dis_mask);
}

#endif // COMM_EVENT_HANDLER_H_
