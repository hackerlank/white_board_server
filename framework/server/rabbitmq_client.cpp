/*
 * rabbitmq_client.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: jszhang
 */


#include "rabbitmq_client.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <amqp.h>
#include "CbFormat.h"
//#include <amqp_private.h>
//#include <amqp_socket.h>


/*
void die_on_error(int x, char const *context) {
	if (x < 0) {
		char *errstr = amqp_error_string(-x);
		fprintf(stderr, "%s: %s\n", context, errstr);
		free(errstr);
		exit(1);
	}
}


void die_on_amqp_error(amqp_rpc_reply_t x, char const *context) {
	switch (x.reply_type) {
	case AMQP_RESPONSE_NORMAL:
		return;


	case AMQP_RESPONSE_NONE:
		fprintf(stderr, "%s: missing RPC reply type!\n", context);
		break;


	case AMQP_RESPONSE_LIBRARY_EXCEPTION:
		fprintf(stderr, "%s: %s\n", context, amqp_error_string(x.library_error));
		break;


	case AMQP_RESPONSE_SERVER_EXCEPTION:
		switch (x.reply.id) {
		case AMQP_CONNECTION_CLOSE_METHOD: {
											   amqp_connection_close_t *m = (amqp_connection_close_t *)x.reply.decoded;
											   fprintf(stderr, "%s: server connection error %d, message: %.*s\n",
												   context,
												   m->reply_code,
												   (int)m->reply_text.len, (char *)m->reply_text.bytes);
											   break;
		}
		case AMQP_CHANNEL_CLOSE_METHOD: {
											amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
											fprintf(stderr, "%s: server channel error %d, message: %.*s\n",
												context,
												m->reply_code,
												(int)m->reply_text.len, (char *)m->reply_text.bytes);
											break;
		}
		default:
			fprintf(stderr, "%s: unknown server error, method id 0x%08X\n", context, x.reply.id);
			break;
		}
		break;
	}


	exit(1);
}


static void dump_row(long count, int numinrow, int *chs) {
	int i;


	printf("%08lX:", count - numinrow);


	if (numinrow > 0) {
		for (i = 0; i < numinrow; i++) {
			if (i == 8)
				printf(" :");
			printf(" %02X", chs[i]);
		}
		for (i = numinrow; i < 16; i++) {
			if (i == 8)
				printf(" :");
			printf("   ");
		}
		printf("  ");
		for (i = 0; i < numinrow; i++) {
			if (isprint(chs[i]))
				printf("%c", chs[i]);
			else
				printf(".");
		}
	}
	printf("\n");
}


static int rows_eq(int *a, int *b) {
	int i;


	for (i = 0; i < 16; i++)
	if (a[i] != b[i])
		return 0;


	return 1;
}


void amqp_dump(void const *buffer, size_t len) {
	unsigned char *buf = (unsigned char *)buffer;
	long count = 0;
	int numinrow = 0;
	int chs[16];
	int oldchs[16];
	int showed_dots = 0;
	int i;


	for (i = 0; i < len; i++) {
		int ch = buf[i];


		if (numinrow == 16) {
			int i;


			if (rows_eq(oldchs, chs)) {
				if (!showed_dots) {
					showed_dots = 1;
					printf("          .. .. .. .. .. .. .. .. : .. .. .. .. .. .. .. ..\n");
				}
			}
			else {
				showed_dots = 0;
				dump_row(count, numinrow, chs);
			}


			for (i = 0; i < 16; i++)
				oldchs[i] = chs[i];


			numinrow = 0;
		}


		count++;
		chs[numinrow++] = ch;
	}


	dump_row(count, numinrow, chs);


	if (numinrow != 0)
		printf("%08lX:\n", count);
}

*/


