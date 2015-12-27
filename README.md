1. 源码目录说明
    bin: 运行目录，包含配置以及启停脚本，linux下编译的二进制也会输出到这个目录
    client: 客户端代码,需要编译客户端时，直接在这个目录make即可，如要编译release版本，使用make DEBUG=0
    comm: 客户端和服务端公共代码
    proj: vs2005工程文件，可在windows下编译调试
    proto: tdr协议目录，如果需要修改协议，修改svr_proto.xml后运行tdrgen.bat即可重新生成协议代码
    server: 服务端代码，需要编译服务端时，直接在这个目录make即可，如要编译release版本，使用make DEBUG=0
    
2. 运行说明
    编译生成的二进制在bin目录，svr可直接在fdsf bin下直接运行./server, 默认配置在bin下的server.conf
    可通过配置调整日志级别，以及侦听ip和端口，默认配置侦听的是127.0.0.1, 测试时需要修改成内网IP
    
