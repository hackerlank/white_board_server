//
//  tcp_connection.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/6/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include "tcp_connection.h"

#include <stdlib.h>
#include <assert.h>

#include <time.h> 

#include "logging.h"
#include "scope_guard.h"

#include "tcp_server_message_codes.h"
#include "CbFormat.h"

typedef struct : public Message {
    const uint8_t*  data;
    size_t          len;
} WriteMessage;

class TcpConnectionCallback {
public:
    
    static void free_buffer(uint8_t* buf) {
        if (buf) {
            free(buf);
        }
    }
    
    static uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size) {
        char *buf = static_cast<char *>(malloc(suggested_size));
        return uv_buf_init(buf, static_cast<unsigned int>(suggested_size));
    }

    static void on_closed(uv_handle_t* handle) {
        tcp_connection_t* stream = reinterpret_cast<tcp_connection_t*>(handle);
        assert(stream);

        Server* server = stream->connection->server();
        assert(server);
        auto msg = CreateMessage<TcpConnectionMessage>(TCP_CONNECTION_MESSAGE_CLOSE);
        msg->connection = stream->connection;
        server->PostMessage(msg);
    }
    
    static void do_close(uv_handle_t* handle) {
        tcp_connection_t* stream = reinterpret_cast<tcp_connection_t*>(handle);
        assert(stream);
        
        if (stream->connection->running_) {
            stream->connection->running_ = false;
            uv_close(handle, on_closed);
        }
    }

    static void do_write(uv_write_t *req, int status) {
        ON_SCOPE_EXIT([req] {
            if (req) {
                if (req->data) {
                    free(req->data);
                }
                tcp_connection_t* stream = reinterpret_cast<tcp_connection_t*>(req->handle);
                assert(stream);
                stream->connection->write_req_set_.erase(req);
                free(req);
            }
        });

        if (status == -1) {
            LOG(ERROR) << "Write error: " << uv_err_name(uv_last_error(req->handle->loop));
            do_close(reinterpret_cast<uv_handle_t*>(req->handle));
            return;
        }
    }

    static void do_read(uv_stream_t *client, ssize_t nread, uv_buf_t buf) {
        if (nread == -1) {
             if (uv_last_error(client->loop).code != UV_EOF) {
                 LOG(ERROR) << " TcpConnectionCallback Read error: " << uv_err_name(uv_last_error(client->loop));
             }
 
             free(buf.base);
             
             do_close(reinterpret_cast<uv_handle_t*>(client));
             return;
        }
        
        tcp_connection_t* stream = reinterpret_cast<tcp_connection_t*>(client);
        assert(stream);

        stream->connection->OnRead(reinterpret_cast<uint8_t*>(buf.base), nread);
    }
};

int64_t     TcpConnection::session_index_ = 0;

TcpConnection::TcpConnection(Async* async_owner, Server* server) : closing_time_(0)
	, last_keep_alive_time(time(nullptr))
	, async_owner_(async_owner)
	, server_(server)
    , remote_port_(0)
    , running_(false)
    , reading_(false)
	, closing_(false)
	, buf_(nullptr)
{
    Copy(*async_owner);
    stream_.connection = this;

    uv_tcp_init(loop_, &stream_.tcp);
}

TcpConnection::~TcpConnection() {
    LOG(DEBUG) << "Tcpconnection::~Tcpconnection";
 
    uint8_t* data = nullptr;
    while (Read(&data) > 0) {
        Free(data);
    }
    if (buf_ != nullptr) {
    	free(buf_);
    }
    
    for(uv_write_t * req : write_req_set_){
        if(req){
            if(req->data){
                free(req->data);
                
                LOG(DEBUG) << "Tcpconnection::~Tcpconnection  free data";
            }
            LOG(DEBUG) << "Tcpconnection::~Tcpconnection  free req";
            free(req);
        }
    }
}

uv_tcp_t* TcpConnection::stream() {
    return &stream_.tcp;
}

Server* TcpConnection::server() {
    return server_;
}

std::string TcpConnection::remote_address() const {
    return remote_address_;
}

int32_t TcpConnection::remote_port() const {
    return remote_port_;
}

std::string TcpConnection::GetSessionId()  
{
	if (session_id.empty())
	{
		session_index_++; 
		char session[10] = { '\0' };
		snprintf(session, sizeof(session), "%ld", session_index_);
		session_id =  std::string(remote_address_).append(":").append(session);
		return session_id; 
	}
	else
	{
		return session_id; 
	}	
}

uint8_t* TcpConnection::Malloc(size_t size) {
    return static_cast<uint8_t*>(malloc(size));
}

uint8_t* TcpConnection::Realloc(uint8_t *data, size_t size) {
    return static_cast<uint8_t*>(realloc(static_cast<void*>(data), size));
}

void TcpConnection::Free(uint8_t *data) {
    TcpConnectionCallback::free_buffer(data);
}

