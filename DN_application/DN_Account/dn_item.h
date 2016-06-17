#ifndef _DN_ITEM_H_
#define _DN_ITEM_H_

#include "item.h"

class dn_item_sys: public ItemSys
{
public:
	dn_item_sys(Player& owner);
	virtual ~dn_item_sys();
public:
	virtual uint32_t OnAddItem(uint32_t item_id, uint32_t item_count, const char* action);
	virtual uint32_t OnDelItem(uint32_t item_id, uint32_t item_count, const char* action);
	virtual uint32_t OnUseItem(uint32_t item_id, const char* action);
	virtual uint32_t CanUseItem(uint32_t item_id);
	virtual uint32_t AfterUseItem(uint32_t item_id);	
public:
	virtual bool FromJson(rapidjson::Value& resp);
	virtual bool ToJson(rapidjson::Value& resp, rapidjson::Value::AllocatorType& allocator);
    virtual bool ExchangeDataToJson(rapidjson::Value& resp, rapidjson::Value::AllocatorType& allocator);
    virtual bool ExchangeDataFromJson(rapidjson::Value& resp);
public:
	virtual void SendPlayerItemChangeNotify(uint32_t item_id, uint32_t item_count, int32_t change_count);
	virtual void SendPlayerItemList();
	virtual void SendPlayerUseItem(uint32_t item_id, int32_t err_code);
};







#endif
