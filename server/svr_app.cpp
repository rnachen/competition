/// @file       svr_app.cpp
/// @date       2013/12/04 17:35
///
/// @author
///
/// @brief      主逻辑类实现
///
#include "svr_app.h"
#include "comm_refactor.h"
#include "comm_epoll.h"
#include "comm_log.h"
//#include <signal.h>

SvrApp *SvrApp::instance_ = NULL;
SvrApp *
SvrApp::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new SvrApp;
    }

    return instance_;
}

SvrApp::SvrApp()
{
    is_run_ = true;
}

SvrApp::~SvrApp()
{

}

int
SvrApp::run(int argc, const char **argv)
{
    const char *cfg_path = "server.conf";
    if (argv[1] != NULL)
    {
        cfg_path = argv[1];
    }

    int ret = init(cfg_path);
    if (ret != 0)
    {
        debug_log("init fail. ret=%d", ret);
        return ret;
    }

    while (is_run_ && ret == 0)
    {
        static const unsigned int WAIT_TIME_MIL_SECOND = 10;
        ret = CommEpoll::instance()->proc(WAIT_TIME_MIL_SECOND);
    }

    return ret;

}

//void
//SvrApp::on_timer_handle(int sig)
//{
//    GameAxis::instance()->move();
//    alarm(1);
//}

int
SvrApp::init(const char *cfg_path)
{
    int ret = load_cfg(cfg_path);
    if (ret != 0)
    {
        error_log("load config from %s fail. ret=%d", cfg_path, ret);
    }

    CommLog::init_log(conf_.log_path_, conf_.log_level_);

    // 初始化
    ret = CommEpoll::instance()->init(conf_.max_client_);
    if (ret != 0)
    {
        error_log("refactor init fail. ret=%d", ret);
        return ret;
    }

    // 侦听初始化
    ret = accept_handler_.start_service(conf_.listen_ip_, conf_.listen_port_);
    if (ret != 0)
    {
        error_log("start service fail. ret=%d", ret);
        return ret;
    }

	//初始化epoll
	ret = CommEpoll::instance()->init_epoll(accept_handler_.get_handle());
	
   // ret = CommRefactor::instance()->regist(&accept_handler_,
   //     CommEventHandler::READ_MASK|CommEventHandler::EXCEPT_MASK);
   ret = CommEpoll::instance()->regist(&accept_handler_, true);
    if (ret != 0)
    {
        error_log("net regist fail. ret=%d", ret);
        return ret;
    }
    // 精度要求不高，直接用简单的alarm定时器
    //signal(SIGALRM, on_timer_handle);
    //alarm(1);

    // 初始化结束
    return ret;
}

int
SvrApp::load_cfg(const char *cfg_path)
{
    return conf_.load(cfg_path);
}

