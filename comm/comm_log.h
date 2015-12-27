/// @file       comm_log.h
/// @date       2013/12/04 19:06
///
/// @author     aaron
///
/// @brief      ¥Ú”°»’÷æ
///
#ifndef COMM_LOG_H_
#define COMM_LOG_H_

enum SIMPLE_LOG_LEVEL
{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

#define DEBUG 

#if defined DEBUG
#define debug_log(...) CommLog::simple_log(__FUNCTION__, LOG_DEBUG,   ##__VA_ARGS__)
#else
#define debug_log(...) do {} while (0); 
#endif

#define info_log(...)  CommLog::simple_log(__FUNCTION__, LOG_INFO,    ##__VA_ARGS__)
#define warn_log(...)  CommLog::simple_log(__FUNCTION__, LOG_WARN,    ##__VA_ARGS__)
#define error_log(...) CommLog::simple_log(__FUNCTION__, LOG_ERROR,   ##__VA_ARGS__)

class CommLog
{
    public:
        static void init_log(const char *log_file_name, int log_level, bool need_head = true);
        static void simple_log(const char *func, SIMPLE_LOG_LEVEL level, const char *fmt, ...);

    private:
        static SIMPLE_LOG_LEVEL log_level_;
        static const char *log_file_name_;
        static bool need_head_;
};

#endif // COMM_LOG_H_

