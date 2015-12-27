/// @file       svr_tcp_client_handler.cpp
/// @date       2013/12/05 9:59
///
/// @author
///
/// @brief      tcp�ͻ��˴���
///

#include "svr_tcp_client_handler.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include "comm_log.h"
//#include "comm_refactor.h"
#include "comm_epoll.h"
#include "svr_app.h"
#include "svr_config.h"

using namespace svr_proto;

SvrTcpClientHandler::SvrTcpClientHandler()
{

}

SvrTcpClientHandler::SvrTcpClientHandler(int fd)
{
    init(fd);
}

int
SvrTcpClientHandler::init(int fd)
{
    socket_.close();
    socket_.set_fd(fd);
    socket_.set_pkg_info(4, 0);
    socket_.enable_noblock();
    return 0;
}

int
SvrTcpClientHandler::get_handle(void) const
{
    return socket_.get_fd();
}

int
SvrTcpClientHandler::handle_input()
{
    int ret = socket_.recv();
    if (ret == CommSocket::ERR_SOCKET_FAIL)
    {
        // �հ�ʧ�ܣ��������ӿ����ѱ��Ͽ�, ����-1���ӻᱻ����
        return -1;
    }

    unsigned char *pkg_addr = NULL;
    unsigned int pkg_len = sizeof(recv_buffer_);
    svr_proto::SvrPkg pkg;

    while (socket_.get_pkg(&pkg_addr, &pkg_len) == 0)
    {
        // �յ�һ�������İ�, �Ƚ��
        ret = pkg.unpack((char *)pkg_addr, pkg_len);
        if (ret != 0)
        {
            // ���ʧ�ܣ�����-1�����ӻ�ر�
            info_log("unpack fail. pkg_len=%u", pkg_len);
            return -1;
        }

        // �������ݰ�
        ret = handle_recv_pkg(pkg);
        if (ret != 0)
        {
            info_log("handle recv pkg fail. ret=%d", ret);
            return ret;
        }

        pkg_len = sizeof(recv_buffer_);
    }

    // δ�յ�����������
    return 0;
}

int
SvrTcpClientHandler::handle_input_ex()
{
    int ret = socket_.recv();
    if (ret == CommSocket::ERR_SOCKET_FAIL)
    {
        // �հ�ʧ�ܣ��������ӿ����ѱ��Ͽ�, ����-1���ӻᱻ����
        return -1;
    }

    unsigned char *pkg_addr = NULL;
    unsigned int pkg_len = sizeof(recv_buffer_);
    svr_proto::SvrPkg pkg;

    while (socket_.get_pkg(&pkg_addr, &pkg_len) == 0)
    {
        // �յ�һ�������İ�, �Ƚ��
        ret = pkg.unpack((char *)pkg_addr, pkg_len);
        if (ret != 0)
        {
            // ���ʧ�ܣ�����-1�����ӻ�ر�
            info_log("unpack fail. pkg_len=%u", pkg_len);
            return -1;
        }

        // �������ݰ�
        ret = handle_recv_pkg(pkg);
        if (ret != 0)
        {
            info_log("handle recv pkg fail. ret=%d", ret);
            return ret;
        }

        pkg_len = sizeof(recv_buffer_);
    }

    // δ�յ�����������
    return 0;
}


int
SvrTcpClientHandler::handle_output()
{
	// ��������Ҫ����
	int ret = socket_.send(NULL, 0);

	if (ret == CommSocket::ERR_SOCKET_FAIL)
	{
		// ���ͳ���
		error_log("send fail. fd=%d", socket_.get_fd());
		return -1;
	}

	if (ret == CommSocket::ERR_SOCKET_CONTINUE)
	{
		// ���������
		//CommRefactor::instance()->regist(this,
		//	CommEventHandler::READ_MASK | CommEventHandler::WRITE_MASK | CommEventHandler::EXCEPT_MASK);
		CommEpoll::instance()->regist(this, false);
	}
	else if (ret == CommSocket::ERR_SOCKET_FINISH)
	{
	}
	else
	{
		// δ֪�Ĵ�����
		error_log("send fail. unknow err =%d", ret);
		return -1;
	}

    return 0;
}

