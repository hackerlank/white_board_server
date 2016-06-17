//
//  tcp_connection.h
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#ifndef FRAMEWORK_SERVER_TCP_CONNECTION_H_
#define FRAMEWORK_SERVER_TCP_CONNECTION_H_

#include <thread>
#include <string>
#include <uv.h>

#include "circularfifo.hpp"
#include "async.h"
#include "server.h"
#include "transcode_endpoint.h"

#ifndef FACTOR
#define FACTOR 2
#endif

#define BUFINITLEN 1

class TcpConnection;

typedef struct {
    uv_tcp_t        tcp;
    TcpConnection*  connection;
} tcp_connection_t;

typedef struct : public Message {
    TcpConnection* connection;
} TcpConnectionMessage;

typedef struct {
    uint8_t*    data;
    size_t      len;
} tcp_connection_binary_data_t;

class TcpConnection : public Async {
public:
    TcpConnection(Async* async_owner, Server* server);
    virtual ~TcpConnection();

public:
    size_t Read(EndpointTranscode& transcoder);
    size_t Read(uint8_t** data);
    void Write(const EndpointTranscode &transcoder);
    void Write(const uint8_t* data, size_t len);
    void Close();

    virtual bool Init();
    virtual void OnClosed();
    
    static uint8_t* Malloc(size_t size);
    static void Free(uint8_t* data);
    static uint8_t* Realloc(uint8_t* data, size_t size);

public:
    uv_tcp_t*   stream();
    Server*     server();
    std::string remote_address() const;
    int32_t     remote_port() const;

    std::string GetSessionId() ;

    //连接是否关闭
    bool        IsClosed();

    void SetKeepAliveTime(time_t last_keep_alive_time);
    time_t GetKeepAliveTime();
protected:
    virtual void OnMessage(Message* msg);

    void StartRead();
    void OnRead(uint8_t* data, size_t len);
    void BufCat(uint8_t* data, size_t dataLen);
    void BufClear(size_t bufOffset);

public:
	//关闭的时间
	time_t			closing_time_;

    std::set<uv_write_t *> write_req_set_; 
    
private:
    //连接保持活跃时间
    time_t			last_keep_alive_time;

	static int64_t     session_index_; 
	std::string		   session_id; 

    tcp_connection_t    stream_;
    Async*              async_owner_;
    Server*             server_;
    CircularFifo<tcp_connection_binary_data_t, 128>    read_queue_;
    std::string         remote_address_;
    int32_t             remote_port_;
    std::atomic<bool>   running_;
    std::atomic<bool>   reading_;

	std::atomic<bool>   closing_;

    BinaryData*         buf_;
    


    friend class TcpConnectionCallback;
    friend class TcpServerCallback;
};

#endif  // FRAMEWORK_SERVER_TCP_CONNECTION_H_
