 cmake_minimum_required (VERSION 2.8)

include (common)
include (source_group)
include (cotire)

project (game_server_game_base)

include_directories (
    "../framework/base"
    "../framework/server"
    "../framework/lua_service"
	"../framework/base/tinyxml"
	"../framework/base/lua"
	"account"
	"account/source"
	"game_manager_service"	
)

set (HEADER_LIST
	account/Login.pb.h
	account/RoomOperate.pb.h
	account/PlayerListResp.pb.h
	account/RoomListResp.pb.h
	account/WhiteBoardMessage.pb.h
	account/RemoveShape.pb.h
	account/ErrorResp.pb.h
	account/player_def.h
    account/account_service.h
    account/account_opcode_def.h
	account/source/room.h
	account/source/room_manager.h
    account/source/player_session.h
    account/source/player_manager.h
    account/source/account_service_pimpl.h
	account/player_constructor.h
	#game_manager_service
	game_manager_service/game_manager_service.h
	game_manager_service/game_manager_service_pimpl.h
)

set (SRC_LIST
	account/Login.pb.cpp
	account/RoomOperate.pb.cpp
	account/PlayerListResp.pb.cpp
	account/RoomListResp.pb.cpp
	account/WhiteBoardMessage.pb.cpp
	account/RemoveShape.pb.cpp
	account/ErrorResp.pb.cpp
	account/source/room.cpp
	account/source/room_manager.cpp
	account/source/account_opcode_def.cpp
	account/source/player_def.cpp
    account/source/player_session.cpp

    account/source/account_service.cpp
    account/source/player_manager.cpp
    account/source/account_service_pimpl.cpp   
	
	game_manager_service/game_manager_service.cpp
	game_manager_service/game_manager_service_pimpl.cpp
)

source_group_by_dir(SRC_LIST)
source_group_by_dir(HEADER_LIST)

add_library (game_server_game_base SHARED ${SRC_LIST} ${HEADER_LIST})
# cotire (game_server_game_base)
