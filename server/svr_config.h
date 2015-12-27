/// @file       svr_config.h
/// @date       2013/12/05 20:56
///
/// @author
///
/// @brief      配置管理
///
///

#ifndef SVR_CONFIG_H_
#define SVR_CONFIG_H_

class SvrConfig
{
public:
    SvrConfig();
    ~SvrConfig();

    int load(const char *cfg_path);

public:
    char listen_ip_[16]; //
    unsigned short listen_port_; //
    unsigned int max_client_; // 最多允许的连接数
    char log_path_[256]; // 日志存放路径
    int log_level_; // 日志级别: debug info error

    char data_dir_[256]; // 数据文件存放目录

};

#endif // SVR_CONFIG_H_

