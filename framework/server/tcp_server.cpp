//
//  tcp_server.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/5/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "tcp_server.h"

#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <thread>

#include "logging.h"
#include "tcp_connection.h"
#include "tcp_server_message_codes.h"
#include "game_service.h"
#include "service_manager.h"


//延迟删除conn的时间
static const time_t kDeleteInterval = 5 * 60;

class TcpServerCallback {
public:
    static void on_new_connection(uv_stream_t *server, int status) {
        if (status == -1) {
            LOG(ERROR) << " on_new_connection TcpConnection,status = " << status;
            return;
        }

        tcp_server_t * tcp_server = reinterpret_cast<tcp_server_t*>(server);
        assert(tcp_server);
        
        TcpConnection* conn = tcp_server->server->connection_creator_(tcp_server->server, tcp_server->server);
        if (uv_accept(server, reinterpret_cast<uv_stream_t *>(conn->stream())) == 0) {
            if(!conn->Init()) {
                LOG(ERROR) << "connection init failed.";
                tcp_server->server->connection_deletor_(conn);
                return;
            }
            
            conn->StartRead();
            tcp_server->server->OnNewConnection(conn);
        }
    }
    
    static TcpConnection* default_new_connection(Async* async_owner, Server* server) {
        return new TcpConnection(async_owner, server);
    }
    
    static void default_delete_connection(TcpConnection* conn) {
        if (conn) {
            delete conn;
        }
    }
};

TcpServer::TcpServer(int port) : TcpServer(port, &TcpServerCallback::default_new_connection,
                                           &TcpServerCallback::default_delete_connection) {

}

TcpServer::TcpServer(int port, connection_creator_func connection_creator, connection_deletor_func connection_deletor)
    : port_(port)
    , server_(nullptr)
    //, new_connections_(nullptr)
    , connection_creator_(connection_creator)
    , connection_deletor_(connection_deletor)
{
   // new_connections_ = new new_connections_queue(10240);
}

TcpServer::~TcpServer() {
    if (server_) {
        free(server_);
        server_ = nullptr;
    }
    
//     if (new_connections_) {
//         delete new_connections_;
//         new_connections_ = nullptr;
//     }
//      circlefifo 内容只能在创建线程中销毁，故需移到OnShutDown，在update线程中调用
}

bool TcpServer::Init() {
    bool ret = Server::Init();
    assert(ret);
    if (!ret) {
        return ret;
    }
    
    if (!server_) {
        server_ = static_cast<tcp_server_t*>(calloc(1, sizeof(tcp_server_t)));
        server_->server = this;
        uv_tcp_init(loop_, &server_->tcp);
    }

    return true;
}

bool TcpServer::Start() {
    struct sockaddr_in addr = uv_ip4_addr("0.0.0.0", port_);
    uv_tcp_bind(&server_->tcp, addr);
    
    if (uv_listen(reinterpret_cast<uv_stream_t *>(&server_->tcp), 128, &TcpServerCallback::on_new_connection)) {
        LOG(ERROR) << "Listen error: " << uv_strerror(uv_last_error(loop_));
        return false;
    }

    return Server::Start();
}

bool TcpServer::Stop() {
    bool ret = Server::Stop();
    assert(ret);
    return ret;
}

uv_loop_t* TcpServer::GetClientLoop() {
    return loop_;
}

void TcpServer::OnMessage(Message* msg) {
    switch (msg->code) {
        case TCP_CONNECTION_MESSAGE_CLOSE:
            {
                TcpConnection* conn = reinterpret_cast<TcpConnectionMessage*>(msg)->connection;
                closed_connections_.push(conn);
            }
            break;
    }

    Server::OnMessage(msg);
}

void TcpServer::OnNewConnection(TcpConnection *conn) {
    if (conn) 
	{
		LOG(INFO) << "OnNewConnection TcpConnection " << conn << " SessionId " << conn->GetSessionId(); 
        new_connections_.push(conn);
    }
}

new_connections_queue* TcpServer::new_connections() {
    return &new_connections_;
}

closed_connections_queue* TcpServer::closed_connections() {
    return &closed_connections_;
}

active_connections_list* TcpServer::active_connections() {
    return &active_connections_;
}


