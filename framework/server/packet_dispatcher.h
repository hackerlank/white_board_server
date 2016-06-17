/*
 * packet_dispatcher.h
 *
 *  Created on: Dec 25, 2013
 *      Author: shshang
 */

#ifndef FRAMEWORK_SERVER_PACKET_DISPATCHER_H_
#define FRAMEWORK_SERVER_PACKET_DISPATCHER_H_

#include <functional>
#include <map>
#include <string>

#include <rapidjson/document.h>

#include "service.h"
#include "transcode_packet.h"

class TcpServer;
class GameService;
class TcpConnection;

class Dispatcher : public Service {
public:
	Dispatcher();
	virtual ~Dispatcher();

public:
	bool RegisterOpcode(std::set<uint32_t> &opcodes, GameService* service);
	bool DeregisterOpcode(std::set<uint32_t> &opcodes);

	//TODO(jszhang):minimal   TcpConnectionHandler
	bool SubscribeConnClosedMessage(GameService* service);
	bool UnsubscribeConnClosedMessage(GameService* service);

public:
	virtual const std::set<std::string> GetDependentServices() const;
	virtual bool IsDependentServicesResolved() const;
	virtual void Inject(std::string& name, Service* service);

	//强制将连接和Service绑定,针对有些操作是服务器主动发起的,其中一些Sevice还未来得及和连接绑定
	virtual	void  BindConnToSerice(TcpConnection* pConnection, GameService* pSerice); 


    //同步处理连接断开 ！！
    virtual void  SyncProcessConnClosed(TcpConnection* pConnection);


public:
	virtual bool Init();
	virtual bool Start();
	virtual bool Stop();
	virtual bool IsRunning();

	void Update();

private:
	TcpServer *server_;
	std::map<uint32_t, GameService*> dispatchRules_;
	std::map<TcpConnection*, std::set<GameService*>> connToGameService_;
	std::set<GameService*> subscribers_;
	bool isRunning_;
};

#endif  // FRAMEWORK_SERVER_PACKET_DISPATCHER_H_
