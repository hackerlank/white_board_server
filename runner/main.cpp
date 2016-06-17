//
//  main.cpp
//  gateway
//
//  Created by Fubai Zhong on 10/8/13.
//  Copyright (c) 2013 Baina. All rights reserved.
//

#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <thread>
#include <chrono>
#include <gflags/gflags.h>
#include "logging.h"
#include "service_manager.h"
#include "game_loop.h"
#include "game_server_service.h"
#include "packet_dispatcher.h"
#include "player_def.h"
#include "account_service.h"
#include "CbFormat.h"
#include "logpluslogger.h"
#include "room_manager.h"
/*
#include "hall_service.h"
#include "gm_service.h"
#include "contests_service.h"
#include "shop_payment_service.h"
#include "caishen_manager.h"
#include "file_monitor.h"
#include "game_script.h"
#include "lua_service.h"
#include "money_tree_service.h"
#include "player_constructor.h"
#include "task_service.h"
#include "CbFormat.h"
#include "zookeeper_service.h"
#include "trade_service.h"
#include "chart_service.h"
#include "game_manager_service.h"
#include "message_service.h"
#include "http_request_handler.h"
#include "http_server.h"
#include "activity_service.h"
#include "wanren_service.h"
#include "global_table_data.h"
#include "reward_data.h"
#include "hundred_table_chart_data.h"
#include "encryption.h"
#include "encrypthelper.h"
#include "oss_log.h"
*/



void init_daemon()
{
	int pid;
	int i;
	if ((pid = fork()) > 0)
		exit(0);
	else if (pid<0)
		exit(1);

	setsid();

	if ((pid = fork())>0)
		exit(0);
	else if (pid < 0)
		exit(1);
	for (i = 0; i < NOFILE; i++)
		close(i);
	umask(0);
}

class OurGameLoop : public GameLoop {
public:
	explicit OurGameLoop(const std::string &config) : GameLoop(config)
	{
	
	}

  	virtual bool Init() 
    {

		srand( time(nullptr));
  		service_manager_->Register("Dispatcher", new Dispatcher());
  		
  		if (config_.IsObject() && config_.HasMember("tcp_port") && config_["tcp_port"].IsInt()) {
  			service_manager_->Register("server", new TcpServer(config_["tcp_port"].GetInt()));
  		} else {
  			LOG(ERROR) << "Please check the config has an object member 'Dispatcher'";
  			return false;
  		}
  		if (config_.IsObject() && config_.HasMember("game_service") && config_["game_service"].IsObject()) 
        {
			//account
			LOG(INFO) <<  "[Account]";
			if (config_["game_service"].HasMember("account") && config_["game_service"]["account"].IsObject()) 
            {
				AccountService *service = new AccountService(config_["game_service"]["account"]);
				service_manager_->Register("Account", service);
				gameServices_.insert(service);
			} else 
            {
				LOG(ERROR) << "Please check the config has an object member 'account' in 'game_service'";
				return false;
			}
  		} else 
        {
  			LOG(ERROR) << "Please check the config has an object member 'game_service'";
  			return false;
  		}

   		bool res = service_manager_->Init();	
   		if(res == false)
		{
			LOG(ERROR) << "service_manager_ Init fail";
			return false;
		}
		else
		{
			LOG(INFO) << "service_manager_ Init success";
		}

		//res = reward_data_mgr::GetInstance()->Init();
		//if( res == false )
		//{
		//	LOG(ERROR)<<" reward_data_mgr init fail ";
		//	return res;
		//}
		//else
		//{
		//	LOG(INFO)<<"reward_data_mgr init ok";
		//}


		/*
		bool ret = GlobalTableDataMgr::GetInstance()->Init( );
		if( ret == false)
		{
			LOG(ERROR) <<" GlobalTableDataMgr Init Fail";
			return ret;
		}

		LOG(INFO) <<" GlobalTableDataMgr Init sucess";

		ret = HundredTableChartDataMgr::GetInstance()->Init();
		if( ret == false)
		{
			LOG(ERROR)<< "HundredTableChartDataMgr Init Fail";
			return ret;
		}

		LOG(INFO) <<"HundredTableChartDataMgr Init Sucess";
		*/
		return true;
   	}