bool TcpConnection::Init() {
    struct sockaddr_in name;
    int namelen = sizeof(name);
    if (uv_tcp_getpeername(&stream_.tcp, (struct sockaddr*)&name, &namelen) == 0) {
        char addr[16];
        uv_inet_ntop(AF_INET, &name.sin_addr, addr, sizeof(addr));
        remote_address_ = addr;
        remote_port_ = ntohs(name.sin_port);
    }
    
    LOG(INFO) << " TcpConnection " << this << " from " << remote_address_ << " port :" << remote_port_;
    
    running_ = true;
    
    return true;
}

void TcpConnection::OnMessage(Message *msg) {
    ON_SCOPE_EXIT([&] { DestroyMessage(msg); });
    
	//yujiding 2014/07/10 防止close后的read和write
	if (closing_)
	{
		return;
	}

    switch (msg->code) {
        case TCP_CONNECTION_MESSAGE_READ:
        {
		    //LOG(INFO) << this << " TcpConnection::OnMessage, StartRead  "; 
            StartRead();
            break;
        }
        case TCP_CONNECTION_MESSAGE_WRITE:
        {
            auto write_msg = reinterpret_cast<const WriteMessage*>(msg);
            assert(write_msg);
            if (write_msg) 
			{
				//LOG(INFO) << this << " TcpConnection::OnMessage, Write DataSize " << write_msg->len  << " : " << CbUtility::byteArrayToString((uint8_t* )write_msg->data, write_msg->len);
                Write(write_msg->data, write_msg->len);
            }
            break;
        }
        case TCP_CONNECTION_MESSAGE_CLOSE:
        {
            if (running_) {
                auto handle = reinterpret_cast<uv_handle_t*>(&stream_.tcp);
                TcpConnectionCallback::do_close(handle);
            }
            break;
        }
        default:
            LOG(WARNING) << "message code " << msg->code << " has no process handler.";
            break;
    }
}

void TcpConnection::StartRead() {
    int err = uv_read_start(reinterpret_cast<uv_stream_t*>(&stream_.tcp), &TcpConnectionCallback::alloc_buffer,
        &TcpConnectionCallback::do_read);
    if (err == 0) {
		LOG(DEBUG) << this << " TcpConnection::OnMessage, uv_read_start  ";
        reading_ = true;
    } else {
        LOG(WARNING) << this << " TcpConnection:OnMessage, uv_read_start err " << err;
        reading_ = false;
    }
}

//uv-loop线程调用
void TcpConnection::OnRead(uint8_t *data, size_t len) 
{
    tcp_connection_binary_data_t item {.data = data, .len = len};
    bool ret = read_queue_.push(item);
    assert(ret);

	//LOG(INFO) << this << " TcpConnection::OnRead  PushQueue , DataSize " << len <<  " ,DataPtr "  <<  data << " :  "<< CbUtility::byteArrayToString(data,len)  ;

    if (read_queue_.wasFull()) 
	{
        if (uv_read_stop(reinterpret_cast<uv_stream_t*>(&stream_.tcp)) == 0)
		{
            reading_ = false;
            LOG(WARNING) << this << " TcpConnection::OnRead , Queue is Full, StopSuccess ";
        }
        else
        {
            //缓冲区满 & 停止读失败时需要关闭连接。
            LOG(WARNING) << this << " TcpConnection::OnRead , Queue is Full,StopFailed, Need Close Connection ";
            if (running_) {
                auto handle = reinterpret_cast<uv_handle_t*>(&stream_.tcp);
                TcpConnectionCallback::do_close(handle);
            }
        }
    }
}

void TcpConnection::SetKeepAliveTime(time_t last_keep_alive_time)
{
    this->last_keep_alive_time = last_keep_alive_time;
}
time_t TcpConnection::GetKeepAliveTime()
{
    return this->last_keep_alive_time;
}

//主线程调用
size_t TcpConnection::Read(uint8_t **data) {
    tcp_connection_binary_data_t item;
    if (read_queue_.pop(item)) {
        *data = item.data;
        if (!reading_)
        {
            LOG(WARNING) << this << " TcpConnection::Read , CreateMessage(TCP_CONNECTION_MESSAGE_READ), flag is " << reading_;
            auto msg = CreateMessage<Message>(TCP_CONNECTION_MESSAGE_READ);
            PostMessage(msg);
        }

        return item.len;
    } else {
        *data = nullptr;
        return 0;
    }
}



void TcpConnection::Write(const EndpointTranscode &transcoder) {
    if (closing_){
        return;
    }

	BinaryData *message = transcoder.Encode();
	uint8_t *messageData = reinterpret_cast<uint8_t*>(malloc(message->offset));
	memcpy(messageData,message->data,message->offset);
	Write(messageData, message->offset);
	free(message);
	message = nullptr;
}

