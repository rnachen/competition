#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#if defined(WIN32) || defined(_WIN64)

#include <windows.h>
#include <process.h>

#else

#include <pthread.h>
#include <unistd.h>

#endif

#include "comm_log.h"

const char * CommLog::log_file_name_ = NULL;
SIMPLE_LOG_LEVEL CommLog::log_level_ = LOG_DEBUG;
bool CommLog::need_head_ = false;

void CommLog::init_log(const char *log_file_name, int log_level, bool need_head)
{
    log_file_name_ = log_file_name;
    log_level_ = (SIMPLE_LOG_LEVEL)log_level;
    need_head_ = need_head;
}

void CommLog::simple_log(const char *func, SIMPLE_LOG_LEVEL level, const char *fmt, ...)
{
#if defined(WIN32) || defined(_WIN64)
    int pid = _getpid();
    DWORD cur_thread_id = ::GetCurrentThreadId();
#else
    int pid = (int)getpid();
    pthread_t cur_thread_id = ::pthread_self();
#endif

    static const char *err_msg[] =
    {
        "DEBUG",
        "INFO ",
        "WARN ",
        "ERROR"
    };

    // ÃÌº” ±º‰
    char time_s[256] = {0};
    time_t now;
    time(&now);
    strftime(time_s, sizeof(time_s), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char head[256] = {0};
    if (CommLog::need_head_)
    {
#if defined(WIN32) || defined(_WIN64)
    _snprintf(head, sizeof(head), "%d|%lu|%s|%s", pid, cur_thread_id, err_msg[level], func);
#else
    snprintf(head, sizeof(head), "%d|%lu|%s|%s", pid, cur_thread_id, err_msg[level], func);
#endif
    }

    char buf[1024] = {0};
    va_list ap;
    va_start(ap, fmt);

#if defined(WIN32) || defined(_WIN64)
    _vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, fmt, ap);
#else
    vsnprintf(buf, sizeof(buf), fmt, ap);
#endif

    va_end(ap);
    buf[sizeof(buf) - 1] = 0;

    FILE *fp = NULL;

    if (log_level_ <= level)
    {
        if (log_file_name_ != NULL)
        {
            fp = fopen(log_file_name_, "a");
        }

        if (fp == NULL)
        {
            fp = stdout;
        }

        if (CommLog::need_head_)
        {
            fprintf(fp, "%s|%s|", time_s, head);
        }
        fprintf(fp, "%s\n", buf);
        if (fp != stdout)
        {
            fclose(fp);
        }
    }
}

