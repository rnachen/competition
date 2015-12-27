::本批处理的目的辅助根据TDR的XML配置文件，生产H,CPP文件，(按照团队内部的规范)
::同时为了避免重复生成，造成大家多次编译，使用了文件简单的文件时间戳比较规则，
::配置和协议部分分到使用不同的方式是因为TDR的自己的不舒服，特别是目前C语言版本(还有不同平台步兼容的问题)的情况，以后换到C++版本也许好一点,

::bat是个很神奇的东东，我个人觉得他不是为程序员准备的，希望你喜欢，
::http://www.bathome.net/ 关于批处理的各种资料，你可以在这个网站找到，比较全面了
::BTW：由于该死的cmd的时间戳不支持s，所以如果在一分钟内编译，你仍然可能编译两次

::命令格式说明tdrgen 参数1：处理的目录  [参数2：相关的平台,win32,x64]，
::参数2可选，如果步声明，跟进你当前的环境进行抉择,

@echo off

setlocal 

::--------------------------------------------------------------------------------------------

set cur_path_=%~dp0
set os_name_=%~1

if "%os_name_%"=="" (
    if /i "%PROCESSOR_IDENTIFIER:~0,3%"=="x86" ( set os_name_=win32) else ( set os_name_=win64)
)else (
    if /i "%os_name_%"=="win32" ( set os_name_=win32) else ( set os_name_=win64)
)

::实现简单的文件时间戳比较功能，

set xml_filename_=
set xml_timestamp_=
set h_filename_=
set h_timestamp_=
set cpp_filename_=
set cpp_timestamp_=
set game_channel_file_name=

::处理的目录，如果有增加，请在后面增加
set process_set_=.\

:: bat中变量累加需要使用!,并且要打开延迟扩展
:: 关于延迟扩展，请阅读http://www.bathome.net/thread-2189-1-1.html
setlocal enabledelayedexpansion

for %%d in (%process_set_%) do (
    set process_dir_=%%d
    echo Check dir [!cur_path_!!process_dir_!]

    for /f "delims==" %%i in ('dir /b !process_dir_!*_proto*.xml ' ) do (
        set xml_filename_=!process_dir_!%%i
        set cpp_filename_=!xml_filename_:~0,-3!cpp
        set h_filename_=!xml_filename_:~0,-3!h

        ::如果文件不存在，之间跳过，要求进行检查
        if not exist !cpp_filename_! ( goto label_need_tdr )
        if not exist !h_filename_! ( goto label_need_tdr )
        
        for /f "delims==" %%x in ("!xml_filename_!") do (
           ::echo %%x %%~tx
           set xml_timestamp_=%%~tx
           set xml_timestamp_=!xml_timestamp_: =!
        )

        ::比较CPP文件的时间戳和XML文件的时间戳
        for /f "delims==" %%x in ("!cpp_filename_!") do (
           ::echo %%x %%~tx
           set cpp_timestamp_=%%~tx
           set cpp_timestamp_=!cpp_timestamp_: =!
           if !cpp_timestamp_! leq !xml_timestamp_! ( goto label_need_tdr )
        )
        
        ::比较H文件的时间戳和XML文件的时间戳
        for /f "delims==" %%y in ("!h_filename_!") do (
           ::echo %%y %%~ty
           set h_timestamp_=%%~ty
           set h_timestamp_=!h_timestamp_: =!
           if !h_timestamp_! leq !xml_timestamp_! ( goto label_need_tdr )
        )
    )
)

:label_notneed_tdr
echo not need tdr process
goto label_right_exit

:label_need_tdr
echo need tdr process
::--------------------------------------------------------------------------------------------

::选择TDR程序


set tdr_cmd_=%~dp0\tdr.exe

:: for中间用set必须用延迟扩展,
setlocal enabledelayedexpansion

for %%d in (%process_set_%) do (
    :: 生成协议TDR
    set proto_xml_files_=
    set process_dir_=%%d
    
    echo Process dir [!cur_path_!!process_dir_!]
    
    for %%i in (!process_dir_!*_proto*.xml) do (
        :: 变量的累加必须使用!
        set proto_xml_files_=!proto_xml_files_! %%i
    )
    echo show xmlfile[ !proto_xml_files_!]    
    !tdr_cmd_! --xml2cpp -pl -O !process_dir_! !proto_xml_files_!
    if not %ERRORLEVEL% == 0 goto label_error_pocess

)

::--------------------------------------------------------------------------------------------
:label_right_exit
::正常退出
endlocal
cd %cur_path_%
exit /b 0

::--------------------------------------------------------------------------------------------
::退出控制点
:label_error_pocess
endlocal
cd %cur_path_%
exit /b 110
