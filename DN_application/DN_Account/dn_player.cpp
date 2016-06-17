/*
* dn_player.h
*
*  Created on: June 10, 2014
*      Author: yjding
*/

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "service_manager.h"
#include "task_service.h"
#include "chart_service.h"
#include "contests_service.h"
#include "player_manager.h"
#include "player_session.h"
#include "global_error_code.h"
#include "dn_task_trigger_event_def.h"
#include "lua_service.h"
#include "dn_lua_event.h"
#include "DN_player_def.h"
#include "rabbitmq_client.h"
#include "hundred_table_chart_data.h"
#include "CbFormat.h"
#include "cb_time.h"
#include "global_table_data.h"
#include "dn_table.h"
#include "dn_bull_red_bag.h"
#include "dn_player.h"
#include "dn_contests_service_def.h"
#include "dn_hall_service_opcode_def.h"
#include "trade_service.h"
#include "trade_service_def.h"
#include "item.h"
#include "dn_item.h"
#include "items_exchange_def.h"
#include "items_exchange_opcode.h"
#include "item_exchange_config_data.h"
#include "dn_hall_service_opcode_def.h"
#include "login_reward.h"
#include "treasure_config_data.h"
//yujiding 2014/05/30
static uint32_t DecodingInviteCode(std::string invite_code)
{
    for (size_t i = 0; i<invite_code.length(); ++i)
    {
        char v = static_cast<char>(invite_code[i]);
        if (v > 57 || v < 48)
            return 0;
    }
    uint32_t a = atoi(invite_code.substr(0, invite_code.length() - 1).c_str());
    uint32_t b = atoi(invite_code.substr(invite_code.length() - 1, 1).c_str());
    if (b != a % 7)
        return 0;

    return a - 32606170;
}

/*define but not used delete by malong in 2015-3-26
static std::string EncodingInviteCode(uint32_t account_id)
{
    uint32_t t = account_id + 32606170;
    char buf[48] = { 0 };
    snprintf(buf, sizeof(buf), "%d%d", t, t % 7);
    return buf;
}
*/
DNPlayer::DNPlayer() :is_player_lock_(false)
{
	InitItemSys();		
}

DNPlayer::~DNPlayer()
{
	FinalItemSys();
}

void DNPlayer::OnLogin()
{
    Player::OnLogin();

	DNPlayerCharacterInfo* dn_player_info = reinterpret_cast<DNPlayerCharacterInfo*>(player_character_info_);

	time_t cur_tm = time(nullptr);
	if( !IsSameDay( dn_player_info->new_continue_login_tm_, cur_tm) )
	{
		dn_player_info->new_continue_login_times_++;
		dn_player_info->new_continue_login_tm_ = cur_tm;
	}

	if( IsNeedShowLoginReward() )
	{
		SendContinueLoginRewardList();
	}
}

void DNPlayer::OnReconnect()
{
    Player::OnReconnect();
}

void DNPlayer::OnLogout()
{
    DNPlayerCharacterInfo* dn_player_characterinfo = reinterpret_cast<DNPlayerCharacterInfo*>(player_character_info_);

    dn_player_characterinfo->lastLogoutTime = time(nullptr);

    Player::OnLogout();
}

void DNPlayer::SetAccountId(uint32_t accountId){
    Player::SetAccountId(accountId);
    DNPlayerCharacterInfo* dn_character_info = static_cast< DNPlayerCharacterInfo*>(player_character_info_);
    dn_character_info->SetTradeID(accountId);
}

void DNPlayer::OnLogined()
{
    LOG(DEBUG) << "[DNPlayer::OnLogined]  ";
    Player::OnLogined();
    TaskService* task_service = ServiceManager::instance()->LocateService<TaskService>("Task");
    if (task_service != nullptr) {
        task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventDailyLogin, 1);
    }
    LuaService * luaService = ServiceManager::instance()->LocateService<LuaService>("LuaService");


    std::map<std::string, LuaValue> maplua;
    maplua["accountId"] = LuaValue(GetAccountId());
    DNPlayerCharacterInfo* characterInfo = reinterpret_cast<DNPlayerCharacterInfo*>(GetPlayerCharacterInfo());
    maplua["lastLoginTime"] = LuaValue(characterInfo->lastLoginTime);
    maplua["superGiftBuyTime"] = LuaValue(characterInfo->superGiftBuyTime);
    maplua["superGiftFinishFlag"] = LuaValue(characterInfo->superGiftFinishFlag);

    luaService->CallLuaEventDispatcher(maplua, SCRIPT_EVENT_PLAYER_LOGIN);

    characterInfo->lastLoginTime = time(nullptr);
    OnChanged();

}

void DNPlayer::OnPlayed()
{
    ++player_character_info_->Played;

    TaskService* task_service = ServiceManager::instance()->LocateService<TaskService>("Task");
    if (task_service != nullptr) {
        task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventGameTimes, 1);
        task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventDailyGameTimes, 1);
    }

    OnChanged();
}