/*
 *	format : json
 *	{
 *		"ip": "172.16.7.216",
 *		"port": 5672,
 *		"channel": 1,
 *		"writeExchange": "InMsgExchange",
 *		"writeBindindkey": "In",
 *		"writeQueuename": "InMsgQueue",
 *		"readExchange": "OutMsgExchange",
 *		"readBindingkey": "Out",
 *		"readQueuename": "OutMsgQueue",
 *		"username": "guest"
 *		"password": "guest"
 *	}
 */
RabbitMQClient::RabbitMQClient(rapidjson::Value &conf) :start_client_time_(0), 
need_reconnect_(true), 
conn_(nullptr),
socket_(nullptr), 
producer_conn_(nullptr), 
producer_socket_(nullptr),
producer_need_reconnect_(true),
hostname_(""), 
port_(0), 
channel_(1), 
writeExchange_(""),
writeBindingkey_(""), 
writeQueuename_(""), 
readExchange_(""), 
readBindingkey_(""), 
readQueuename_(""),
username_(""), 
password_(""), 
listenThread_(nullptr), 
sendBytesThread_(nullptr), 
isRunning_(false)
{
	conf_ = conf;

	//rapidjson::StringBuffer buffer;
	//rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	//conf_.Accept(writer);
	//LOG(INFO) << "11111===========================================";
	//LOG(INFO) << " buffer" << buffer.GetString();

	this->readQueue_.swap();
	this->writeQueue_.swap();
	timeout_.tv_sec = 0;
	timeout_.tv_usec = 50000;
	LOG(INFO) << "construct rabbitmq";
}




RabbitMQClient::~RabbitMQClient() {
	RMQCQueueMember *pMember = nullptr;
	readQueue_.swap();
	while(readQueue_.pop(&pMember)){
		free(pMember);
	}
	readQueue_.swap();
	while(readQueue_.pop(&pMember)){
		free(pMember);
	}
	writeQueue_.swap();
	while(writeQueue_.pop(&pMember)) {
		free(pMember);
	}
	writeQueue_.swap();
	while(writeQueue_.pop(&pMember)) {
		free(pMember);
	}
	if(listenThread_ != nullptr) {
		free(listenThread_);
	}
	if(sendBytesThread_ != nullptr) {
		free(sendBytesThread_);
	}
}

bool RabbitMQClient::InitConfig() {
	if (!conf_.IsObject()) {	
		LOG(INFO) << "config is not a object";
		return false;
	}
	//rapidjson::StringBuffer buffer;
	//rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	//conf_.Accept(writer);
	//LOG(INFO) << "===========================================333333";
	//LOG(INFO) << " buffer" << buffer.GetString();

	if(!conf_.HasMember("ip") || !conf_["ip"].IsString()) {
		LOG(INFO) << "ip missing or error";
		return false;
	}
	if(!conf_.HasMember("port") || !conf_["port"].IsInt()) {
		LOG(INFO) << "port missing or error";
		return false;
	}
	if(!conf_.HasMember("channel") || !conf_["channel"].IsInt()) {
		LOG(INFO) << "channel missing or error";
		return false;
	}
	if(!conf_.HasMember("writeExchange") || !conf_["writeExchange"].IsString()) {
		LOG(INFO) << "writeExchange missing or error";
		return false;
	}
	if(!conf_.HasMember("writeBindingkey") || !conf_["writeBindingkey"].IsString()) {
		LOG(INFO) << "writeBindingkey missing or error";
		return false;
	}
	if(!conf_.HasMember("writeQueuename") || !conf_["writeQueuename"].IsString()) {
		LOG(INFO) << "writeQueuename missing or error";
		return false;
	}
	if(!conf_.HasMember("readExchange") || !conf_["readExchange"].IsString()) {
		LOG(INFO) << "readExchange missing or error";
		return false;
	}
    if (!conf_.HasMember("readBindingkey") || !conf_["readBindingkey"].IsString()) {
        LOG(INFO) << "readBindingkey missing or error";
        //兼容不配置readBindingkey ，通过SetReadBindKey设置
    }
    else{
        readBindingkey_ = conf_["readBindingkey"].GetString();
    }
	if(!conf_.HasMember("readQueuename") || !conf_["readQueuename"].IsString()) {
		LOG(INFO) << "readQueuename missing or error";
		return false;
	}
	if(!conf_.HasMember("username") || !conf_["username"].IsString()) {
		LOG(INFO) << "username missing or error";
		return false;
	}
	if(!conf_.HasMember("password") || !conf_["password"].IsString()) {
		LOG(INFO) << "password missing or error";
		return false;
	}
	hostname_ = conf_["ip"].GetString();
	port_ = conf_["port"].GetInt();
	channel_ = conf_["channel"].GetInt();
	writeExchange_ = conf_["writeExchange"].GetString();
	writeBindingkey_ = conf_["writeBindingkey"].GetString();
	writeQueuename_ = conf_["writeQueuename"].GetString();
	readExchange_ = conf_["readExchange"].GetString();

	readQueuename_ = conf_["readQueuename"].GetString();
	username_ = conf_["username"].GetString();
	password_ = conf_["password"].GetString();
	LOG(INFO) <<  this << "  RabbitMQClient  init config success";
	return true;
}

