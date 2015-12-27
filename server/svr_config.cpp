/// @file       svr_config.cpp
/// @date       2013/12/05 21:12
///
/// @author
///
/// @brief      ≈‰÷√¥¶¿Ì¿‡
///
///
#include "svr_config.h"
#include <string.h>
#include "comm_key_value_config.h"
#include "comm_log.h"

SIMPLE_LOG_LEVEL get_log_level(const char *log_level)
{
    if (strcmp(log_level, "debug") == 0)
    {
        return LOG_DEBUG;
    }
    else if (strcmp(log_level, "info") == 0)
    {
        return LOG_INFO;
    }
    else if (strcmp(log_level, "warn") == 0)
    {
        return LOG_WARN;
    }
    else if (strcmp(log_level, "error") == 0)
    {
        return LOG_ERROR;
    }
    return LOG_ERROR;
}

int
SvrConfig::load(const char *cfg_path)
{
    CommKeyValueConfig kv_config;
    if (kv_config.load_from_file(cfg_path) != 0)
    {
        info_log("load conf %s fail. ", cfg_path);
        return -1;
    }

    kv_config.get_str("log_path", log_path_, sizeof(log_path_), "./log/svr.log");
    char log_level[32];
    kv_config.get_str("log_level", log_level, sizeof(log_level), "debug");
    log_level_ = (int)get_log_level(log_level);

    kv_config.get_str("listen_ip", listen_ip_, sizeof(listen_ip_), "0.0.0.0");
    kv_config.get_short("listen_port", (short *)&listen_port_, 9300);
    kv_config.get_int("max_client", (int *)&max_client_, 10000);

    kv_config.get_str("data_dir", data_dir_, sizeof(data_dir_), "./data");

    return 0;
}

SvrConfig::SvrConfig()
{
    log_level_ = LOG_INFO;
    log_path_[0] = 0;
    listen_port_ = 0;
    listen_ip_[0] = 0;
}

SvrConfig::~SvrConfig()
{

}