void DNPlayer::OnWined(int64_t result)
{
    LOG(DEBUG) << "[DNPlayer::OnWined]       ";
    int64_t max_win_before_change = player_character_info_->Wined;
    int64_t day_max_win_vefore_change = player_character_info_->thisDayMaxWin;
    ++player_character_info_->Wined;
    player_character_info_->MaxWin =
        result > player_character_info_->MaxWin ? result : player_character_info_->MaxWin;

    //如果现在和lastTimeOfDayMaxWin不是一天，则daymaxwin重置，lasttime设为现在
    time_t now = time(nullptr);
    if (!IsSameDay(player_character_info_->lastTimeOfDayMaxWin,now))
    //if ((player_character_info_->lastTimeOfDayMaxWin / (24 * 3600)) != (now / (24 * 3600)))
    {
        player_character_info_->lastTimeOfDayMaxWin = now;
        player_character_info_->thisDayMaxWin = result;
    }
    else
    {
        player_character_info_->thisDayMaxWin += result;
    }

    player_character_info_->dayMaxwin = player_character_info_->thisDayMaxWin > player_character_info_->dayMaxwin ?
        player_character_info_->thisDayMaxWin : player_character_info_->dayMaxwin;


    TaskService* task_service = ServiceManager::instance()->LocateService<TaskService>("Task");
    if (task_service != nullptr) {
        task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventDailyWin, 1);
        task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventGameWin, 1);
    }

    ChartService* chart_service = ServiceManager::instance()->LocateService<ChartService>("Chart");
    if (chart_service != nullptr)
    {
        chart_service->OnChange(kChartIdWined, this, max_win_before_change);
        chart_service->OnChange(kChartIdDayMaxWin, this, day_max_win_vefore_change);
    }

    OnChanged();
}

void DNPlayer::InitSpreeRelateData()
{
    LOG(DEBUG) << "DnPlayer:InitSpreeRelateData " << player_character_info_->onlineSpree;
    //重新登录后需要将上次领奖时间重置
    if (player_character_info_->onlineSpreeTime == 0 || !IsSameDay(player_character_info_->onlineSpreeTime, time(nullptr))) //player_character_info_->onlineSpreeTime / (3600 * 24) != time(nullptr) / (3600 * 24))
    {
        player_character_info_->onlineSpreeSum = 0;
    }
    player_character_info_->onlineSpreeTime = time(NULL);

    DNPlayerCharacterInfo * dn_player_character_info = reinterpret_cast<DNPlayerCharacterInfo *>(player_character_info_);

    //    //若上次登录和现在不是一天，重置领奖相关的数据
    //    if (player_character_info_->onlineSpree == 0 || (dn_player_character_info->lastLoginTime / (3600 * 24) != time(nullptr) / (3600 * 24)))

    if (player_character_info_->onlineSpree == 0 || (time(nullptr) - dn_player_character_info->lastLogoutTime) / 60 > 10)
    {
        player_character_info_->onlineSpree = 1;
        player_character_info_->nextSpreeRemaningTime = CalculateNextSpreeRemainTime(true, 0);
        player_character_info_->nextSpreeValue = CalculateNextSpreeValue();

        LOG(DEBUG) << "DnPlayer:InitSpreeRelateData set" << player_character_info_->nextSpreeRemaningTime;
    }

    //yujiding 2014/05/29 登陆成功时同步客户端定时奖励时间
    //     PacketTranscode packet;
    //     packet.SetOpcode(SMSG_PLAYER_GET_SPREE_RESP);
    //     packet << CommonError << player_character_info_->nextSpreeRemaningTime << player_character_info_->nextSpreeValue;
    //     player_session_->SendTranscode(packet);

    LOG(DEBUG) << "DnPlayer:InitSpreeRelateData  " << player_character_info_->nextSpreeRemaningTime << " and " << player_character_info_->onlineSpreeTime;
}

uint32_t DNPlayer::CalculateNextSpreeRemainTime(bool can_get_spree, int64_t current_time)
{
    //时间公式 =IF(次数>=5,POWER(5,2)*60,POWER(次数,2)*60)
    uint32_t remain_time = 0;

    if (can_get_spree)
    {
        uint32_t temp_online_spree = player_character_info_->onlineSpree;
        remain_time = temp_online_spree > 5 ? 5 * 5 * 60 : temp_online_spree * temp_online_spree * 60;
        return remain_time;
    }
    else
    {
        remain_time = player_character_info_->onlineSpreeTime + player_character_info_->nextSpreeRemaningTime - current_time;
        return remain_time > 0 ? remain_time : 0;
    }
}

uint32_t DNPlayer::CalculateNextSpreeValue()
{
    //奖励公式 =IF(次数>=5,200+(5-1)*800,200+(次数-1)*800)
    return player_character_info_->onlineSpree > 5 ?
        (200 + (5 - 1) * 800) * player_manager_->spree_rate_ :
        (200 + (player_character_info_->onlineSpree - 1) * 800) * player_manager_->spree_rate_;
}

void DNPlayer::OnGetOnlineSpree()
{
}

int32_t DNPlayer::OnFillInviteCode(std::string invite_code)
{
    if (player_character_info_->Invite.Invited != "")
    {
        return INVITED_ERROR_COMPLETED;
    }

    uint32_t invite_player_id = DecodingInviteCode(invite_code);
    LOG(DEBUG) << "&&&&invite id is" << invite_player_id;

    if (invite_player_id <= 0 || invite_player_id == player_account_info_->accountID)
    {
        return INVITED_ERROR_INVALID;
    }

    // 推荐人的奖励为 金币=2000
    // 输入推荐码的奖励为 金币=10000
    
    if (player_manager_ != nullptr){
        std::string str = "";
        char tmp[1024] = { 0 };
        snprintf(tmp, sizeof(tmp), "%d|%u", player_manager_->reward_of_invited_, invite_player_id);
        str.append(tmp);
        PrintGrootLog("be_recommended_reward", str);
    }
       


    ChangeProperty(PropertyType::PROPERTY_GOLD_COINS, player_manager_->reward_of_invited_, PLAYER_MONEY_CHANGE_TYPE::PLAYER_MONEY_CHANGE_INVITED_REWARD, 1);
    OSS_MONEY_LOG(this, "DN_PLAYER", "INVITED_REWARD", player_manager_->reward_of_invited_);
	
	player_character_info_->Invite.Invited = invite_code;

    SyncPlayerInfo();
         
    player_manager_->ChangeProperty(invite_player_id, PropertyType::PROPERTY_GOLD_COINS,
        player_manager_->reward_of_invite_, OfflinePlayerPropertyChange::KInvitedBones, 0);

    return CommonSuccess;
}