int
SvrTcpClientHandler::handle_exception()
{
    error_log("tcp client %d exception. ", socket_.get_fd());
    return 0;
}

int
SvrTcpClientHandler::handle_close()
{
   info_log("tcp client %d close. ", socket_.get_fd());

   // ������new�����ģ�����Ҫ�Բã����治Ҫ���õ����������
   delete this;
   return 0;
}

int
SvrTcpClientHandler::handle_recv_pkg(const svr_proto::SvrPkg &pkg)
{
    int ret = 0;
    
    switch( pkg.head_.cmd_ ) {
    case SVR_CMD_CS_PLAYER_LOGIN: {
        ret = handle_player_login( pkg.body_.player_login_ );
        break;
    }
    
    case SVR_CMD_CS_PLAYER_GET_AXIS: {
        ret = handle_player_get_axis( pkg.body_.player_get_axis_ );
        break;
    }
    
    default:
        error_log( "unknow cmd:%d", pkg.head_.cmd_ );
        ret = -1;
        break;
    }

    if (ret != 0)
    {
        error_log("handler cmd=%d fail. ret=%d", pkg.head_.cmd_, ret);
    }

    return ret;
}

int
SvrTcpClientHandler::handle_player_login(const svr_proto::PlayerLogin &pkg_body)
{
    GameAxis::instance()->born( pkg_body.uin_ );
    GameAxis::instance()->get_sight_data( pkg_body.uin_,
                                          send_pkg_.body_.player_sight_ );
    send_pkg_.head_.cmd_ = SVR_CMD_SC_PLAYER_SIGHT;
    send_pkg_.body_.player_sight_.uin_ = pkg_body.uin_;
    send_pkg_.body_.player_sight_.time_ = time( NULL );
    
    return send_pkg(send_pkg_);
}

int
SvrTcpClientHandler::handle_player_get_axis(const svr_proto::PlayerGetAxis &pkg_body)
{
    GameAxis::instance()->move( pkg_body.time_ );
    GameAxis::instance()->get_sight_data( pkg_body.uin_,
                                          send_pkg_.body_.player_sight_ );
    send_pkg_.head_.cmd_ = SVR_CMD_SC_PLAYER_SIGHT;
    send_pkg_.body_.player_sight_.uin_ = pkg_body.uin_;
    send_pkg_.body_.player_sight_.time_ = pkg_body.time_;
    
    return send_pkg(send_pkg_);
}

int
SvrTcpClientHandler::send_pkg(const svr_proto::SvrPkg &send_pkg)
{
    size_t use_size = 0;
    int ret = send_pkg.pack(send_buffer_,
        sizeof(send_buffer_), &use_size);
    if (ret != 0)
    {
        error_log("pack fail. buffer is short. left size=%u", sizeof(send_buffer_));
        return -1;
    }

    ret = ( int )socket_.send( ( unsigned char * )send_buffer_, use_size );
    
    if( ret == CommSocket::ERR_SOCKET_CONTINUE ) {
        // δ�����꣬��������ͣ�����д�¼�
        // CommRefactor::instance()->regist(this,
        //     CommEventHandler::READ_MASK|CommEventHandler::WRITE_MASK|CommEventHandler::EXCEPT_MASK);
        CommEpoll::instance()->regist( this, false );
        return ret;
    }
    else if( ret == CommSocket::ERR_SOCKET_FINISH ) {
        // �������ˣ��ر�д�¼�
        //CommRefactor::instance()->regist(this,
        //   CommEventHandler::READ_MASK|CommEventHandler::EXCEPT_MASK);
        CommEpoll::instance()->regist( this, false );
    }
    else {
        // д����
        error_log( "send error. ret=%d", ret );
        return ret;
    }
    

    return ret;
}