bool RabbitMQClient::Init() 
{
	start_client_time_ = CbUtility::getCurLoopTime() + 1000 * 2; 
	if (!InitConfig()) {
		return false;
	}
	return true;
	//return InitRabbitMQClient(); 
}


bool RabbitMQClient::InitProducerRabbitMQClient()
{
	//发送的消息队列(生产者)
	producer_conn_ = amqp_new_connection();

	producer_socket_ = amqp_tcp_socket_new(producer_conn_);

	if (!producer_socket_) 
	{
		LOG(INFO) << this  << " InitProducerRabbitMQClient init failed, creating TCP socket_";
		return false;
	}
	int32_t	state = amqp_socket_open(producer_socket_, hostname_.c_str(), port_);
	if (state) {
		LOG(INFO) << this << " InitProducerRabbitMQClient init failed, opening TCP socket_ :" << state;
		return false;
	}

	if (amqp_login(producer_conn_, "/", 0, 131072, 600, AMQP_SASL_METHOD_PLAIN, username_.c_str(), password_.c_str()).reply_type != AMQP_RESPONSE_NORMAL) 
	{
		LOG(INFO) << this << " InitProducerRabbitMQClient login failed";
		return false;
	}

	amqp_channel_open(producer_conn_, channel_);
	if (AMQP_RESPONSE_NORMAL != amqp_get_rpc_reply(producer_conn_).reply_type) {
		LOG(INFO) << this << " InitProducerRabbitMQClient init failed, Opening channel";
		return false;
	}
	LOG(ERROR) << this << " RabbitMQClient  InitProducerRabbitMQClient init success ";
	return true; 
}