void DNPlayer::AddInvitedBonus(int32_t bounus)
{
    int32_t invite_count = atoi(player_character_info_->Invite.MyInviteCount.c_str());
    ++invite_count;

    char buf_invite_count[16] = { 0 };
    snprintf(buf_invite_count, sizeof(buf_invite_count), "%d", invite_count);

    player_character_info_->Invite.MyInviteCount = std::string(buf_invite_count);

    LOG(DEBUG) << "&&&&invite player hisInviteCount is" << invite_count;

    if (invite_count <= player_manager_->max_invite_count_)
    {  
       
        if (player_manager_ != nullptr){
            std::string str = "";
            char tmp[1024] = { 0 };
            snprintf(tmp, sizeof(tmp), "%d", player_manager_->reward_of_invite_);
            str.append(tmp);
            PrintGrootLog("recommended_reward", str);
        }
           
       
        ChangeProperty(PropertyType::PROPERTY_GOLD_COINS, player_manager_->reward_of_invite_, PLAYER_MONEY_CHANGE_TYPE::PLAYER_MONEY_CHANGE_INVITED_REWARD, 0);
		OSS_MONEY_LOG(this, "DN_PLAYER", "INVITED_REWARD", player_manager_->reward_of_invited_);
    }
}

void DNPlayer::OnQueryInviteSmsContent(int32_t invite_type)
{
}

/*
std::string DNPlayer::GetTitle()
{
    //称号
    return player_character_info_->Title = player_manager_->GetTitleName(player_character_info_->GoldCoins);
}
*/

void DNPlayer::OnRegistered(uint32_t new_account_id)
{
    TaskService* task_service = ServiceManager::instance()->LocateService<TaskService>("Task");
    if (task_service != nullptr) {

        task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventAccountBind, 1);
        task_service->OnPlayerRegistered(GetAccountId(), new_account_id);
        task_service->OnPlayerRegisteredBeforeLogout(this);
    }
}

void DNPlayer::OnInitTaskAfterRegistered()
{
    TaskService* task_service = ServiceManager::instance()->LocateService<TaskService>("Task");
    if (task_service != nullptr) {
        task_service->OnPlayerRegisteredAndLogin(this);
    }
}

void DNPlayer::OnLosed(int64_t result)
{
    player_character_info_->MaxLose =
        (-1 * result) > player_character_info_->MaxLose ? (-1 * result) : player_character_info_->MaxLose;
}

void DNPlayer::ChangeProperty(const int32_t& propId, int64_t change, int32_t changeType /*= PLAYER_MONEY_CHANGE_NOT_SPECIFIED*/, int32_t flags /*= 0*/)
{
    Player::ChangeProperty(propId, change, changeType, flags);

    if (propId != PropertyType::PROPERTY_GOLD_COINS && propId != PropertyType::PROPERTY_DIAMONDS)
    {
        return;
    }
    //如果玩家在牌桌中，需要刷新桌子里显示的钱数和称号
	ContestsService* contests_service = ServiceManager::instance()->LocateService<ContestsService>("Contests");
    if (contests_service != nullptr) {
        LOG(DEBUG) << "DNPlayer::ChangeProperty OnChangePropertyInTable";
        contests_service->OnChangePropertyInTable(this, propId, change, changeType);
    }
}

uint32_t DNPlayer::CheckCanOpenTreasure(uint32_t id, uint64_t & need){

	int state =0 ;
	int flag=Error_Treasue::ERR_NO_ERROR;
	need = 0;
	DiamondTreasure* box = treasure_config_mgr::GetInstance()->GetTreasure(id);
	if (box == NULL)
	{
		flag = Error_Treasue::ERR_SYSTERM_ERROR;
		return flag;
	}
	need=box->TreatureNeed;
	if (GetPlayerMatchType()==eMatchDiamondType)
	{
		CheckCanCharge(state);
		switch (state)
		{
		case EnumPlayerInGameState::ePlayerNotInGame :
			
			break;
		case EnumPlayerInGameState::ePlayerZhuang :
			flag = Error_Treasue::ERR_DIAMOND_DEALER;
			break;
		case EnumPlayerInGameState::ePlayerInZhuangList :
			flag = Error_Treasue::ERR_DIAMOND_DEALER_LIST;
			break;
		case EnumPlayerInGameState::ePlayerAnteNotJieSuan :
			flag = Error_Treasue::ERR_DIAMOND_BET;
			break;
		}
	}
	else if(GetPlayerMatchType()==eMatchNormlDiamondType1||GetPlayerMatchType()==eMatchNormlDiamondType2||GetPlayerMatchType()==eMatchNormlDiamondType3)
	{
		flag = Error_Treasue::ERR_DIAMOND_GRAB_DEALER;
	}
	else if(GetPlayerMatchType()==eMatchRandomDiamondType1||GetPlayerMatchType()==eMatchRandomDiamondType2||GetPlayerMatchType()==eMatchRandomDiamondType3)
	{
		flag = Error_Treasue::ERR_DIAMOND_RANDOM_DEALER;
	}
	else
	{
		flag =  Error_Treasue::ERR_NO_ERROR;
	}
	if(flag == Error_Treasue::ERR_NO_ERROR)
	{
		if((uint64_t)GetProperty(PropertyType::PROPERTY_DIAMONDS) < box->TreatureNeed)
		{
			flag = Error_Treasue::ERR_DIAMOND_NOT_ENOUGH;
		}
	}
	return flag;
}

