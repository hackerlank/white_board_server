#include "dn_item.h"
#include "player.h"
#include "item_config_data.h"
#include <assert.h>
#include "account_opcode_def.h"
#include "table.h"
#include "dn_table.h"
#include "dn_hall_service_opcode_def.h"
#include "service_manager.h"
#include "lua_service.h"


dn_item_sys::dn_item_sys(Player& owner)
: ItemSys( owner)
{

}

dn_item_sys::~dn_item_sys()
{

}

uint32_t dn_item_sys::CanUseItem(uint32_t item_id)
{	
	uint32_t ret = ItemSys::CanUseItem(item_id);
	if( ret != ERR_ITEM_NO_ERROR)
	{
		return ret;
	}

	LuaService* service = ServiceManager::instance()->LocateService<LuaService>("LuaService");
	if( service != nullptr)
	{
		return service->CanUseItem( &GetOwner(), item_id, 1);
	}
	return 0;
}

uint32_t dn_item_sys::AfterUseItem(uint32_t item_id)
{
	LuaService* service = ServiceManager::instance()->LocateService<LuaService>("LuaService");
	if(service != nullptr)
	{
		return service->AfterUseItem( &GetOwner(), item_id, 1);             
	}
	return 0;
}

uint32_t dn_item_sys::OnUseItem(uint32_t item_id, const char* action)
{
	LOG(INFO)<<"dn_item_sys::UseAddItem Player = "<< GetOwner().GetAccountId() <<" item_id = "<< item_id;

	uint32_t ret = CanUseItem(item_id);
	if( ret != ERR_ITEM_NO_ERROR)
	{
		LOG(ERROR)<<"CanUseItem ret = "<<ret;
		SendPlayerUseItem( item_id, ret);
		return ret;
	}

	ret = UseItem(item_id, action);
	if( ret != ERR_ITEM_NO_ERROR)
	{
		LOG(ERROR)<<" use item ret = "<<ret;
		SendPlayerUseItem( item_id, ret);
		return ret;
	}

	SendPlayerUseItem( item_id, ret);
	return ret;
}

uint32_t dn_item_sys::OnAddItem(uint32_t item_id, uint32_t item_count, const char* action)
{
	LOG(INFO)<<"dn_item_sys::OnAddItem Player = "<< GetOwner().GetAccountId() <<" item_id = "<< item_id <<" item_count = "<< item_count;
	uint32_t ret = CanAddItem(item_id, item_count);
	if( ret != ERR_ITEM_NO_ERROR )
	{
		LOG(ERROR)<<"CanAddItem ret = "<<ret;
		return ret;
	}
	ret = AddItem(item_id, item_count, action);
	if( ret != ERR_ITEM_NO_ERROR)
	{
		LOG(ERROR)<<"AddItem ret = " << ret;
		return ret;
	}

	int32_t change_count = item_count;

	uint32_t cur_count = GetItemCount(item_id);
	SendPlayerItemChangeNotify( item_id, cur_count, change_count);
	
	Table* table = FindTable( &GetOwner() );
	if(table)
	{
		table->BroadCastPlayerItemChange( GetOwner().GetAccountId(), item_id, cur_count, change_count);
	}
	return ERR_ITEM_NO_ERROR;
}
 
uint32_t dn_item_sys::OnDelItem(uint32_t item_id, uint32_t item_count, const char* action)
{
	LOG(INFO)<<"dn_item_sys::OnDelItem Player = "<< GetOwner().GetAccountId() <<" item_id = "<< item_id <<" item_count = "<< item_count;

	uint32_t ret = CanDelItem(item_id, item_count);
	if( ret != ERR_ITEM_NO_ERROR )
	{   
		LOG(ERROR)<<"CanDelItem ret = "<<ret;
		return ret;
	}   
	ret = DelItem(item_id, item_count, action);
	if( ret != ERR_ITEM_NO_ERROR)
	{   
		LOG(ERROR)<<"DelItem ret = " << ret;
		return ret;
	}   

	int32_t change_count = (int32_t)item_count * (-1);
	uint32_t cur_count = GetItemCount(item_id);
	SendPlayerItemChangeNotify( item_id, cur_count, change_count);
	return ERR_ITEM_NO_ERROR;
}

