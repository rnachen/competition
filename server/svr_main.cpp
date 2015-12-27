/// @file       svr_main.cpp
/// @date       2013/12/04 16:01
///
/// @author
///
/// @brief      svrÈë¿Ú
///

#include <stdio.h>
#include <assert.h>
#include "comm_log.h"
#include "svr_app.h"

void usage(const char *bin_path)
{
    assert(bin_path != NULL);
    printf("Usage:%s [cfg_path]\n\n", bin_path);
}

int main(int argc, const char **argv)
{
    if (argc > 2)
    {
        usage(argv[0]);
        return -1;
    }

    int ret = SvrApp::instance()->run(argc, argv);
    info_log("server demo exit, ret = %d", ret);
    return ret;
}