void DNPlayer::OnAvatarChanged(const std::string& url)
{
    Player::OnAvatarChanged(url);
    if (!url.empty())
    {
        // 完成任务
        TaskService* task_service = ServiceManager::instance()->LocateService<TaskService>("Task");
        if (task_service != nullptr)
        {
            task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventAccountAvatar, 1);
        }
    }
}

bool DNPlayer::ifFirstCharge()
{
    uint32_t superGiftBuyTime = reinterpret_cast<DNPlayerCharacterInfo*>(player_character_info_)->superGiftBuyTime;
    double totalChargeFee = GetMaxFillCoins();
    if ((totalChargeFee == 6.0 && superGiftBuyTime > 0) || totalChargeFee == 0.0)
        return true;
    else
        return false;
}

void DNPlayer::OnConnectionClosed()
{
    DNPlayerCharacterInfo* dn_player_characterinfo = reinterpret_cast<DNPlayerCharacterInfo*>(player_character_info_);

    dn_player_characterinfo->lastLogoutTime = time(nullptr);
}

void DNPlayer::OnShowNiuType(int32_t niu_type)
{
    //yujiding    2014/05/28 修改玩家最大牛记录及触发特殊牛成就任务
    DNPlayerCharacterInfo * dn_character_info = reinterpret_cast<DNPlayerCharacterInfo *>(GetPlayerCharacterInfo());
    if (niu_type > atoi(dn_character_info->MaxNiuType.c_str()))
    {
        dn_character_info->MaxNiuType = CbUtility::formatString("%d", niu_type);
    }

    TaskService* task_service = ServiceManager::instance()->LocateService<TaskService>("Task");
    if (task_service != nullptr){
        if (niu_type == DNCardType::SI_ZHA){
            task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventHandSizha, 1);
        }else if (niu_type == DNCardType::WU_HUA_NIU){
            task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventHandWuhuaniu, 1);
        }else if (niu_type == DNCardType::WU_XIAO_NIU){
            task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventHandWuxiaoniu, 1);
        }else if (niu_type == DNCardType::NIU_NIU){
            task_service->TriggerEvent(GetAccountId(), DNTaskTriggerEvent::kDnTaskEventHandNiuniu, 1);
        }else{
        }
    }
}

void DNPlayer::OnWinHundredTableGold(uint32_t win_gold, time_t cur_tm)
{
	LOG(INFO)<<"DNPlayer::OnWinHundredTableGold AccountID = "<< GetAccountId() <<" win_gold = "<< win_gold <<" cur_tm "<< cur_tm;

	DNTable* table = FindTable(this);
	if(table == nullptr)
	{
		LOG(INFO)<<"DNPlayer::OnWinHundredTableGold AccountID = "<< GetAccountId()<<" Can not find table";
		return;
	}

	uint32_t match_type = table->GetMatchType();

	DNPlayerCharacterInfo * dn_character_info = reinterpret_cast<DNPlayerCharacterInfo *>(GetPlayerCharacterInfo()); 		
	assert(dn_character_info);
	if( !IsSameDay( dn_character_info->GetHundredWinMoneyTm(match_type),  cur_tm) )
	{
		LOG(INFO)<<"DNPlayer::OnWinHundredTableGold AccountID = "<< GetAccountId() << "last_tm = "<< dn_character_info->GetHundredWinMoneyTm(match_type);
		dn_character_info->SetHundredWinMoneyTm(match_type, cur_tm);
		dn_character_info->SetHundredTableWinMoney(match_type, win_gold);
	}
	else
	{
		LOG(INFO)<<"DNPlayer::OnWinHundredTableGold AccountID = "<< GetAccountId() << "cur_money = "<< dn_character_info->GetHundredTableWinMoney(match_type); 
		dn_character_info->SetHundredWinMoneyTm(match_type, cur_tm);
		dn_character_info->AddHundredTableWinMoney(match_type, win_gold);
	}

	LOG(INFO)<<"DNPlayer::OnWinHundredTableGold AccountID = "<< GetAccountId() << "final_win_gold = "<<dn_character_info->GetHundredTableWinMoney(match_type);

	table->GetHundredTableChartDataMgr().OnUpdateNormalChart(match_type, 
															 GetAccountId(), 
															 GetNick(), 
															 GetPlayerAccountInfo()->avatar,  
															 dn_character_info->GetHundredTableWinMoney(match_type), 
															 cur_tm);
}
void DNPlayer::OnChampionHundredTable(uint32_t win_gold, time_t cur_tm, int32_t rounds)
{
    if (rounds <= 0)
    {
        return;
    }
    DNTable* table = FindTable(this);
    if (table == nullptr)
    {
        LOG(INFO) << "DNPlayer::OnChampionHundredTable AccountID = " << GetAccountId() << " Can not find table";
        return;
    }

    uint32_t match_type = table->GetMatchType();
    DNPlayerCharacterInfo * dn_character_info = reinterpret_cast<DNPlayerCharacterInfo *>(GetPlayerCharacterInfo());
    assert(dn_character_info);

    if (rounds == 1 || cur_tm - dn_character_info->GetLastChampionWinTime(match_type) > 30*60 )
    {
        dn_character_info->SetHundredTableChampionMoney(match_type, win_gold);
    }
    else
	{
        dn_character_info->AddHundredTableChampionMoney(match_type, win_gold);
    }
    if (rounds >0)
    {
        dn_character_info->SetLastChampionWinTime(match_type, cur_tm);
    }
    table->GetHundredTableChartDataMgr().OnUpdateChampionChart(match_type, 
															   GetAccountId(), 
															   GetNick(), 
															   GetPlayerAccountInfo()->avatar, 
															   dn_character_info->GetHundredTableChampionMoney(match_type),
															   rounds);
}