void dn_item_sys::SendPlayerItemChangeNotify(uint32_t item_id, uint32_t item_count, int32_t change_count)
{
	PlayerItemChangeNtf ntf;
	ntf.item_id = item_id;
	ntf.item_count = item_count;
	ntf.change_count = change_count;

	PacketTranscode packet;
	packet.SetOpcode( SMSG_PLAYER_ITEM_CHANGE );
	ntf.ToPacket(packet);

	GetOwner().SendMsgToClient(packet);
}

void dn_item_sys::SendPlayerItemList()
{
	PlayerItemInfoList list;
	WriteInfo(list);

	PacketTranscode packet;
	packet.SetOpcode( SMSG_PLAYER_ITEM_LIST_NTF );

	list.ToPacket(packet);

	GetOwner().SendMsgToClient(packet);
}

void dn_item_sys::SendPlayerUseItem(uint32_t item_id, int32_t err_code)
{
	PlayerUseItemRsp rsp;
	rsp.item_id  = item_id;
	rsp.err_code = err_code;
	
	PacketTranscode packet;
	packet.SetOpcode( SMSG_USE_ITEM_RSP );
	rsp.ToPacket(packet);

	GetOwner().SendMsgToClient(packet);
}


bool dn_item_sys::FromJson(rapidjson::Value& resp)
{
	if(!resp.IsObject())
	{
		LOG(ERROR)<<"dn_item_sys::FromJson IsNotObject";
		return false;
	}

	ClearItems();

	if(resp.HasMember("items") && resp["items"].IsString() )
	{
		rapidjson::Value &items_obj = resp["items"];
		
		std::string jsonStr = items_obj.GetString();
		rapidjson::Document json;
		json.Parse<0>(jsonStr.c_str());
		if (!json.IsObject()) 
		{    
			return false;
		}     

		auto it_end = json.MemberEnd();
		for (auto it = json.MemberBegin(); it != it_end; ++it) 
		{    
			if (it->name.IsString()) 
			{    
				uint32_t item_id = atoi(it->name.GetString());
				uint32_t item_count = it->value.GetUint();

				AddItem(item_id, item_count);
			}    
			else 
			{    
				continue;
			}    
		}    

	}
	return true;
}

bool dn_item_sys::ToJson(rapidjson::Value& resp, rapidjson::Value::AllocatorType& allocator)
{
	std::string json_str;
	json_str.append("{");
	
	std::map<uint32_t, player_item_data>::const_iterator it_begin = item_datas_.begin();
	std::map<uint32_t, player_item_data>::const_iterator it_end = item_datas_.end();

	if (it_begin != it_end)
	{
		const player_item_data& r_data = it_begin->second;
		uint32_t item_id = r_data.GetItemID();
		uint32_t item_count = r_data.GetItemCount();
		char item_buff[1024] = { 0 };

		snprintf(item_buff, sizeof(item_buff), "\"%u\":%u", item_id, item_count);
		json_str.append(item_buff);

		for (++it_begin; it_begin != it_end; ++it_begin)
		{
			const player_item_data& r_data = it_begin->second;
			uint32_t item_id = r_data.GetItemID();
			uint32_t item_count = r_data.GetItemCount();
			char item_buff[1024] = { 0 };

			snprintf(item_buff, sizeof(item_buff), ",\"%u\":%u", item_id, item_count);
			json_str.append(item_buff);
		}
	}
	json_str.append("}");

	rapidjson::Value items_info_value(json_str.c_str(), allocator);
	resp.AddMember("items", items_info_value, allocator);

	return true;
}