  	virtual void Update() 
    {
		CbUtility::updateCurLoopTime(CbUtility::get_start_time()); 
		int64_t  t1 = CbUtility::get_start_time(); 
  		Dispatcher *dispatcher = service_manager_->LocateService<Dispatcher>("Dispatcher");
  		dispatcher->Update();
		
		RoomManager::instance()->Update();

		int64_t  t2 = CbUtility::get_start_time();
		if( t2 - t1 > 100)
		{
		    LOG(ERROR) << "[Time]dispatcher  Update " << t2 - t1;
		}

  		for (auto it = gameServices_.begin(); it != gameServices_.end(); ++it)
        {
  			if ((*it)->IsRunning())	
            {
  			    (*it)->Update();
  			}
  		}

		int64_t  t3 = CbUtility::get_start_time(); 
		if( t3 - t2 > 100)
		{
		    LOG(ERROR) << "[Time]service Update " <<  t3 - t2;
			LOG(ERROR) << "[Time]loop Update " << t3 - t1;
		}

    }

  	virtual void Destory() {
  		GameLoop::Destory();
  	}
private:
  	std::set<GameService*> gameServices_;
};

OurGameLoop* loop = nullptr;

static void sigterm_handler(int signo)
{
    loop->ForceStop();
}

void signal_handler(int sig)
{
	switch (sig)
	{
	case SIGUSR1:
	{
			loop->ForceStop();
			break;
	}
	default:
		break;
	}
}

void init_signal()
{
	struct sigaction sa;
	sigset_t sset;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigterm_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

	sigemptyset(&sset);
	sigaddset(&sset, SIGSEGV);
	sigaddset(&sset, SIGBUS);
	sigaddset(&sset, SIGABRT);
	sigaddset(&sset, SIGILL);
	sigaddset(&sset, SIGCHLD);
	sigaddset(&sset, SIGFPE);
	sigprocmask(SIG_UNBLOCK, &sset, &sset);

	signal(SIGUSR1, signal_handler);
}

int main(int argc, char* argv[]) 
{
	if (argc >= 2)
	{
		if(!strcmp(argv[1],"-d"))
		{
			init_signal();
			init_daemon();	
		}
		else
		{
			init_signal();
		}
	}

	std::string config_dir = "runner";

	if(argc == 3)
	{
		config_dir = std::string(argv[2]);
	}
	std::string logcplus_config = config_dir + "/log4cplus_gs.properties";
	std::string main_config= config_dir + "/conf_gs.txt";

	LOG(DEBUG)<<"log config path" << logcplus_config;
	LOG(DEBUG)<<"main config path" << main_config;

	//为了检查是否log读取配置文件失败
	FILE* logFile = fopen(logcplus_config.c_str(), "r");
    if (logFile == nullptr) {
    	abort();
    	return 1;
    }	
	fclose(logFile);
	log4cplus::PropertyConfigurator::doConfigure(logcplus_config.c_str());
	LogPlusLogger  log4cplus_logger; 
	SetDefaultLogger(&log4cplus_logger); 

    
	LOG(DEBUG) << "server is running...";
    FILE* configFile = fopen(main_config.c_str(), "r");
    if (configFile == nullptr) {
    	LOG(ERROR) <<  "fopen error";
    	return 1;
    }
    char config[102400] = {0};
    uint32_t realLength = fread(config, sizeof(config[0]), sizeof(config), configFile);

    LOG(DEBUG) <<  "realLength: " << realLength;

    if (realLength >= sizeof(config)) {
    	LOG(ERROR) <<  "fread error , buffer size  " << sizeof(config) << " is not enough";
    	fclose(configFile);
    	return 2;
    }

    int32_t result = 0;
    if ((result = ferror(configFile)) != 0) {
    	LOG(ERROR) << "fopen error code is " << result;
    	fclose(configFile);
    	return 3;
    }
    fclose(configFile);

	loop = new OurGameLoop(config);
	loop->Run();
    return 0;
}