void DNPlayer::OnChallengeHundredTable(int64_t win_gold, time_t cur_tm, int32_t rounds)
{
    if (rounds <= 0)
    {
        LOG(INFO) << "DNPlayer::OnChallengeHundredTable AccountID = " << GetAccountId() << "  rounds = " <<rounds;
        return;
    }
    DNTable* table = FindTable(this);
    if (table == nullptr)
    {
        LOG(INFO) << "DNPlayer::OnChallengeHundredTable AccountID = " << GetAccountId() << " Can not find table";
        return;
    }

	uint32_t match_type = table->GetMatchType();
    DNPlayerCharacterInfo * dn_character_info = reinterpret_cast<DNPlayerCharacterInfo *>(GetPlayerCharacterInfo());
    assert(dn_character_info);

    if (win_gold > 0)
    {
        dn_character_info->IncChallengeLastWin(match_type, win_gold);
    }
    if (dn_character_info->GetChallengeMostWin(match_type) < dn_character_info->GetChallengeLastWin(match_type)) //新的成绩
    {
        dn_character_info->SetChallengeMostWin(match_type, dn_character_info->GetChallengeLastWin(match_type));
        table->GetHundredTableChartDataMgr().OnUpdateChallengeChart(match_type,
            GetAccountId(),
            GetNick(),
            GetPlayerAccountInfo()->avatar,
            dn_character_info->GetChallengeLastWin(match_type),
            rounds);
    }
    LOG(INFO) << "DNPlayer::OnChallengeHundredTable AccountID = " << GetAccountId() << " most = " << dn_character_info->GetChallengeMostWin(match_type) 
        << " last =" << dn_character_info->GetChallengeLastWin(match_type);
}

void DNPlayer::OnExchangeItem(uint32_t exchange_id)
{	
    uint32_t error_status = ItemExchangeStatus::kExchangeSuccess;
    item_exchange_config_data *config_data = item_exchange_config_mgr::GetInstance()->GetItemConfig(exchange_id);
    if (config_data == nullptr)
    {
        error_status = ItemExchangeStatus::kExchangeIDNotExist;
        LOG(ERROR) << "DNPlayer::OnExchangeItem exchange id not exist,the exchange id is" << exchange_id;
    }
    else
    {
       //get player item info
        player_item_exchange_limit_data *player_data = GetItemSys()->GetExchangeItem(exchange_id);
        uint32_t item_has_exchanged = 0;
        time_t time_last_exchanged = time(nullptr);

        if(player_data != nullptr)// not first time to exchange
        {
            item_has_exchanged = player_data->GetCurrentExchange();//has exchanged      
            time_last_exchanged = (time_t)player_data->GetLastExchangeTime();
        }
        // check is same day or not
        time_t now = time(nullptr);
        if(IsSameDay(time_last_exchanged,now)) //same day
        {
            if(item_has_exchanged >= config_data->GetItemExchangeDailyMax())//reach limit
            {
                error_status = ItemExchangeStatus::kReachedLimit;
                LOG(INFO) << "DNPlayer::OnExchangeItem exchange read limit exchange_upper_limit =" << config_data->GetItemExchangeDailyMax() << "current_exchange" << item_has_exchanged;
            }
            else //same day not reach limit 
            {
                error_status = CanExchangeItem(config_data,item_has_exchanged,time_last_exchanged);
                if(error_status == ItemExchangeStatus::kExchangeSuccess)// can exchange
                {
                    GetItemSys()->OnDelItem(config_data->GetItemNeedID(), config_data->GetItemNeedQuantity(),"exchange_item");
                    GetItemSys()->OnAddItem(config_data->GetItemExchangeID(), config_data->GetItemExchangeQuantity(),"exchange_item");
                    GetItemSys()->SetExchangeItemById(exchange_id,1);
                    LOG(INFO) << "DNPlayer::OnExchangeItem exchange sucess exchange_id"<< config_data->GetExchangeID() <<"item_has_exchanged"<< ++item_has_exchanged <<"time_last_exchanged"<< time_last_exchanged << "limit exchange_upper_limit =" << config_data->GetItemExchangeDailyMax();
                }
                else    //can not change
                {
                    LOG(INFO) << "DNPlayer::OnExchangeItem exchange fail exchange_id"<< config_data->GetExchangeID() <<"item_has_exchanged"<< item_has_exchanged <<"time_last_exchanged"<<time_last_exchanged<< "limit exchange_upper_limit =" << config_data->GetItemExchangeDailyMax();
                }
            }
        }
        else //not same day
        {
            //reset player info
            player_data->current_exchanged_ = 0;
            player_data->last_exchange_time_ = time(nullptr);
            item_has_exchanged = 0;
            time_last_exchanged = time(nullptr);
            error_status = CanExchangeItem(config_data,item_has_exchanged,time_last_exchanged);
            if(error_status == ItemExchangeStatus::kExchangeSuccess)// can exchange
            {
                GetItemSys()->OnDelItem(config_data->GetItemNeedID(), config_data->GetItemNeedQuantity(),"exchange_item");
                GetItemSys()->OnAddItem(config_data->GetItemExchangeID(), config_data->GetItemExchangeQuantity(),"exchange_item");
                GetItemSys()->SetExchangeItemById(exchange_id,1);
                LOG(INFO) << "DNPlayer::OnExchangeItem exchange sucess exchange_id"<< config_data->GetExchangeID() <<"item_has_exchanged"<< ++item_has_exchanged <<"time_last_exchanged"<<time_last_exchanged << "limit exchange_upper_limit =" << config_data->GetItemExchangeDailyMax();
            }
            else //can not exchange
            {
                LOG(INFO) << "DNPlayer::OnExchangeItem exchange fail exchange_id"<< config_data->GetExchangeID() <<"item_has_exchanged"<<item_has_exchanged<<"time_last_exchanged"<<time_last_exchanged<< "limit exchange_upper_limit =" << config_data->GetItemExchangeDailyMax();
            }
        }
    }
    //send to client
    SendItemsExchangeResp(error_status,exchange_id);
}