bool dn_item_sys::ExchangeDataFromJson(rapidjson::Value& resp)
{
    if(!resp.IsObject())
    {
        LOG(ERROR)<<"dn_item_sys::ExchangeDataFromJson IsNotObject";
        return false;
    }
    ClearExchangeItems();

    if(resp.HasMember("exchange_items") && resp["exchange_items"].IsString() )
    {
        rapidjson::Value &items_obj = resp["exchange_items"];

        std::string jsonStr = items_obj.GetString();
        rapidjson::Document json;
        json.Parse<0>(jsonStr.c_str());
        if (!json.IsObject()) 
        {    
            return false;
        }     
        player_item_exchange_limit_data item_exchange ;
        auto it_end = json.MemberEnd();
        for (auto it = json.MemberBegin(); it != it_end; ++it) 
        {    
            if (it->name.IsString()) 
            {    
                uint32_t exchange_id = atoi(it->name.GetString());
                const char * buffer = it->value.GetString();  
                if(sscanf(buffer,"%d_%d",&item_exchange.current_exchanged_,&item_exchange.last_exchange_time_) == 2)
                {
                    item_exchange.exchange_id_ = exchange_id;
                    item_exchange_limit_datas_[exchange_id] = item_exchange;
                    LOG(DEBUG)<<"dn_item_sys::ExchangeDataFromJson exchange_items exchange_id" << exchange_id << " current_exchanged_ "<<item_exchange.current_exchanged_ << "last_exchange_time_" << item_exchange.last_exchange_time_;
                }
            }    
            else 
            {    
                continue;
            }    
        }    

    }
    return true;
}

bool dn_item_sys::ExchangeDataToJson(rapidjson::Value& resp,rapidjson::Value::AllocatorType& allocator)
{

    std::string json_str;
    json_str.append("{");

    std::map<uint32_t, player_item_exchange_limit_data>::const_iterator it_begin = item_exchange_limit_datas_.begin();
    std::map<uint32_t, player_item_exchange_limit_data>::const_iterator it_end = item_exchange_limit_datas_.end();

    if (it_begin != it_end)
    {
        const player_item_exchange_limit_data& r_data = it_begin->second;
        uint32_t exchange_id = it_begin->first;
        uint32_t current_exchange = r_data.GetCurrentExchange();
        uint32_t last_exchange_time = r_data.GetLastExchangeTime();
        char item_buff[1024] = { 0 };

        snprintf(item_buff, sizeof(item_buff), "\"%u\":%u_%u", exchange_id, current_exchange, last_exchange_time);
        LOG(DEBUG)<<"dn_item_sys::ExchangeDataToJson exchange_items exchange_id" << exchange_id << " current_exchanged_ "<<current_exchange << "last_exchange_time_" << last_exchange_time;
        json_str.append(item_buff);

        for (++it_begin; it_begin != it_end; ++it_begin)
        {
            const player_item_exchange_limit_data& r_data = it_begin->second;
            uint32_t exchange_id = it_begin->first;
            uint32_t current_exchange = r_data.GetCurrentExchange();
            uint32_t last_exchange_time = r_data.GetLastExchangeTime();
            char item_buff[1024] = { 0 };

            snprintf(item_buff, sizeof(item_buff), "\"%u\":%u_%u", exchange_id, current_exchange, last_exchange_time);
            LOG(DEBUG)<<"dn_item_sys::ExchangeDataToJson exchange_items exchange_id" << exchange_id << " current_exchanged_ "<<current_exchange << "last_exchange_time_" << last_exchange_time;
            json_str.append(item_buff);
        }
    }
    json_str.append("}");

    rapidjson::Value items_info_value(json_str.c_str(), allocator);
    resp.AddMember("exchange_items", items_info_value, allocator);
    //log
    rapidjson::StringBuffer buffer_test;
    rapidjson::Writer<rapidjson::StringBuffer> writer_test(buffer_test);
    resp.Accept(writer_test);
    LOG(DEBUG) << "[dn_item_sys::ExchangeDataToJson] json debug" << buffer_test.GetString();

    return true;
}


