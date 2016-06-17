cmake_minimum_required (VERSION 2.8)

include (common)
include (source_group)
include (cotire)

project (game_server_framework)

include_directories (
    "base"
    "server"
    "lua_service"
)

set (HEADER_LIST
	lua_service/game_server_service.h
	base/csv_parser/csv_parser.h
    base/tinyxml/tinystr.h
	base/tinyxml/tinyxml.h
	base/beluga_track.h
    base/circularfifo.hpp
    base/circularfifo_memory_relaxed_aquire_release.hpp
    base/circularfifo_memory_sequential_consistent.hpp
    base/double_buffer_queue.h
    base/event_loop.h
    base/game_loop.h
    base/logger.h
    base/logging.h
	base/logpluslogger.h
    base/scope_guard.h
    base/service.h
    base/singleton.h
    base/google_logger.h
    base/runnable.h
    base/thread_pool.h
    base/worker.h
    base/timer.h
    base/lua/lua_tinker.hpp
    base/script_base.hpp
    base/game_script.h
    base/CbAIObj.h
    base/CbAIState.h
    base/CbStateMachine.h
    base/CbFormat.h
    base/CbCommonInclude.h
    base/posix_sem.h
	server/async.h
    server/server.h
    server/server_message_codes.h
    server/service_manager.h
    server/tcp_client.h
    server/tcp_connection.h
    server/tcp_server.h
    server/tcp_server_message_codes.h
    server/dependencyResolver.h
    server/transcode.h
	server/transcode_protobuf.h
	server/transcode_endpoint.h
    server/transcode_packet.h
    server/transcode_zip.h
    server/zookeeper_service.h
    server/zookeeper_node.h
    server/zookeeper_role.h
    server/zookeeper_client.h
    server/rabbitmq_client.h
    server/global_error_code.h
    server/file_monitor.h
    server/redis_manager.h
    server/mysql_connection.h
    server/mysql_database.h
    server/mysql_rst.h
    server/database_manager.h
    server/async_work_manager.h
    server/http_parse_query.h
    server/http_request.h
    server/http_response.h
    server/game_service.h
    server/packet_dispatcher.h
	server/mongoose.h
	base/cb_time.h
	base/cb_span_time.h
	base/rapidjson_utility.h
	base/encrypt.h
	base/encrypthelper.h
	base/encryption.h
)

set (SRC_LIST
	base/encrypt.cpp
	base/encrypthelper.cpp
	base/encryption.cpp
	lua_service/game_server_service.cpp
	base/csv_parser/csv_parser.cpp
    base/posix_sem.cpp
	base/tinyxml/tinystr.cpp
	base/tinyxml/tinyxml.cpp
	base/tinyxml/tinyxmlerror.cpp
	base/tinyxml/tinyxmlparser.cpp
	base/beluga_track.cpp
    base/game_loop.cpp
    base/logger.cpp
    base/logging.cpp
	base/logpluslogger.cpp
    base/service.cpp
    base/google_logger.cpp
    base/runnable.cpp
    base/thread_pool.cpp
    base/worker.cpp
    base/lua/struct.c
    base/lua/lua_tinker.cpp
    base/script_base.cpp
    base/game_script.cpp
    base/CbAIObj.cpp
    base/CbFormat.cpp
    server/async.cpp
    server/server.cpp
    server/service_manager.cpp
    server/tcp_client.cpp
    server/tcp_connection.cpp
    server/tcp_server.cpp
    server/dependencyResolver.cpp
    server/transcode.cpp
	server/transcode_protobuf.cpp
    server/transcode_endpoint.cpp
    server/transcode_packet.cpp
    server/transcode_zip.cpp
    server/zookeeper_service.cpp
    server/zookeeper_node.cpp
    server/zookeeper_role.cpp
    server/zookeeper_client.cpp
    server/rabbitmq_client.cpp
    server/file_monitor.cpp
    server/redis_manager.cpp
    server/mysql_connection.cpp
    server/mysql_database.cpp
    server/mysql_rst.cpp
    server/database_manager.cpp
    server/async_work_manager.cpp
    server/http_request.cpp
    server/http_response.cpp
    #server/http_server.cpp
    #server/http_request_handler.cpp
    server/game_service.cpp
    server/packet_dispatcher.cpp
	server/mongoose.c
	base/cb_time.cpp
	base/rapidjson_utility.cpp
)

source_group_by_dir(SRC_LIST)
source_group_by_dir(HEADER_LIST)

add_library (game_server_framework SHARED ${SRC_LIST} ${HEADER_LIST})
# cotire (game_server_framework)