uint32_t DNPlayer::CanExchangeItem(item_exchange_config_data * config_data,  uint32_t item_has_exchanged , time_t time_last_exchanged)
{
    uint32_t error_status = ItemExchangeStatus::kExchangeSuccess;
    uint32_t ret = GetItemSys()->CanDelItem(config_data->GetItemNeedID(), config_data->GetItemNeedQuantity());
    if (ret != ERR_ITEM_NO_ERROR)
    {
        error_status = ItemExchangeStatus::kTicketNotEnough;
        LOG(INFO) << "DNPlayer::CanExchangeItem Ticket is not enough ";
        return error_status;
    }
    ret =GetItemSys()->CanAddItem(config_data->GetItemExchangeID(), config_data->GetItemExchangeQuantity());
    if(ret != ERR_ITEM_NO_ERROR)
    {
        error_status = ItemExchangeStatus::kMaxCarrayOut;
        LOG(INFO) << "DNPlayer::CanExchangeItem Item reach max carray out ";
        return error_status;
    }
    return error_status;
}
const char* DNPlayer::GetNewestVersion() const
{
	const DNPlayerCharacterInfo* dn_player_characterinfo = reinterpret_cast<const DNPlayerCharacterInfo*>(player_character_info_);
    if (dn_player_characterinfo == nullptr){
        LOG(ERROR) << "[DNPlayer::GetNewestVersion()] empty DNPlayerCharacterInfo";
        return nullptr;
    }
	return dn_player_characterinfo->the_newest_version.c_str();
}

std::string DNPlayer::GetTradePassword() {
    const DNPlayerCharacterInfo* dn_player_characterinfo = (DNPlayerCharacterInfo*)(player_character_info_);
    if (dn_player_characterinfo == nullptr){
        LOG(ERROR) << "[DNPlayer::GetTradePassword()] empty DNPlayerCharacterInfo";
        return nullptr;
    }
    return dn_player_characterinfo->trade_password_;
}
void  DNPlayer::SetTradePassword(const std::string&password){
    DNPlayerCharacterInfo* dn_player_characterinfo = (DNPlayerCharacterInfo*)(player_character_info_);
    if (dn_player_characterinfo == nullptr){
        LOG(ERROR) << "[DNPlayer::SetTradePassword()] empty DNPlayerCharacterInfo";
        return;
    }
    dn_player_characterinfo->trade_password_     = password;
    dn_player_characterinfo->has_trade_password_ = true;
}
uint64_t DNPlayer::GetLeftTradeAmount(int32_t type, int32_t property_id){
    TradeService* tradeService = ServiceManager::instance()->LocateService<TradeService>("Trade");
    if (tradeService != nullptr)
    {
        DNPlayerCharacterInfo* dn_player_characterinfo = (DNPlayerCharacterInfo*)(player_character_info_);
        uint64_t maxdailyTradeAmount = tradeService->GetMaxDailyTradeAmount(type,property_id,player_character_info_->AccountID);
        return maxdailyTradeAmount > dn_player_characterinfo->GetTradeCount(type, property_id) ? maxdailyTradeAmount - dn_player_characterinfo->GetTradeCount(type, property_id) : 0;
    }
    else
    {
        LOG(ERROR) << "DNPlayer::GetLeftTradeAmount get null TradeService";
        return 0;
    }
}

uint64_t DNPlayer::GetTradeAmount(int32_t type, int32_t property_id)
{
    TradeService* tradeService = ServiceManager::instance()->LocateService<TradeService>("Trade");
    if (tradeService != nullptr)
    {
        DNPlayerCharacterInfo* dn_player_characterinfo = (DNPlayerCharacterInfo*)(player_character_info_);
        return dn_player_characterinfo->GetTradeCount(type, property_id);
    }
    else
    {
        LOG(ERROR) << "DNPlayer::GetTradeAmount get null TradeService";
        return 0;
    }
}

uint32_t DNPlayer::GetMinTradeAmount(int32_t type, int32_t property_id)
{
    TradeService* tradeService = ServiceManager::instance()->LocateService<TradeService>("Trade");
    if (tradeService != nullptr)
    {
            return tradeService->GetMinTradeAmount(type,property_id,player_character_info_->AccountID);
    }
    else
    {
        LOG(ERROR) << "DNPlayer::GetMinimunTradeAmount get null TradeService";
        return 0;
    }
}
void  DNPlayer::IncreaseTradeAmount(int32_t type, int32_t property_id,const uint64_t& trade_amout)
{
    DNPlayerCharacterInfo* dn_player_characterinfo = (DNPlayerCharacterInfo*)(player_character_info_);
    if (dn_player_characterinfo == nullptr){
        LOG(ERROR) << "[DNPlayer::UpdateLeftTradeAmount()] empty DNPlayerCharacterInfo";
        return;
    }
    if (GetLeftTradeAmount(type,property_id) >= trade_amout)
    {
        dn_player_characterinfo->IncreaseTradeCount(type, property_id, trade_amout);
    }
    else
    {
        LOG(ERROR) << "[DNPlayer::UpdateLeftTradeAmount] wrong  trade_amount more than left_trade_amout";
    }
}

