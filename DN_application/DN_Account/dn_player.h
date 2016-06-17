/*
* dn_player.h
*
*  Created on: June 10, 2014
*      Author: yjding
*/

#ifndef DN_APPLICATION_DN_ACCOUNT_DN_PLAYER_H_
#define DN_APPLICATION_DN_ACCOUNT_DN_PLAYER_H_

#include "player.h"

class item_exchange_config_data;
struct GreenHandGiftItemList;

class DNPlayer : public Player{
public:
	DNPlayer();
	virtual ~DNPlayer();

	virtual void OnLogin();
	virtual void OnReconnect();
	virtual void OnLogout();

	virtual void OnLogined();
	virtual void OnPlayed();
	
    virtual void SetAccountId(uint32_t accountId);
	virtual void InitSpreeRelateData();
	virtual uint32_t CalculateNextSpreeRemainTime(bool can_get_spree, int64_t current_time);
	virtual uint32_t CalculateNextSpreeValue();
	virtual void OnGetOnlineSpree();

	virtual int32_t OnFillInviteCode(std::string invite_code);
	virtual void AddInvitedBonus(int32_t bounus);
	virtual void OnQueryInviteSmsContent(int32_t invite_type);

	//virtual std::string GetTitle(); implement in player.cpp so malong delete function in 2015-7-22

	virtual void OnRegistered(uint32_t new_account_id);
	virtual void OnInitTaskAfterRegistered();

	virtual void OnWined(int64_t result);

	virtual void OnLosed(int64_t result);

	virtual void ChangeProperty(const int32_t& propId, int64_t change, int32_t changeType = PLAYER_MONEY_CHANGE_NOT_SPECIFIED, int32_t flags = 0);

	virtual uint32_t CheckCanOpenTreasure(uint32_t id, uint64_t & need);

	virtual void OnAvatarChanged(const std::string& url);

	virtual bool ifFirstCharge();

	virtual void OnConnectionClosed();

	virtual void OnShowNiuType(int32_t niu_type);

	virtual void OnWinHundredTableGold( uint32_t win_gold, time_t cur_tm);

    virtual void OnChampionHundredTable(uint32_t win_gold, time_t cur_tm, int32_t rounds);
    virtual void OnChallengeHundredTable(int64_t win_gold, time_t cur_tm, int32_t rounds);
	virtual void OnExchangeItem(uint32_t exchange_id);
	virtual const char* GetNewestVersion() const;

    int32_t GetPlayerMatchType();

    std::string GetTradePassword() ;
    void  SetTradePassword(const std::string&password);
	 
    uint64_t GetLeftTradeAmount(int32_t type, int32_t property_id);
    uint64_t GetTradeAmount(int32_t type, int32_t property_id);
    uint32_t GetMinTradeAmount(int32_t type, int32_t property_id);
    void  IncreaseTradeAmount(int32_t type, int32_t property_id,const uint64_t& trade_amout);
    uint32_t GetPrivilege();
    uint32_t GetMaxTradeCount();
    void   SendTradeInfo();
    uint32_t CanExchangeItem(item_exchange_config_data * config_data,  uint32_t item_has_exchanged , time_t time_last_exchanged);
    void LockPlayerStatus();
    void UnLockPlayerStatus();
    bool GetPlayerLockStatus();
    void ResetTradeInfo();
    void ResetAllPlayerExchangeInfo();
    void  SetPlayerWhetherHasTradePassWord(bool has_password);

	void SendContinueLoginRewardList();
	void SendGetLoginRewardRsp(uint32_t err_code, uint32_t reward_index, uint32_t reward_type, uint32_t reward_id, uint32_t reward_count,  std::string& msg);
	uint32_t CanGetLoginReward(uint32_t reward_index);
	uint32_t OnGetLoginReward(uint32_t reward_index);
	void WriteInfo( GreenHandGiftItemList& list );
	uint32_t GetCurLoginRewardID();
	bool IsNeedShowLoginReward();

    bool IsFirstCharge();
    void UpdateFirstCharge();
	void ReadTreasureInfo(DiamondTreasureInfoRsp& rep);

	void SendOpenTreasure(OpenDiamondTreasureResp& rep);

    int32_t GetChargeMoneyTimes(int32_t money);
    void ResetChargeMoneyTimes(int32_t money);

public:
	virtual void InitItemSys();
	virtual void FinalItemSys();
private:
    bool  is_player_lock_;
public:
	virtual void SendSitDownRsp(int32_t err_code, uint8_t pos_id); 
    virtual void SendItemsExchangeResp(uint32_t error_status, uint32_t exchange_id);
};


#endif  // DN_APPLICATION_DN_ACCOUNT_DN_PLAYER_H_
