//
//  tcp_server.h
//  gateway
//
//  Created by Fubai Zhong on 10/5/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_SERVER_TCP_SERVER_H_
#define FRAMEWORK_SERVER_TCP_SERVER_H_

#include <set>
#include <map>
#include <list>
#include <memory>
#include <functional>
#include <boost/lockfree/queue.hpp>

#include "server.h"
#include "double_buffer_queue.h"

class TcpServer;
class TcpConnection;
class GameService;

typedef struct {
    uv_tcp_t tcp;
    TcpServer* server;
} tcp_server_t;

//typedef boost::lockfree::queue<TcpConnection*>      new_connections_queue;
typedef DoubleBufferQueue<TcpConnection*>           new_connections_queue;
typedef DoubleBufferQueue<TcpConnection*>           closed_connections_queue;
typedef std::set<TcpConnection*>                    active_connections_list;

typedef std::list<TcpConnection*>                   wait_delete_connections_list;

typedef std::function<TcpConnection*(Async*, Server*)> connection_creator_func;
typedef std::function<void(TcpConnection*)>         connection_deletor_func;

class TcpServer : public Server {
public:
    explicit TcpServer(int port);
    TcpServer(int port, connection_creator_func connection_creator, connection_deletor_func connection_deletor);
    ~TcpServer();
    
    virtual bool Init();
    virtual bool Start();
    virtual bool Stop();

    virtual bool OnShutDown();

    virtual uv_loop_t* GetClientLoop();

    virtual void ProcessConnections(std::map<TcpConnection*, std::set<GameService*>> *connToGameService = nullptr);

	//yujiding 2014/07/10 删除待关闭的连接
	virtual void ProcessWaitDeleteConnections();

	//处理连接活的连接,KeepAlive
	virtual	 int32_t ProcessConnnectionsKeepAlive();


	//连接断开时处理的逻辑。
    virtual void  ProcessConnClosed(TcpConnection* pConnection, std::map<TcpConnection*, std::set<GameService*>> *connToGameService);

    
public:
    new_connections_queue*      new_connections();
    closed_connections_queue*   closed_connections();
    active_connections_list*    active_connections();
    
protected:
    virtual void OnMessage(Message* msg);
    virtual void OnNewConnection(TcpConnection* conn);

private:
    int                         port_;
    tcp_server_t*               server_;
    new_connections_queue       new_connections_;
    closed_connections_queue    closed_connections_;
    active_connections_list     active_connections_;
    
    connection_creator_func     connection_creator_;
    connection_deletor_func     connection_deletor_;

	wait_delete_connections_list wait_delete_connections_list_;
private:
    friend class TcpServerCallback;
};

#endif  // FRAMEWORK_SERVER_TCP_SERVER_H_