void   DNPlayer::SendTradeInfo()
{
    TradeService* tradeService = ServiceManager::instance()->LocateService<TradeService>("Trade");
    assert(tradeService);
    tradeService->SendTradeInfoToPlayer(this);
}

uint32_t DNPlayer::GetPrivilege()
{
    TradeService* tradeService = ServiceManager::instance()->LocateService<TradeService>("Trade");
    assert(tradeService);
    return tradeService->GetPrivilege(GetAccountId());
}

uint32_t DNPlayer::GetMaxTradeCount()
{
    TradeService* tradeService = ServiceManager::instance()->LocateService<TradeService>("Trade");
    assert(tradeService);
    return tradeService->GetMaxTradeCount();
}

void DNPlayer::LockPlayerStatus(){
    is_player_lock_ = true;
}
void DNPlayer::UnLockPlayerStatus(){
    is_player_lock_ = false;
}
bool DNPlayer::GetPlayerLockStatus(){
    return is_player_lock_;
}
void DNPlayer ::ResetAllPlayerExchangeInfo()
{
    std::map<uint32_t, player_item_exchange_limit_data> ExchangeItemList = GetItemSys()->GetExchangeItemList();
    ExchangeItemList.clear();
}
void DNPlayer::ResetTradeInfo(){
    DNPlayerCharacterInfo* dnChracterInfo = (DNPlayerCharacterInfo*)GetPlayerCharacterInfo();
    if (dnChracterInfo == nullptr){
        LOG(ERROR) << "[DNPlayer::ResetTradeInfo()] empty DNPlayerCharacterInfo";
        return;
    }
    dnChracterInfo->ResetTradeInfo();
}
void DNPlayer::SetPlayerWhetherHasTradePassWord(bool has_password){
    DNPlayerCharacterInfo* dnCharacterInfo = (DNPlayerCharacterInfo*)GetPlayerCharacterInfo();
    if (dnCharacterInfo == nullptr){
        LOG(ERROR) << "[DNPlayer::SetPlayerWhetherHasTradePassWord()] empty DNPlayerCharacterInfo";
        return;
    }
    dnCharacterInfo->has_trade_password_ = has_password;
}

void DNPlayer::SendSitDownRsp(int32_t err_code, uint8_t pos_id)
{
	SitDownResp resp; 
	resp.error_code = err_code;
	resp.pos_id = pos_id;
	PacketTranscode packet;
	
	resp.ToPacket(packet);

	packet.SetOpcode(SMSG_HUNDREDS_TABLE_SIT_RESP);
	SendMsgToClient(packet);
}

void DNPlayer::SendItemsExchangeResp(uint32_t error_status, uint32_t exchange_id)
{
    SMSG_ItemsExchangeResp itemExchangeResp;
    PacketTranscode packet;
    itemExchangeResp.error_code = error_status;
    itemExchangeResp.exchange_id = exchange_id;
    itemExchangeResp.ToPacket(packet);
    packet.SetOpcode(ItemExchangeOpcode::SMSG_ITEMS_EXCHANGE_RESP);
    SendMsgToClient(packet);
}

void DNPlayer::SendOpenTreasure(OpenDiamondTreasureResp& rep)
{
	PacketTranscode packet;
	packet.SetOpcode(SMSG_OPEN_DIAMOND_TREASURE_CHEST_INFO_RESP);
	rep.ToPacket(packet);
	SendMsgToClient(packet);
}

void DNPlayer::ReadTreasureInfo(DiamondTreasureInfoRsp& rep)
{
	vector<DiamondTreasure> &treasure_v = treasure_config_mgr::GetInstance()->ReadInfo();
	rep.treasure_v=treasure_v;
}

void DNPlayer::InitItemSys()
{
	assert(item_sys == nullptr);
	item_sys = new dn_item_sys(*this);
}

void DNPlayer::FinalItemSys()
{
	assert(item_sys != nullptr);
	if( item_sys )
	{
		delete item_sys;
		item_sys = nullptr;
	}
}

void DNPlayer::SendContinueLoginRewardList()
{
	GreenHandGiftItemList list;
	WriteInfo(list);

	PacketTranscode packet;
	list.ToPacket(packet);
	packet.SetOpcode( SMSG_GREEN_HAND_GIFT_LIST_NTF );

	SendMsgToClient(packet);
}


uint32_t DNPlayer::CanGetLoginReward(uint32_t reward_index)
{
	login_reward_config_data* p_data = LoginRewardMgr::GetInstance()->GetConfigDataByID( reward_index);
	if( p_data == nullptr )
	{
		return GET_REWARD_ERROR_INVALID_REWARD_ID;
	}

	const DNPlayerCharacterInfo* dn_player_info = (DNPlayerCharacterInfo*)(player_character_info_);
	uint32_t cur_get_reward_index = dn_player_info->last_get_login_reward_index_;
	uint32_t next_get_reward_index = cur_get_reward_index + 1;
	uint32_t continue_login_times = dn_player_info->new_continue_login_times_;
	
	login_reward_config_data* p_next_data = LoginRewardMgr::GetInstance()->GetConfigDataByID( next_get_reward_index );
	if( p_next_data  == nullptr)
	{
		return GET_REWARD_ERROR_GET_FINISH;
	}
	if( p_next_data->continue_login_day_ >  continue_login_times )
	{
		return GET_REWARD_ERROR_CAN_NOT_GET;
	}

	if( reward_index != next_get_reward_index )
	{
		return GET_REWARD_ERROR_CAN_NOT_GET;
	}

	return GET_REWARD_ERROR_NO_ERROR ;

}

