/*
 * rabbitmq_client.h
 *
 *  Created on: Dec 3, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_SERVER_RABBITMQ_CLIENT_H_
#define FRAMEWORK_SERVER_RABBITMQ_CLIENT_H_
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <rapidjson/document.h>
#include "logging.h"
#include "double_buffer_queue.h"

#define QUEUEMAXSIZE 100
#define SLEEPTIME 100

struct RMQCQueueMember
{
	RMQCQueueMember()
	{
		Reset(); 	
	}
	void  Reset()
	{
		memset(this, 0, sizeof(RMQCQueueMember));
	}
	char write_exchange_name[100]; 
	char write_bind_key[100]; 
	size_t len;
	uint64_t delivery_tag;
	char data[0];
};




class RabbitMQClient {
public:
	explicit RabbitMQClient(rapidjson::Value &conf);
	~RabbitMQClient();

	std::string Read(uint64_t& delivery_tag);
	bool Write(const std::string &msg, std::string write_exchange_name = "", std::string write_bind_key = "");

	bool Init();
	bool InitRabbitMQClient(); 
	bool InitProducerRabbitMQClient();

	bool Start();

	bool StartRabbitMQClient(); 
	bool Stop();

	//消费者
	void ReadRabbitMessageBlock(); 
	void ReadRabbitMessageNoBlock();


	//生产者
	void WriteRabbitMessageNoBlock(); 


	int32_t Ack(uint64_t delivery_tag);
	int32_t Reject(uint64_t delivery_tag);

public:
	std::string GetWriteBindingKey();
	std::string GetWriteExchange();
    void SetReadBindKey(const std::string& bind_key);
	std::string GetReadBindKey();
	std::string GetReadExchange();
protected:
	bool InitConfig();
	bool IsRunning();



private:

	int64_t		start_client_time_; 
	bool		need_reconnect_; 
	rapidjson::Value conf_;

	//消费者的参数
	amqp_connection_state_t conn_;
	amqp_socket_t *socket_;

	//生产者的参数
	amqp_connection_state_t producer_conn_;
	amqp_socket_t *producer_socket_;
	bool		producer_need_reconnect_;


	std::string hostname_;
	uint16_t port_;
	uint16_t channel_;

	std::string writeExchange_;
	std::string writeBindingkey_;
	std::string writeQueuename_;

	std::string readExchange_;
	std::string readBindingkey_;
	std::string readQueuename_;

	std::string username_;
	std::string password_;
	
	std::thread *listenThread_;
	std::thread *sendBytesThread_;
	bool isRunning_;
	timeval timeout_;

	DoubleBufferQueue<RMQCQueueMember *> readQueue_;
	DoubleBufferQueue<RMQCQueueMember *> writeQueue_;
	amqp_bytes_t readQueueRep_;
};

#endif  // FRAMEWORK_SERVER_RABBITMQ_CLIENT_H_
