cmake_minimum_required (VERSION 2.8)

include (common)
include (source_group)
include (cotire)

project (DN_Account)

#set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/DN_application/DN_Account")

include_directories (
    "../../framework/base"
    "../../framework/server"
    "../../framework/lua_service"
    "../../game_base/caishen"
    "../../game_base/account"
	"../../game_base/trade"
	"../../game_base/item"
	"../../game_base/treasure"
    "../../game_base/account/source"
    "../../game_base/lua_service"
    "../../game_base/money_tree"
    "../../game_base/reward_base"
    "../../game_base/hall"
    "../../game_base/contests"
    "../../game_base/task"
	"../../game_base/chart"
	"../../game_base/message"
	"../../game_base/oss_log"
	"../../game_base/global_table_data"
	"../../DN_application/DN_Task"
	"../../DN_application/DN_Lua"
	"../../DN_application/DN_Contests"
	"../../DN_application/DN_Account"
	"../../DN_application/DN_Hall"
	"../../game_base/items_exchange"
    "./"
)

set (HEADER_LIST
		dn_account_service_pimpl.h
		DN_player_def.h
		dn_player_session.h
		dn_poker.h		
)

set (SRC_LIST
    dn_account_service_pimpl.cpp
    dn_account_service_pimpl_app.cpp
    DN_player_def.cpp
    dn_player_session.cpp
    dn_poker.cpp 
)

source_group_by_dir(SRC_LIST)
source_group_by_dir(HEADER_LIST)

add_library (dnasp SHARED ${SRC_LIST} ${HEADER_LIST})
# cotire (dnasp)