//连接断开时处理的逻辑。
void  TcpServer::ProcessConnClosed(TcpConnection* conn, std::map<TcpConnection*, std::set<GameService*>> *connToGameService){
    auto it = active_connections_.find(conn);
    if (it != active_connections_.end()) { 
        active_connections_.erase(it);
        if (connToGameService != nullptr && connToGameService->find(conn) != connToGameService->end()) {
            std::set<GameService*> gameServices = (*connToGameService)[conn];
            connToGameService->erase(conn);
            //按依赖关系调用OnConnClosed
            ServiceManager* manager = ServiceManager::instance();
            std::list<std::string> orders = manager->GetSort();
            for (auto iter = orders.rbegin(); iter != orders.rend(); ++iter) {
                GameService* gameService = manager->LocateService<GameService>(*iter);
                if (gameService != nullptr && gameServices.find(gameService) != gameServices.end()) {
                    LOG(INFO) << "[TcpServer::ProcessConnClosed] " << *iter << " TcpConnection  " << conn;
                    gameService->OnConnClosed(conn);
                }
            }
        }

        //取消conn->OnClosed()调用,依赖TcpServer的Closed消息处理,参考ProcessConnections
        //如果调用conn->OnClosed(),uv_close()将没有机会执行，会导致TcpConnection已经释放,uv_close()没有执行,在uv_run里可能coredump。
        //类似这样的错误：
        //#1  0x000000000041b5c7 in uv__read(stream = stream@entry = 0x7f19583c90e0) at .. / src / unix / stream.c:1020
        //#2  0x000000000041bf48 in uv__stream_io(loop = <optimized out>, w = 0x7f19583c9150, events = <optimized out>) at .. / src / unix / stream.c:1115
        //#3  0x0000000000420492 in uv__io_poll(loop = loop@entry = 0x27198d0, timeout = -1) at .. / src / unix / linux - core.c:211
        //#4  0x0000000000417ce4 in uv_run(loop = 0x27198d0, mode = <optimized out>) at .. / src / unix / core.c:317
        //conn->OnClosed();
        //wait_delete_connections_list_.push_back(conn);
    }
}

void TcpServer::ProcessConnections(std::map<TcpConnection*, std::set<GameService*>> *connToGameService/* = nullptr*/) {
    // process new connctions.
    {
        new_connections_.swap();
        while (!new_connections_.empty()) {
            TcpConnection* conn = nullptr;
            if (!new_connections_.pop(&conn)) {
                break;
            }
            
            if (conn) {
                active_connections_.insert(conn);
            }
        }
    }
    
    // process closed connections.
    {
        closed_connections_.swap();
        
        while (!closed_connections_.empty()) {
            TcpConnection* conn = nullptr;
            if (!closed_connections_.pop(&conn)) {
                break;
            }
        
            // remove from connections.
            if (conn  && !conn->IsClosed()) {
                ProcessConnClosed(conn, connToGameService); 

                //close时并不delete掉conn对象，防止之后收到该conn的read或write消息造成core down，延迟5分钟删除
                conn->OnClosed();
                wait_delete_connections_list_.push_back(conn);
                    //connection_deletor_(conn);
                }
            }
        //处理活跃连接的KeepAlive
        ProcessConnnectionsKeepAlive();

        //处理延迟删除的conn
        ProcessWaitDeleteConnections();

     }
}


//处理连接活的连接,KeepAlive
int32_t TcpServer::ProcessConnnectionsKeepAlive()
{
	static time_t   last_update_time = time(nullptr); 
	time_t now = time(nullptr);
	//loop 10s 更新一次
	if (now <= last_update_time + 10 )
	{
		return  0; 
	}
	last_update_time = now; 

	auto iter = active_connections_.begin(); 
	for (; iter != active_connections_.end();)
	{
		auto iter_back = iter++;
		TcpConnection* tcp_connection = (*iter_back); 
		if ((tcp_connection->GetKeepAliveTime() + 60 * 1) <= now)
		{
			//关闭连接
			LOG(ERROR) << tcp_connection << " TcpConnection ,ProcessConnnectionsKeepAlive, Not Alive " ;
			tcp_connection->Close(); 
		}
	}

	return 0;
}

void TcpServer::ProcessWaitDeleteConnections()
{
	static time_t   last_update_time2 = time(nullptr);
	time_t now = time(nullptr);
	//loop 30s 更新一次
	if (now <= last_update_time2 + 30)
	{
		return;
	}
	last_update_time2 = now;

	auto it = wait_delete_connections_list_.begin();
	for (; it != wait_delete_connections_list_.end(); )
	{
		auto iterBack = it++; 
		if ((*iterBack)->closing_time_ + kDeleteInterval <= now)
		{
			connection_deletor_(*iterBack);
			wait_delete_connections_list_.erase(iterBack); 
		}
	}

	/*
	auto it = wait_delete_connections_list_.begin();
	time_t now = time(nullptr);
	for (; it != wait_delete_connections_list_.end(); ++it)
	{
		if ((*it)->closing_time_ + kDeleteInterval <= now)
		{
			connection_deletor_(*it);
			*it = nullptr;
		}
		else
		{
			break;
		}
	}
	wait_delete_connections_list_.erase(wait_delete_connections_list_.begin(), it);
	*/
}

bool TcpServer::OnShutDown()
{
/*    if (new_connections_) {
        delete new_connections_;
        new_connections_ = nullptr;
    }
*/
    assert(closed_connections_.size() == 0);
    closed_connections_.swap();
    assert(closed_connections_.size() == 0);

    auto wait_delete_it = wait_delete_connections_list_.begin();
    for (; wait_delete_it != wait_delete_connections_list_.end();)
    {
        auto iterBack = wait_delete_it++;
        connection_deletor_(*iterBack);
    }
    wait_delete_connections_list_.clear();

    auto active_it = active_connections_.begin();
    for (; active_it != active_connections_.end();)
    {
        auto iterBack = active_it++;
        connection_deletor_(*iterBack);
    }
    active_connections_.clear();

	return true;
}
