/// @file       comm_define.h
/// @date       2013/12/04 21:20
///
/// @author
///
/// @brief      头文件包含以及通用的定义
///
#ifndef COMM_DEFINE_H_
#define COMM_DEFINE_H_

#if defined(WIN32) || defined(_WIN64)

#include <tchar.h>
#include <winsock2.h>

#if _WIN32_WINNT >= 0x0400
#include <windows.h>
#include <Ws2tcpip.h>
#else
#include <windows.h>
#include <Ws2tcpip.h>
#endif

#pragma comment(lib, "Ws2_32.lib")

#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif /* !EWOULDBLOCK */

#else // ! windows

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/sendfile.h>
#endif

#endif // COMM_DEFINE_H_