bool RabbitMQClient::InitRabbitMQClient()
{
	conn_ = amqp_new_connection();

	socket_ = amqp_tcp_socket_new(conn_);

	if (!socket_) {
		LOG(INFO) << this << " InitRabbitMQClient  init failed, creating TCP socket_";
		return false;
	}
	int32_t	state = amqp_socket_open(socket_, hostname_.c_str(), port_);
	if (state) {
		LOG(INFO) << this << " init failed, opening TCP socket_ :" << state;
		return false;
	}

	if (amqp_login(conn_, "/", 0, 131072, 600, AMQP_SASL_METHOD_PLAIN, username_.c_str(), password_.c_str()).reply_type != AMQP_RESPONSE_NORMAL) {
		LOG(INFO) << this << " login failed";
		return false;
	}

	amqp_channel_open(conn_, channel_);
	if (AMQP_RESPONSE_NORMAL != amqp_get_rpc_reply(conn_).reply_type) {
		LOG(INFO) << this <<  " init failed, Opening channel";
		return false;
	}

	amqp_exchange_declare(conn_, channel_, amqp_cstring_bytes(readExchange_.c_str()), amqp_cstring_bytes("topic"), 0, 1, amqp_empty_table);
	if (AMQP_RESPONSE_NORMAL != amqp_get_rpc_reply(conn_).reply_type) {
		LOG(INFO) << this <<  " duplicate declare of exchange";
		return false;
	}

	amqp_bytes_t readQueuename = amqp_cstring_bytes(readQueuename_.c_str());
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn_, channel_, readQueuename, 0, 1, 0, 1, amqp_empty_table);
	if(AMQP_RESPONSE_NORMAL != amqp_get_rpc_reply(conn_).reply_type) {
	LOG(INFO)<< "duplicate declare of queue name";
	return false;
	}

	/*
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn_, channel_, amqp_empty_bytes, 0, 0, 0, 1,
		amqp_empty_table);
	if (AMQP_RESPONSE_NORMAL != amqp_get_rpc_reply(conn_).reply_type) {
		LOG(INFO) << this << "duplicate declare of queue name";
		return false;
	}
	*/


	readQueueRep_ = amqp_bytes_malloc_dup(r->queue);
	if (readQueueRep_.bytes == NULL) {
		LOG(INFO) << this << " init failed, Out of memory while copying queue name";
		return false;
	}

	amqp_queue_bind(conn_, channel_, readQueueRep_, amqp_cstring_bytes(readExchange_.c_str()), amqp_cstring_bytes(readBindingkey_.c_str()), amqp_empty_table);
	if (AMQP_RESPONSE_NORMAL != amqp_get_rpc_reply(conn_).reply_type) {
		LOG(INFO) <<  this << " init failed, Binding queue";
		return false;
	}

	//amqp_basic_qos(conn_, channel_, 0, 1, 0);
	amqp_basic_consume(conn_, channel_, readQueueRep_, amqp_empty_bytes, 0, 0, 0, amqp_empty_table);
	if (AMQP_RESPONSE_NORMAL != amqp_get_rpc_reply(conn_).reply_type) {
		LOG(INFO) <<  this << " init failed, Binding queue";
		return false;
	}

	LOG(ERROR) << this << " RabbitMQClient  init success " << " QueueName, " << readQueuename_ << " ,ReadExchange " << readExchange_ << " ,readBindingkey  " << readBindingkey_ << " Channel, " << channel_;
	return true;
}

bool RabbitMQClient::IsRunning() {
	return isRunning_;
}




