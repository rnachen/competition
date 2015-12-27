/// @file       clt_send_recv.h
/// @date       2013/12/06 21:29
///
/// @author
///
/// @brief      客户端收发包
///

#ifndef CLT_SEND_RECV_H_
#define CLT_SEND_RECV_H_

#include "comm_socket.h"
#include "svr_proto.h"

unsigned long long get_file_size(const char *file_path);

int send_pkg(CommSocket &client_socket, const svr_proto::SvrPkg &pkg);

int read_pkg(CommSocket &client_socket, svr_proto::SvrPkg &pkg);

#endif

