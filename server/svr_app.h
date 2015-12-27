/// @file       svr_app.h
/// @date       2013/12/04 17:30
///
/// @author
///
/// @brief      主逻辑类定义
///

#ifndef SVR_APP_H
#define SVR_APP_H

#include "svr_config.h"
#include "svr_accept_handler.h"
#include "comm_game_axis.h"

// 主逻辑类定义
class SvrApp
{
public:
    // 获取单例
    static SvrApp *instance();

    // 运行
    int run(int argc, const char **argv);

    //static void on_timer_handle(int sig);

    const SvrConfig &get_config() const
    {
        return conf_;
    }

protected:
    // 初始化
    int init(const char *cfg_path);

    // 加载配置
    int load_cfg(const char *cfg_path);

    SvrApp();
    ~SvrApp();

private:
    // 主配置对象
    SvrConfig conf_;

    // 侦听事件处理
    SvrAcceptHandler accept_handler_;

    // 是否继续运行
    bool is_run_;

    // 单例对象
    static SvrApp *instance_;
};

#endif