void   RabbitMQClient::ReadRabbitMessageBlock()
{
	LOG(INFO) << this << " RabbitMQClient  Start SendBytes";
	while (this->IsRunning())
	{
		amqp_connection_state_t& conn = this->conn_;
		amqp_frame_t frame;

		int result = 0;
		amqp_basic_deliver_t *d;
		amqp_basic_properties_t *p;
		size_t body_target = 0;
		size_t body_received = 0;

		amqp_maybe_release_buffers(conn);

		LOG(DEBUG) << this << " RabbitMQClient, amqp_simple_wait_frame " << conn;
		result = amqp_simple_wait_frame(conn, &frame);
		if (result < 0)
		{
			LOG(ERROR) << this << " RabbitMQClient, amqp_simple_wait_frame 1 , Result  " << result;
			break;
		}

		LOG(INFO) << this << " RabbitMQClient, Frame Type " << frame.frame_type << " ,Frame Channel " << frame.channel;

		if (frame.frame_type != AMQP_FRAME_METHOD)
		{
			continue;
		}

		LOG(INFO) << this << " RabbitMQClient, MethodID   " << frame.payload.method.id << " , MethodName " << amqp_method_name(frame.payload.method.id);

		if (frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD)
		{
			if (frame.payload.method.id == AMQP_CONNECTION_CLOSE_METHOD)
			{
			}
			continue;
		}

		d = reinterpret_cast<amqp_basic_deliver_t *>(frame.payload.method.decoded);

		LOG(INFO) << this << " RabbitMQClient, Delivery " << static_cast<unsigned>(d->delivery_tag) \
            << " ,exchange " << static_cast<char *>(d->exchange.bytes) \
            << " ,routing_key " << static_cast<char *>(d->routing_key.bytes);

		result = amqp_simple_wait_frame(conn, &frame);
		if (result < 0)
		{
			LOG(ERROR) << this << " RabbitMQClient, amqp_simple_wait_frame 2 , Result  " << result;
			break;
		}
		if (frame.frame_type != AMQP_FRAME_HEADER)
		{
			LOG(ERROR) << this << " RabbitMQClient, frame.frame_type  " << frame.frame_type << ",  HeadType " << AMQP_FRAME_HEADER;
			break;
		}
        p = reinterpret_cast<amqp_basic_properties_t *>(frame.payload.properties.decoded);
		if (p->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG)
		{
            LOG(INFO) << this << " RabbitMQClient , Content-type, len " \
                << static_cast<int>(p->content_type.len) << " : " << static_cast<char *>(p->content_type.bytes);
		}

		body_target = frame.payload.properties.body_size;
		body_received = 0;

		LOG(INFO) << this << " RabbitMQClient amqp_simple_wait_frame body Size  " << body_target;

		RMQCQueueMember *pMember = reinterpret_cast<RMQCQueueMember *>(malloc(sizeof(RMQCQueueMember)+body_target));
		if (pMember == nullptr)
		{
			LOG(INFO) << "Memory is not enough";
			return;
		}
		pMember->Reset();

		//int sleep_seconds = 0;
		while (body_received < body_target)
		{
			result = amqp_simple_wait_frame(conn, &frame);
			if (result < 0)
			{
				LOG(ERROR) << this << " RabbitMQClient, amqp_simple_wait_frame 3 , Result  " << result;
				break;
			}
			if (frame.frame_type != AMQP_FRAME_BODY)
			{
				LOG(ERROR) << this << " RabbitMQClient, frame.frame_type  " << frame.frame_type << ",  BodeType " << AMQP_FRAME_BODY;
				break;
			}
			body_received += frame.payload.body_fragment.len;
			assert(body_received <= body_target);
			memcpy(pMember->data + pMember->len, frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
			pMember->len = body_received;

		}
		if (body_received != body_target)
		{
			break;
		}
		LOG(INFO) << this << " RabbitMQClient ,Receive Finish And Begin Push :  " << pMember->data;
		pMember->delivery_tag = d->delivery_tag;
		this->readQueue_.push(pMember);
	}
	LOG(INFO) << this << " RabbitMQClient Stop Listenning";
}



//生产者
void RabbitMQClient::WriteRabbitMessageNoBlock()
{
	LOG(INFO) << this << " RabbitMQClient Start SendBytes";
	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2; /* persistent delivery mode */
	props.content_encoding = amqp_cstring_bytes("UTF-8");
	while (this->IsRunning())
	{
		//重新连接 ！！！
		if (producer_need_reconnect_)
		{
			//关闭连接
			if (producer_conn_ != nullptr)
			{
				amqp_channel_close(producer_conn_, channel_, AMQP_REPLY_SUCCESS);
				amqp_connection_close(producer_conn_, AMQP_REPLY_SUCCESS);
				amqp_destroy_connection(producer_conn_);
			}
			if (InitProducerRabbitMQClient())
			{
				//重新建立连接！！
				producer_need_reconnect_ = false;
			}
		}

		RMQCQueueMember *pMember = nullptr;
		amqp_bytes_t messagebody;
		if (!this->writeQueue_.pop(&pMember))
		{
			this->writeQueue_.swap();
			if (!this->writeQueue_.pop(&pMember)) {
				//LOG(INFO) << "write Queue is empty";
				usleep(SLEEPTIME);
				continue;
			}
		}

		assert(pMember != nullptr);
		messagebody.len = pMember->len;
		messagebody.bytes = pMember->data;

		std::string write_exchange;
		std::string write_bind_key;
		if (pMember->write_exchange_name[0] != 0)
		{
			write_exchange += pMember->write_exchange_name;
		}
		else
		{
			write_exchange += this->writeExchange_.c_str();
		}

		if (pMember->write_bind_key[0] != 0)
		{
			write_bind_key += pMember->write_bind_key;
		}
		else
		{
			write_bind_key += this->writeBindingkey_.c_str();
		}

		LOG(INFO) << this << " RabbitMQClient  write_exchange: " << write_exchange << " , write_bind_key , " << write_bind_key << " , " << pMember->data;

		int nPublicStatus = amqp_basic_publish(this->producer_conn_, channel_, amqp_cstring_bytes(write_exchange.c_str()), amqp_cstring_bytes(write_bind_key.c_str()),0, 0, &props, messagebody); 
		if (nPublicStatus != AMQP_STATUS_OK)
		{
			LOG(ERROR) << this << " RabbitMQClient  send bytes failed, amqp_basic_publish Status  " << nPublicStatus;
			//需要重新连接
			producer_need_reconnect_ = true; 
			//发送失败的需要重发
			this->writeQueue_.push(pMember); 
			usleep(SLEEPTIME);

			continue; 
		}
		LOG(DEBUG) << this << " RabbitMQClient  public Success !! ";

		free(pMember);
	}
	LOG(INFO) << this << " RabbitMQClient Stop SendBytes";
}


void  RabbitMQClient::ReadRabbitMessageNoBlock()
{
	LOG(INFO) << this << " RabbitMQClient Start Listenning";
	while (this->IsRunning()) 
	{
		if (readQueue_.size() >= QUEUEMAXSIZE) 
		{
			usleep(SLEEPTIME);
			continue;
		}
		//重新连接 ！！！
		if (need_reconnect_)
		{	
			//关闭连接
			if (conn_ != nullptr)
			{
				amqp_channel_close(conn_, channel_, AMQP_REPLY_SUCCESS);
				amqp_connection_close(conn_, AMQP_REPLY_SUCCESS);
				amqp_destroy_connection(conn_);
				if (readQueueRep_.bytes != nullptr)
				{
					amqp_bytes_free(readQueueRep_);
				}
			}		
			if (InitRabbitMQClient())
			{
				//重新建立连接！！
				need_reconnect_ = false;		
			}
		}

		amqp_envelope_t envelope;
		amqp_maybe_release_buffers(this->conn_);

		//AMQP_STATUS_OK
		//AMQP_STATUS_SOCKET_ERROR

		amqp_rpc_reply_t  reply = amqp_consume_message(this->conn_, &envelope, &timeout_, 0); 
		if (AMQP_RESPONSE_NORMAL != reply.reply_type) 
		{
			if (reply.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION || reply.reply_type == AMQP_RESPONSE_NONE)
			{
				//重连服务器
				need_reconnect_ = true; 	
				LOG(ERROR) << this << " amqp_consume_message " << reply.reply_type << " ,Status " << reply.library_error;
			}
			else   if (reply.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
			{
				if (reply.library_error != AMQP_STATUS_TIMEOUT)
				{
					if (!need_reconnect_)
					{
						need_reconnect_ = true;
						LOG(ERROR) << this << " amqp_consume_message " << reply.reply_type << ",Status " << reply.library_error;
					}
				}
				else
				{
					//LOG(DEBUG) << this << " amqp_consume_message " << reply.reply_type << ",Status " << reply.library_error;
				}
			}
			continue;
		}

		RMQCQueueMember *pMember = reinterpret_cast<RMQCQueueMember *>(malloc(sizeof(RMQCQueueMember)+envelope.message.body.len));
		if (pMember == nullptr) {
			LOG(INFO) << "Memory is not enough";
			return;
		}
		pMember->len = envelope.message.body.len;
		pMember->delivery_tag = envelope.delivery_tag;
		memcpy(pMember->data, envelope.message.body.bytes, pMember->len);

		LOG(INFO) << this << " RabbitMQClient ,Receive Finish And Begin Push :  " << pMember->data;

		this->readQueue_.push(pMember);
		amqp_destroy_envelope(&envelope);
	}
	LOG(INFO) << this << " RabbitMQClient Stop Listenning";
}



bool RabbitMQClient::StartRabbitMQClient()
{
	LOG(INFO) <<  this << " RabbitMQClient rabbitmq start ";
	isRunning_ = true;

	listenThread_ = new std::thread([this]
	{
		ReadRabbitMessageNoBlock(); 
	}
	);
	sendBytesThread_ = new std::thread([this]{
		WriteRabbitMessageNoBlock(); 
	});
	return true;
}

bool RabbitMQClient::Start() 
{	
	//return StartRabbitMQClient(); 
	return true; 
}

bool RabbitMQClient::Stop() {
	isRunning_ = false;
	listenThread_->join();
	sendBytesThread_->join();

	amqp_channel_close(conn_, channel_, AMQP_REPLY_SUCCESS);
	amqp_connection_close(conn_, AMQP_REPLY_SUCCESS);
	amqp_destroy_connection(conn_);
	if(readQueueRep_.bytes != nullptr) {
		amqp_bytes_free(readQueueRep_);
	}

	amqp_channel_close(producer_conn_, channel_, AMQP_REPLY_SUCCESS);
	amqp_connection_close(producer_conn_, AMQP_REPLY_SUCCESS);
	amqp_destroy_connection(producer_conn_);

	return true;
}

std::string RabbitMQClient::Read(uint64_t& delivery_tag)
{
	//建立连接 RabbitMQ Client连接 & 收发数据线程 
	//在后台进程模式下,result = amqp_simple_wait_frame(conn, &frame)返回-9(AMQP_STATUS_SOCKET_ERROR),发现连接断开。
	//改成 在Update里面去建立连接 & 收发数据线程,则连接正常。

	
	if (start_client_time_ > 0 && start_client_time_ < CbUtility::getCurLoopTime())
	{
		start_client_time_ = 0; 		
		StartRabbitMQClient(); 
		return std::string();
	}
	

	RMQCQueueMember *pMember = nullptr;

	if(!readQueue_.pop(&pMember)) {
		readQueue_.swap();
		if(!readQueue_.pop(&pMember)) {
			return std::string();
		}
	}
	std::string msg(pMember->data, pMember->len);
	delivery_tag = pMember->delivery_tag;
	free(pMember);
	return msg;
}

bool RabbitMQClient::Write(const std::string &msg, std::string write_exchange_name, std::string write_bind_key) {
	RMQCQueueMember *pMember = reinterpret_cast<RMQCQueueMember*>(malloc(sizeof(RMQCQueueMember) + msg.size()));
	if(pMember == nullptr) {
		LOG(INFO) << "Memory is not enough ";
		return false;
	}
	pMember->Reset(); 
	pMember->len = msg.size();
	memcpy(pMember->data, msg.c_str(), pMember->len);

	memcpy(pMember->write_exchange_name, write_exchange_name.c_str(), write_exchange_name.size()),
	memcpy(pMember->write_bind_key, write_bind_key.c_str(), write_bind_key.size()),

	writeQueue_.push(pMember);

	return true;
}

int32_t RabbitMQClient::Ack(uint64_t delivery_tag) {
	return amqp_basic_ack(conn_, channel_, delivery_tag, 0);
}

int32_t RabbitMQClient::Reject(uint64_t delivery_tag) {
	return amqp_basic_reject(conn_, channel_, delivery_tag, 1);
}

std::string RabbitMQClient::GetWriteBindingKey()
{
	return writeBindingkey_;
}

std::string RabbitMQClient::GetWriteExchange()
{
	return writeExchange_;
}
std::string RabbitMQClient::GetReadBindKey()
{
	return readBindingkey_;
}
std::string RabbitMQClient::GetReadExchange()
{
	return readExchange_;
}
void  RabbitMQClient::SetReadBindKey(const std::string& bind_key){
    readBindingkey_ = bind_key;
}
