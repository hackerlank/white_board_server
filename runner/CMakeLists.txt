cmake_minimum_required (VERSION 2.8)

include (common)
include (source_group)
include (cotire)

project (game_server_runner)

include_directories (
    "../framework/base"
    "../framework/server"
    "../framework/lua_service"
    "../game_base/account"
	"../game_base/account/source"
    "../game_base/game_manager_service"
    "../DN_application"
)

set (HEADER_LIST
)

set (SRC_LIST
    main.cpp
)

source_group_by_dir(SRC_LIST)
source_group_by_dir(HEADER_LIST)

add_executable (game_server_runner ${SRC_LIST} ${HEADER_LIST})
cotire (game_server_runner)

target_link_libraries (game_server_runner
	stdc++
    game_server_game_base
    game_server_framework
    uv
    glog
	log4cplus
    gflags
    z
    zookeeper_mt
    rabbitmq
    mysqlclient
    hiredis
    dl
	cryptopp
	protobuf
)
 if (APPLE)
 target_link_libraries (game_server_runner
     lua.5.2
 )
 else (APPLE)
 target_link_libraries (game_server_runner
    lua5.2
    
 )
 endif (APPLE)

if (CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries (game_server_runner
        rt
        pthread
    )
endif (CMAKE_COMPILER_IS_GNUCC)