uint32_t DNPlayer::OnGetLoginReward(uint32_t reward_index)
{
	std::string msg = LoginRewardMgr::GetInstance()->GetShowMsg();

	uint32_t err_code = CanGetLoginReward( reward_index);
	if( err_code != GET_REWARD_ERROR_NO_ERROR )
	{
		SendGetLoginRewardRsp( err_code, reward_index, 0, 0, 0, msg);
		return 0;
	}

	login_reward_config_data* data = LoginRewardMgr::GetInstance()->GetConfigDataByID( reward_index );
	assert(data);

	LuaService * lua = ServiceManager::instance()->LocateService<LuaService>("LuaService");
	assert(lua);
	err_code = lua->GetLoginReward( GetAccountId(),  reward_index,  data->reward_count_, msg);
	if( err_code == 0 )
	{
		DNPlayerCharacterInfo* dn_player_info = (DNPlayerCharacterInfo*)(player_character_info_);
		dn_player_info->last_get_login_reward_index_ = reward_index;
	}

	return 0;
}


void DNPlayer::SendGetLoginRewardRsp(uint32_t err_code, uint32_t reward_index, uint32_t reward_type, uint32_t reward_id, uint32_t reward_count,  std::string& msg)
{
	GetGreenHandGiftItemRsp rsp;
	rsp.err_code = err_code;
	rsp.gift_index = reward_id;
	rsp.reward_num = reward_count;
	rsp.msg = msg;
	rsp.reward_id = reward_id;
	rsp.reward_type = reward_type;

	PacketTranscode packet;
	rsp.ToPacket(packet);
	packet.SetOpcode(SMSG_GREEN_HAND_GET_GIFT_RSP);

	SendMsgToClient(packet);
}


void DNPlayer::WriteInfo( GreenHandGiftItemList& list )
{
	const DNPlayerCharacterInfo* dn_player_info = (DNPlayerCharacterInfo*)(player_character_info_);
		
	list.show_gold_count = LoginRewardMgr::GetInstance()->GetShowNum();
	list.gift_index = dn_player_info->last_get_login_reward_index_;

	std::map<uint32_t, login_reward_config_data> data_map = LoginRewardMgr::GetInstance()->GetDataMaps();

	std::map<uint32_t, login_reward_config_data>::iterator pItr = data_map.begin();

	for( ; pItr != data_map.end(); ++pItr)
	{
		const login_reward_config_data& r_data = pItr->second;
		GreenHandGiftItem item;
		item.item_id = r_data.show_item_id_;
		item.item_count = r_data.reward_count_;

		list.items.push_back( item );
	}
}

uint32_t DNPlayer::GetCurLoginRewardID()
{
	const DNPlayerCharacterInfo* dn_player_info = (DNPlayerCharacterInfo*)(player_character_info_);
	return dn_player_info->last_get_login_reward_index_;
}

bool DNPlayer::IsNeedShowLoginReward()
{
	uint32_t cur_reward_id = GetCurLoginRewardID();
	uint32_t next_reward_id = cur_reward_id + 1;
	uint32_t ret = CanGetLoginReward(next_reward_id);

	return ret == 0;
}

bool DNPlayer::IsFirstCharge()
{
    DNPlayerCharacterInfo* dnChracterInfo = (DNPlayerCharacterInfo*)GetPlayerCharacterInfo();
    if (dnChracterInfo == nullptr){
        LOG(ERROR) << "[DNPlayer::IsFirstCharge()] empty DNPlayerCharacterInfo";
        return false;
    }
    return dnChracterInfo->first_charge==0? true:false;
}
void DNPlayer::UpdateFirstCharge()
{
    DNPlayerCharacterInfo* dnChracterInfo = (DNPlayerCharacterInfo*)GetPlayerCharacterInfo();
    if (dnChracterInfo == nullptr){
        LOG(ERROR) << "[DNPlayer::UpdateFirstCharge()] empty DNPlayerCharacterInfo";
        return ;
    }
    if (dnChracterInfo->first_charge != 1)
    {
        dnChracterInfo->first_charge = 1 ;
    }
}

int32_t DNPlayer::GetPlayerMatchType()
{
    DNTable* table = FindTable(this);
    if (table == nullptr)
    {
        LOG(ERROR) << " DNPlayer::GetPlayerStatusOnTable hit empity table";
        return 0;
    }
    return table->GetContestInfo()->matchType;
}

int32_t DNPlayer::GetChargeMoneyTimes(int32_t money)
{
    DNPlayerCharacterInfo* dnChracterInfo = (DNPlayerCharacterInfo*)GetPlayerCharacterInfo();
    if (dnChracterInfo == nullptr){
        LOG(ERROR) << "[DNPlayer::UpdateFirstCharge()] empty DNPlayerCharacterInfo";
        return 0;
    }
    return  dnChracterInfo->GetChargeMoneyTimes(money);
}

void DNPlayer::ResetChargeMoneyTimes(int32_t money)
{
    DNPlayerCharacterInfo* dnChracterInfo = (DNPlayerCharacterInfo*)GetPlayerCharacterInfo();
    if (dnChracterInfo == nullptr){
        LOG(ERROR) << "[DNPlayer::UpdateFirstCharge()] empty DNPlayerCharacterInfo";
        return ;
    }
    dnChracterInfo->ResetChargeMoneyTimes(money);
}
