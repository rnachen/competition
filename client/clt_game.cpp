#include "clt_game.h"
#include "comm_game_axis.h"
#include "comm_log.h"
#include "clt_send_recv.h"
#include "svr_proto.h"
#include <string>
#include <sstream>

using namespace svr_proto;
static svr_proto::SvrPkg pkg;
uint32_t uin_ = 0;

int send_get_axis_sight_pkg(CommSocket &socket)
{
	pkg.head_.cmd_ = SVR_CMD_CS_PLAYER_GET_AXIS;
	pkg.body_.player_get_axis_.uin_ = uin_;
    pkg.body_.player_get_axis_.time_ = time(NULL);
	int ret = send_pkg(socket, pkg);
	if (ret != 0)
	{
		error_log("send get axis&sight pkg fail. ret=%d", ret);
		return ret;
	}

	return 0;
}

int read_sight_pkg(CommSocket &socket)
{
	int ret = read_pkg(socket, pkg);
	if (ret != 0)
	{
		error_log("read pkg fail. ret=%d", ret);
		return ret;
	}

	if (pkg.head_.cmd_ != SVR_CMD_SC_PLAYER_SIGHT)
	{
		error_log("recv bad pkg. need game sight pkg. but recv cmd=%d", pkg.head_.cmd_);
		return -1;
	}

    // client直接打印
	std::string axis_str = "";
    for (size_t i = 0; i < pkg.body_.player_sight_.sight_.count_; i++)
    {
        std::stringstream ss_x;
        std::stringstream ss_y;
        std::string x;
        std::string y;
        ss_x << pkg.body_.player_sight_.sight_.axis_[i].x_axis_;
        ss_x >> x;
        ss_y << pkg.body_.player_sight_.sight_.axis_[i].y_axis_;
        ss_y >> y;

        debug_log("x:%s, y:%s", x.c_str(), y.c_str());

        if ("" == axis_str)
            axis_str = "(";
        else
            axis_str += ",(";
        
        axis_str += x + "," + y + ")";
    }
    
    info_log("the sight data, uin[%u], timestamp[%u], sight[%s]", pkg.body_.player_sight_.uin_, pkg.body_.player_sight_.time_, axis_str.c_str());

	return 0;
}

int do_game(CommSocket &socket)
{
	// 开始游戏
	pkg.head_.cmd_ = SVR_CMD_CS_PLAYER_LOGIN;
	uin_ = getpid();  // 随机生成，直接取为pid
	pkg.body_.player_login_.uin_ = uin_;

	int ret = send_pkg(socket, pkg);
	if (ret != 0)
	{
		error_log("send game start pkg fail. ret=%d", ret);
		return ret;
	}

	// 接收登录后返回的视野包
	ret = read_sight_pkg(socket);
	if (ret != 0)
	{
		error_log("read game sight pkg fail. ret=%d", ret);
		return ret;
	}

	// 每隔一段时间获取一次变化的视野数据
	while (true)
	{
		ret = send_get_axis_sight_pkg(socket);
		if (ret != 0)
		{
			error_log("send get axis and sight pkg fail. err=%d", ret);
			return -1;
		}

		ret = read_sight_pkg(socket);
		if (ret != 0)
		{
			error_log("read sight pkg fail. ret=%d", ret);
			return -1;
		}

		// sleep 1s
		sleep(1);
	}

	return 0;
}

