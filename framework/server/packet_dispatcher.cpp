/*
 * packet_dispatcher.cpp
 *
 *  Created on: Dec 25, 2013
 *      Author: shshang
 */

#include "packet_dispatcher.h"

#include "logging.h"
#include "service_manager.h"
#include "game_service.h"
#include "tcp_server.h"
#include "game_server_service.h"
#include "transcode_protobuf.h"
#include "CbFormat.h"

#define PROCESS_PACKET_COUNT_PER_LOOP 1

/*
 * don't need configure
 */
Dispatcher::Dispatcher() : server_(nullptr), isRunning_(false) {
}

Dispatcher::~Dispatcher() {

}

bool Dispatcher::RegisterOpcode(std::set<uint32_t> &opcodes, GameService* service) {
	if (service != nullptr) {
		for (auto it = opcodes.begin(); it != opcodes.end(); ++it) {
			if (dispatchRules_.find(*it) == dispatchRules_.end()) {
				dispatchRules_[*it] = service;
			} else {
				LOG(ERROR) << "Opcode Conflict, opcode(" << *it << ") has been registered by other game service";
				return false;
			}
		}
		return true;
	} else {
		LOG(ERROR) << "RegisterOpcode : register null game service doesn't make sense";
		return false;
	}
}

bool Dispatcher::DeregisterOpcode(std::set<uint32_t> &opcodes) {
	for (auto it = opcodes.begin(); it != opcodes.end(); ++it) {
		if (dispatchRules_.find(*it) != dispatchRules_.end()) {
			dispatchRules_.erase(*it);
		}
	}
	return true;
}

bool Dispatcher::SubscribeConnClosedMessage(GameService* service) {
	if (service != nullptr) {
		subscribers_.insert(service);
		return true;
	} else {
		LOG(ERROR) << "SubscribeConnClosedMessage : subscribing for null game service doesn't make sense";
		return false;
	}
}

bool Dispatcher::UnsubscribeConnClosedMessage(GameService* service) {
	//if (subscribers_.find(service) != subscribers_.end()) {
		subscribers_.erase(service);
	//}
	return true;
}

const std::set<std::string> Dispatcher::GetDependentServices() const {
	std::set<std::string> dependency;
	dependency.insert("server");
	return dependency;
}

bool Dispatcher::IsDependentServicesResolved() const {
	return server_ != nullptr;
}

void Dispatcher::Inject(std::string& name, Service* service) {
	if (name == "server") {
		server_ = static_cast<TcpServer*>(service);
	}
}

bool Dispatcher::Init() {
	return true;
}

bool Dispatcher::Start() {
	isRunning_ = true;
	return true;
}

bool Dispatcher::Stop() {
	isRunning_ = false;
	dispatchRules_.clear();
	connToGameService_.clear();
	subscribers_.clear();
	return true;
}

bool Dispatcher::IsRunning() {
	return isRunning_;
}

//强制将连接和Service绑定,针对有些操作是服务器主动发起的,其中一些Sevice还未来得及和连接绑定
void  Dispatcher::BindConnToSerice(TcpConnection* pConnection, GameService* pSerice)
{
	connToGameService_[pConnection].insert(pSerice);
}

//同步处理连接断开 ！！
void  Dispatcher::SyncProcessConnClosed(TcpConnection* pConnection){
    pConnection->Close();
    server_->ProcessConnClosed(pConnection, &connToGameService_); 
}


void Dispatcher::Update() {
// 	static std::map<uint32_t, int64_t> time_map;
// 	static std::map<uint32_t, int32_t> time_num_map;
// 	static int32_t time_map_num;

    //process session's lifecycle
	int64_t  t1 = CbUtility::get_start_time(); 
    server_->ProcessConnections(&connToGameService_);
	int64_t  t2 = CbUtility::get_start_time(); 
	//LOG(INFO) << "[Time]dispatcher  process connection " << t2 - t1;
    //process request
    auto active_connections = server_->active_connections();

	//LOG(ERROR) << " Active Connections " << active_connections->size(); 
    for (auto it = active_connections->begin(); it != active_connections->end(); ) 
	{
        //  在强制登录消息接收时候，会导致iterBack 被earse。
        auto iterBack = it++; 
        uint16_t count = 0;
		 PacketTranscode packet;
        while (count < PROCESS_PACKET_COUNT_PER_LOOP && ((*iterBack)->Read(packet)) > 0) 
		{
        	//dispatch packet
        	uint32_t opcode = packet.GetOpcode();
			if (opcode == 3)
			{
				//心跳包
                (*iterBack)->SetKeepAliveTime(time(nullptr));

				continue; 
			}


        	auto iter = dispatchRules_.find(opcode);
        	if (iter != dispatchRules_.end()) 
			{
                LOG(DEBUG) << "Dispatcher Get Packet, Opcode is " << opcode << " TcpConnection " << (*iterBack);
        		GameService *service = iter->second;
                connToGameService_[*iterBack].insert(service);

                TcpConnection*  connection = *iterBack; 
                //  在强制登录消息接收时候，会导致iterBack 被earse。
				ProtobufTranscode proto_packet;
				proto_packet.FromPacketTranscode(packet);
				service->OnRecvPacket(*iterBack, proto_packet);
				if (!packet.IsGood())
				{
                    LOG(ERROR) << " TcpConnection " << connection << " OnRecvPacket Not Good ";
				}
        	} 
			else 
			{
        		//LOG(ERROR) << "Opcode(" << opcode << ") hasn't been registered";
        		GameServerService *game_server_service = ServiceManager::instance()->LocateService<GameServerService>("GameServerService");
        		if (game_server_service == nullptr) {
					PacketTranscode resp;
					game_server_service->HandleUnknowPacket(&resp, opcode);
					//it->Write(dynamic_cast<EndpointTranscode>resp);
        		}
        	}
// 			int64_t t_end = CbUtility::get_start_time();
// 			time_map[opcode] += (t_end - t_begin);
// 			time_num_map[opcode] += 1;

        	++count;
        }
    }
	int64_t  t3 = CbUtility::get_start_time();
    //LOG(INFO) << "[Time]dispatcher  service revicepacket" << t3 - t2;
    if(t3 - t1 > 100)
    {
	    LOG(DEBUG) << "[Time]dispatch update > 100, process connection" << t2 - t1 << " service recv packet " << t3 - t2;

// 		for (auto it = time_map.begin(); it != time_map.end(); ++it)
// 		{
// 			auto it2 = time_num_map.find(it->first);
// 			LOG(ERROR) << "[Time] opcode" << it->first << " times " << it2->second << " cost " << it->second;
// 		}

	}	
// 	time_map.clear();
// 	time_num_map.clear();
}