void TcpConnection::Write(const uint8_t *data, size_t len) {
    if (!IsInAsyncThread()) {
        auto msg = CreateMessage<WriteMessage>(TCP_CONNECTION_MESSAGE_WRITE);
        msg->data = data;
        msg->len = len;
        PostMessage(msg);
        return;
    }
    
    uv_write_t *req = static_cast<uv_write_t *>(malloc(sizeof(uv_write_t)));
    req->data = reinterpret_cast<void*>(const_cast<uint8_t*>(data));
    
    uv_buf_t buf {.base = reinterpret_cast<char*>(const_cast<uint8_t*>(data)), .len = len};
    uv_write(req, reinterpret_cast<uv_stream_t*>(&stream_.tcp), &buf, 1, &TcpConnectionCallback::do_write);

    write_req_set_.insert(req);
	//assert(ret == 0);
}

void TcpConnection::BufCat(uint8_t* data, size_t dataLen) {
	if (buf_ == nullptr) {
		buf_ = reinterpret_cast<BinaryData*>(calloc(1, BUFINITLEN + sizeof(BinaryData)));
		buf_->size = BUFINITLEN;
		buf_->offset = 0;
	}
	size_t bufSize = buf_->size;
	if (buf_->offset + dataLen > bufSize)
	{
		do 
		{
			bufSize *= FACTOR;
		}
		while (buf_->offset + dataLen > bufSize);
		BinaryData *bufTmp =  reinterpret_cast<BinaryData*>(realloc(buf_, bufSize + sizeof(BinaryData)));
		if (bufTmp == nullptr) {
			LOG(INFO) << "memory is not enough";
			return;
		}
		buf_ = bufTmp;
		buf_->size = bufSize;
	}
	memcpy(buf_->data + buf_->offset, data, dataLen);
	buf_->offset = buf_->offset + dataLen;
}

void TcpConnection::BufClear(size_t bufOffset) 
{
	//modify by hdchen
	memmove(buf_->data, buf_->data + buf_->offset, bufOffset - buf_->offset);
	buf_->offset = bufOffset - buf_->offset;

    //buf_ = reinterpret_cast<BinaryData*>(realloc(buf_, buf_->offset + sizeof(BinaryData)));
	/*
	//出现内存区域重叠时,出现内存数据乱掉的情况
	memcpy(buf_->data, buf_->data + buf_->offset, bufOffset - buf_->offset);
	buf_->offset = bufOffset - buf_->offset;
	*/
}


size_t TcpConnection::Read(EndpointTranscode& transcoder)
{
	//modify by hdchen,保证优先读取完成的数据包
	size_t actualLength = 0;
	bool isEnough = transcoder.IsDataEnough(buf_, &actualLength,this);
	if (isEnough)
	{
		size_t bufOffset = buf_->offset;
		buf_->offset = actualLength;
		//LOG(INFO) << this << " TcpConnection::Read, Buffer "  <<  buf_ <<  " , DataEnoughSize " << actualLength << " , BufferOffsetSize " << bufOffset  << " ,BufferSize  "  <<  buf_->size  << " BufferHexStr : " << CbUtility::byteArrayToString(buf_->data, actualLength);
		transcoder.Decode(buf_);
		BufClear(bufOffset);

		//TODO(houdao),actualLength 小于4字节的包:连续收到这样的包应该断开该连接 ！！！
		//LOG(INFO) << this << " TcpConnection::Read, DataEnough After, LeftSize, "  << buf_->offset  << " : " << CbUtility::byteArrayToString(buf_->data, buf_->offset);

		return actualLength;
	}
	else
	{
		uint8_t *data = nullptr;
		size_t dataLen = Read(&data);
		if (dataLen == 0)
		{
			return 0;
		}

		//LOG(INFO) << this << " TcpConnection::Read  FromQueue , DataSize  " <<  dataLen << " , DataPtr " << data <<  " : " <<  CbUtility::byteArrayToString(data, dataLen); 

		BufCat(data, dataLen);
		free(data);
		data = nullptr;
		return  0; 
	}
	/*
	uint8_t *data = nullptr;
	size_t dataLen = Read(&data);
	if (dataLen == 0) 
	{
		return 0;
	}
	BufCat(data, dataLen);
	free(data);
	data = nullptr;
	size_t actualLength = 0;

	bool isEnough = transcoder.IsDataEnough(buf_, &actualLength);
	if (isEnough)
	{
		size_t bufOffset = buf_->offset;
		buf_->offset = actualLength;
		transcoder.Decode(buf_);
		BufClear(bufOffset);
		return actualLength;
	} 
	else 
	{
		return 0;
	}
	*/
}



//连接是否关闭
bool  TcpConnection::IsClosed()
{
    return  closing_; 
}


void TcpConnection::OnClosed() {
    LOG(INFO) <<  this <<  " TcpConnection " << this << " closed.";
	closing_ = true;
	if (closing_time_ == 0)
	{
		closing_time_ = time(nullptr);
	}
}

void TcpConnection::Close() {
    if (running_) {
        auto msg = CreateMessage<Message>(TCP_CONNECTION_MESSAGE_CLOSE);
        PostMessage(msg);
    }
}


