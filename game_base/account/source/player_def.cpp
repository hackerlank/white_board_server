/*
 * player_def.cpp
 *
 *  Created on: Feb 21, 2014
 *      Author: vagrant
 */
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "logging.h"

#include "player_def.h"
#include "transcode_packet.h"
#include "CbFormat.h"
#include "logging.h"
#include <time.h> 

const std::string CG_Options::kUDID = "isu";
const std::string CG_Options::kDeviceId = "did";
const std::string CG_Options::kDeviceModel = "model";
const std::string CG_Options::kAppName = "pn";
const std::string CG_Options::kAppVersion = "appvn";
const std::string CG_Options::kAppVersionCode = "appvc";
const std::string CG_Options::kChannelId = "chn";
const std::string CG_Options::kCountry = "cc";
const std::string CG_Options::kOS = "os";
const std::string CG_Options::kOsVersion = "osv";
const std::string CG_Options::kLanguage = "lang";
const std::string CG_Options::kCpu = "cpu";
const std::string CG_Options::kResolution = "res";
const std::string CG_Options::kAccess = "nt";
const std::string CG_Options::kCarrier = "no";
const std::string CG_Options::kLongitude = "lon";
const std::string CG_Options::kLatitude = "lat";
const std::string CG_Options::kCategory = "ctg";
const std::string CG_Options::kAction = "act";
const std::string CG_Options::kLabel = "lab";
const std::string CG_Options::kValue = "val";
const std::string CG_Options::kPushDeviceToken = "pdid";
const std::string CG_Options::kAccId = "acc_id";
const std::string CG_Options::kAccType = "acc_type";
const std::string CG_Options::kPhoneNumber = "ph_num";
const std::string CG_Options::kDeviceType = "vm";
const std::string CG_Options::kCheckSum = "check_sum";


const std::string CG_Options::OS_WINDOWS = "MS WINDOWS";
const std::string CG_Options::OS_IOS = "iOS";
const std::string CG_Options::OS_ANDROID_SYS = "Android";
const std::string CG_Options::OS_WINDOWS_PHONE = "Android";
const std::string CG_Options::OS_ANDROID_BAIDU = "Android_baidu";

CG_Options::CG_Options() {

}

CG_Options::CG_Options(const GameOptions& options) : m_Options_(options) {

}

void CG_Options::ToPacket(PacketTranscode& packet) {
	packet << (uint32_t)m_Options_.size();
	for (GameOptions::iterator it = m_Options_.begin(); it != m_Options_.end(); ++it)
	{
		packet << it->first << it->second;
	}
}

void CG_Options::FromPacket(PacketTranscode& packet) {
	uint32_t nSize;
	packet >> nSize;
    const static uint32_t GC_OPTION_NUMS = 1000;  // 不能超过1000 为了防止恶意攻击
    if (nSize > GC_OPTION_NUMS){
        LOG(ERROR) << "[CG_Options::FromPacket] the size is out of ranage";
        nSize = 0;
    }
	for(uint16_t i = 0; i < nSize; ++i)
	{
		std::string key;
		std::string value;
		packet >> key >> value;
		m_Options_[key] = value;
	}
}

std::string CG_Options::GetValue(const std::string &key) {

	std::map<std::string, std::string>::const_iterator it = m_Options_.find(std::string(key));
	if (it == m_Options_.end()) 
	{
		return "default";
	}
	return it->second;
}

AccountInfo::AccountInfo()
: accountID(0)
, account("")
, password("")
, nick("")
, icon(1)
, sex(PLAYER_SEX_MALE)
, birthday(0)
, thirdlogin("")
, avatar("") {

}

void AccountInfo::ToSimplyPacket(PacketTranscode &resp) {
	resp << accountID <<nick <<  avatar;
}

void AccountInfo::FromSimplyPacket(PacketTranscode &request) {
	request >> accountID >> nick >> avatar;
}

void AccountInfo::ToPacket(PacketTranscode &resp) {
	resp << accountID << account << password << nick << icon << sex << birthday << thirdlogin << avatar;
}

void AccountInfo::FromPacket(PacketTranscode &request) {
	request >> accountID >> account >> password >> nick >> icon >> sex >> birthday >> thirdlogin >> avatar;
}

void AccountInfo::ToJson(rapidjson::Value& json, rapidjson::Value::AllocatorType& allocator) {
	json.SetObject();

	rapidjson::Value nick_value(nick.c_str(), allocator);
	json.AddMember("nick", nick_value, allocator);
	json.AddMember("icon", icon, allocator);
	json.AddMember("sex", sex , allocator);
	json.AddMember("birthday", birthday, allocator);

	rapidjson::Value third_login_value(thirdlogin.c_str(), allocator);
	json.AddMember("thirdlogin ", third_login_value, allocator);

	rapidjson::Value avatar_value(avatar.c_str(), allocator);
	json.AddMember("avatar", avatar_value, allocator);
}

void AccountInfo::FromJson(rapidjson::Value& json) {
	if (!json.IsObject()) {
		LOG(ERROR) << "player account info is not a object, json type is " << json.GetType();
		return ;
	}
	if (json.HasMember("nick") && json["nick"].IsString()) 
	{
		nick = json["nick"].GetString();
	}
	if (json.HasMember("icon") && json["icon"].IsInt()) {
		icon = json["icon"].GetInt();
	}
	if (json.HasMember("sex") && json["sex"].IsInt()) {
		sex = json["sex"].GetInt();
	}
	if (json.HasMember("birthday") && json["birthday"].IsInt()) {
		birthday = json["birthday"].GetInt();
	}
	if (json.HasMember("thirdlogin") && json["thirdlogin"].IsString()) {
		thirdlogin = json["thirdlogin"].GetString();
	}
	if (json.HasMember("avatar") && json["avatar"].IsString()) {
		avatar = json["avatar"].GetString();
	}
}

void AccountInfo::SetThirdLogin(const std::string& third) {
    if (thirdlogin.empty()) {
        thirdlogin = third;
    } else {
        thirdlogin.append("|").append(third);
    }
}

PlayerPropTable::PlayerPropTable () : accountId(0) {

}

uint32_t PlayerPropTable::GetHorns() const {
	PlayerPropMap::const_iterator it = propMap_.find(PROPERTY_HORN);
	if (it != propMap_.end()) 
	{
		return it->second;
	} 
	else 
	{
		return 0;
	}
}

void PlayerPropTable::ChangeHorns(uint32_t change) 
{
	uint32_t ret = 0;
	if (change > 0 || GetHorns() > -change) 
	{
		ret = GetHorns() + change;
	}
	propMap_[PROPERTY_HORN] = ret;
}

void PlayerPropTable::ToPacketEx(PacketTranscode& packet)
{
	packet << accountId;
    //为了兼容客户端  PlayerPropMap encode时不包含PROPERTY_CAISHEN_PLAYED_TIMES 和PROPERTY_MESSAGE_CHARGE_FEE
    PlayerPropMap prop_map_tmp;
    prop_map_tmp.insert(propMap_.begin(), propMap_.end());
    prop_map_tmp.erase(PROPERTY_CAISHEN_PLAYED_TIMES);
    prop_map_tmp.erase(PROPERTY_MESSAGE_CHARGE_FEE);
    int32_t count = prop_map_tmp.size();
	packet << count;
    for (PlayerPropMap::const_iterator it = prop_map_tmp.begin(); it != prop_map_tmp.end(); ++it)
	{
        packet << it->first << it->second;
	}
}

void PlayerPropTable::ToPacket(PacketTranscode& packet)
{
	packet << accountId;

	PlayerPropMap prop_map_tmp;
	prop_map_tmp.insert(propMap_.begin(), propMap_.end());
	prop_map_tmp.erase(PROPERTY_CAISHEN_PLAYED_TIMES);
	prop_map_tmp.erase(PROPERTY_MESSAGE_CHARGE_FEE);
	int32_t count = prop_map_tmp.size();
	packet << count;
	for (PlayerPropMap::const_iterator it = prop_map_tmp.begin(); it != prop_map_tmp.end(); ++it)
	{    
		packet << it->first << (int32_t)it->second;
	}    
}

void PlayerPropTable::FromPacketEx(PacketTranscode& packet)
{

}

void PlayerPropTable::FromPacket(PacketTranscode& packet)
{

}

void PlayerPropTable::ToJson(std::string& req) 
{
	req.append("{");
	auto it_end = propMap_.end();
	auto it = propMap_.begin();
	if (it != it_end) 
	{
		char propertyId[1024] = { 0 };
		if (it->second >=0) 
		{
			snprintf(propertyId, sizeof(propertyId), "\"%d\":%ld", it->first, it->second);
			req.append(propertyId);
		}
		for (++it; it != it_end; ++it) 
		{
			if (it->second >= 0) 
			{
				snprintf(propertyId, sizeof(propertyId), ",\"%d\":%ld", it->first, it->second);
				req.append(propertyId);
			}
		}
	}

	req.append("}");
}

void PlayerPropTable::FromJson(rapidjson::Value& resp) 
{	
	if (resp.IsString()) 
	{
		std::string jsonStr = resp.GetString();
		rapidjson::Document json;
		json.Parse<0>(jsonStr.c_str());
		if (!json.IsObject()) 
		{
			return;
		}
		auto it_end = json.MemberEnd();
		for (auto it = json.MemberBegin(); it != it_end; ++it) 
		{
			if (it->name.IsString()) 
			{
				propMap_[atoi(it->name.GetString())] = it->value.GetInt64();
			}
			else 
			{
				continue;
			}
		}
	}
	else
	{
		LOG(INFO) << "[PlayerPropTable::FromJson]  resp is not string";
	}
}

stPlayerInvite::stPlayerInvite()
: TypeInvite(Invite_Type_Coins)
, TypeInvited(Invite_Type_Coins)
, RewardInvite(2000)
, RewardInvited(10000)
{

}

void stPlayerInvite::ToPacket(PacketTranscode& packet)
{
	packet << MyInviteCount << Invited << TypeInvite << TypeInvited << RewardInvite << RewardInvited;
}

void stPlayerInvite::FromPacket(PacketTranscode& packet)
{
	packet >> MyInviteCount >> Invited >> TypeInvite >> TypeInvited >> RewardInvite >> RewardInvited;
}



Player* PlayerCharacterInfo::GetOwner()
{
    return owner_;
}

PlayerCharacterInfo::PlayerCharacterInfo(Player* player)
    : owner_(player)
    , GoldCoins(0)
	, diamonds(0)
	, AccountID(0)
	, GoldNote(0)
	, Played(0)
	, Wined(0)
	, MaxWin(0)
	, MaxLose(0)
	, onlineSpree(0)
	, onlineSpreeTime(0)
	, nextSpreeRemaningTime(0)
    , onlineSpreeSum(0)
	, rankTime(0)
	, dayMaxwin(0)
    , thisDayMaxWin(0)
    , thisDayCaishen(0)
    , thisDayWanRen(0)
    , thisDayPayment(0)
    , yesterdayPayment(0)
    , lastTimeOfDayMaxWin(0)
    , lastTimeOfDayCaishen(0)
    , lastTimeOfDayWanRen(0)
    , lastTimeOfDayPayment(0)
	, rank(0)

	, currentGuaranteeTimes(0)
	, maxGuaranteeBasicLivingTimes(GAME_GUARANTEEBASICLIVING_MAX_TIMES_PER_DAY)
	, lastTimeOfResetGuaranteeBasicLivingTimes(0)
	, raffle(0)
	, raffleTime(0)
	, maxFillCoins(0)

	, nextSpreeValue(0)
	, lastTimeOfMoneyTreeRock(0)
	, settings(0)
	, LastMatchType(0)
	, continuousLogin(1)
	, playerPropTable(nullptr)
	, playerTaskInfoMap(nullptr)
	, Title("乞丐")
{
	playerPropTable = new PlayerPropTable();

	playerTaskInfoMap = new std::string();

    all_activity_info = new AllActivity();
	// test caishen and money tree
	playerPropTable->propMap_[PROPERTY_GOLD_COINS] =	20000;
	playerPropTable->propMap_[PROPERTY_GOLD_NOTES] = 0;
	playerPropTable->propMap_[PROPERTY_DIAMONDS] = 0;
	playerPropTable->propMap_[PROPERTY_HORN] = 5;
	playerPropTable->propMap_[PROPERTY_TOTAL_CHARGE_FEE] = 0;
	playerPropTable->propMap_[PROPERTY_TICKET] = 0;
	playerPropTable->propMap_[PROPERTY_VIP] = 0;
    playerPropTable->propMap_[PROPERTY_MESSAGE_CHARGE_FEE] = 0;
    playerPropTable->propMap_[PROPERTY_CAISHEN_PLAYED_TIMES] = 0;
	SyncProPertyToCharacterInfo();
}

PlayerCharacterInfo::~PlayerCharacterInfo() {
	if (playerPropTable != nullptr) {
		delete playerPropTable;
		playerPropTable = nullptr;
	}
	if (playerTaskInfoMap != nullptr) {
		delete playerTaskInfoMap;
		playerTaskInfoMap = nullptr;
	}

    owner_ = nullptr;
}

void PlayerCharacterInfo::ToPacketEx(PacketTranscode& packet)
{

}


void PlayerCharacterInfo::ToPacket(PacketTranscode& packet)
{

}

void PlayerCharacterInfo::ToPacketEx64(PacketTranscode& packet)
{

}

void PlayerCharacterInfo::ToSimplePacket( PacketTranscode& packet )
{
	uint32_t goids = 0;
	uint32_t diamonds = 0;
	uint32_t vip = 0;
	auto it = playerPropTable->propMap_.find(PROPERTY_GOLD_COINS);
	if (it != playerPropTable->propMap_.end())
	{
		goids = it->second;
	}
	else
	{
		goids = 0;
	}

	it = playerPropTable->propMap_.find(PROPERTY_DIAMONDS);
	if (it != playerPropTable->propMap_.end())
	{
		diamonds = it->second;
	}
	else
	{
		diamonds = 0;
	}

	it = playerPropTable->propMap_.find(PROPERTY_VIP);
	if (it != playerPropTable->propMap_.end())
	{
		vip = it->second;
	}
	else
	{
		vip = 0;
	}
	packet << goids << diamonds <<vip;
}

void PlayerCharacterInfo::ToSimplePacketEx64(PacketTranscode& packet)
{
}

void PlayerCharacterInfo::ToSimplePacketEx(PacketTranscode& packet)
{
	uint64_t goids = 0;
	uint64_t diamonds = 0;
	uint32_t vip = 0;
	auto it = playerPropTable->propMap_.find(PROPERTY_GOLD_COINS);
	if (it != playerPropTable->propMap_.end())
	{   
		goids = it->second;
	}   
	else
	{   
		goids = 0;
	}   

	it = playerPropTable->propMap_.find(PROPERTY_DIAMONDS);
	if (it != playerPropTable->propMap_.end())
	{   
		diamonds = it->second;
	}   
	else
	{   
		diamonds = 0;
	}   

	it = playerPropTable->propMap_.find(PROPERTY_VIP);
	if (it != playerPropTable->propMap_.end())
	{   
		vip = it->second;
	}   
	else
	{   
		vip = 0;
	}   
	packet << goids << diamonds <<vip;
}

void PlayerCharacterInfo::ToJson(Player* player, 
								 rapidjson::Value& request, 
								 std::string& propJson, 
								 rapidjson::Value::AllocatorType& allocator) 
{
	assert(false);
}

void PlayerCharacterInfo::ToJson(rapidjson::Value& request, std::string& propJson, rapidjson::Value::AllocatorType& allocator) {
	//convert to json
	assert(playerPropTable != nullptr && playerTaskInfoMap != nullptr);

	request.SetObject();
	request.AddMember("played", Played, allocator);
	request.AddMember("wined", Wined, allocator);
	request.AddMember("maxwin", MaxWin, allocator);
	request.AddMember("maxlose", MaxLose, allocator);
	request.AddMember("onlineSpree", onlineSpree, allocator);
	request.AddMember("online_spree_time", onlineSpreeTime, allocator);
	request.AddMember("nextSpreeRemaningTime", nextSpreeRemaningTime, allocator);
    request.AddMember("online_spree_sum", onlineSpreeSum, allocator);
	request.AddMember("rank_time", rankTime, allocator);
	request.AddMember("allday_maxwin", dayMaxwin, allocator);
    request.AddMember("day_maxwin", thisDayMaxWin, allocator);
    request.AddMember("day_caishen", thisDayCaishen, allocator);
    request.AddMember("day_wanren",thisDayWanRen,allocator);
    request.AddMember("day_payment",thisDayPayment,allocator);
    request.AddMember("yesterday_payment", yesterdayPayment,allocator);
    request.AddMember("lastTimeOfDayMaxWin", lastTimeOfDayMaxWin, allocator);
    request.AddMember("lastTimeOfDayCaishen", lastTimeOfDayCaishen, allocator);
    request.AddMember("lastTimeOfWanRen",lastTimeOfDayWanRen,allocator);
    request.AddMember("lastTimeOfDayPayment",lastTimeOfDayPayment,allocator);
	request.AddMember("rank", rank, allocator);
	request.AddMember("current_guarantee_basic_living_times", currentGuaranteeTimes, allocator);
	request.AddMember("maxGuaranteeBasicLivingTimes", maxGuaranteeBasicLivingTimes, allocator);
	request.AddMember("last_time_of_reset_guarantee_basic_living_times", lastTimeOfResetGuaranteeBasicLivingTimes, allocator);
	request.AddMember("raffle", raffle, allocator);
	request.AddMember("raffle_time", raffleTime, allocator);
	request.AddMember("maxFillCoins", maxFillCoins, allocator);
	request.AddMember("nextSpreeValue", nextSpreeValue, allocator);
	request.AddMember("lastTimeOfMoneyTreeRock", lastTimeOfMoneyTreeRock, allocator);
	request.AddMember("settings", settings, allocator);
	request.AddMember("last_match_type", LastMatchType, allocator);
	request.AddMember("continuous_login", continuousLogin, allocator);
	request.AddMember("state", 0, allocator);
	rapidjson::Value title_value(Title.c_str(), allocator);
	request.AddMember("Title", title_value, allocator);
	rapidjson::Value my_invite_count_value(Invite.MyInviteCount.c_str(), allocator);
	request.AddMember("invite", my_invite_count_value, allocator);
	rapidjson::Value invited_value(Invite.Invited.c_str(), allocator);
	request.AddMember("invited", invited_value, allocator);
  
    all_activity_info->ToJson(request, allocator);

	assert(playerPropTable != nullptr);
	playerPropTable->ToJson(propJson);

	rapidjson::Value property_info_value(propJson.c_str(), allocator);
	request.AddMember("propertyInfo", property_info_value, allocator);

	rapidjson::Value task_info_value(playerTaskInfoMap->c_str(), allocator);
	request.AddMember("playerTaskInfoMap", task_info_value, allocator);
}


void PlayerCharacterInfo::FromJson(Player* player, rapidjson::Value& resp) 
{
	assert(false);
}

void PlayerCharacterInfo::FromJson(rapidjson::Value& resp) {
	assert(playerPropTable != nullptr);
//	rapidjson::StringBuffer buffer;
//	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//	resp.Accept(writer);
//
//	LOG(INFO) << buffer.GetString();
//	LOG(INFO) << "Get CharacterInfo success!";
	if (!resp.IsObject()) {
		LOG(INFO) << "player character info is not a object";
		return ;
	}
	if (resp.HasMember("played") && resp["played"].IsInt()) {
		Played = resp["played"].GetInt();
	}
	if (resp.HasMember("wined") && resp["wined"].IsInt()) {
		Wined = resp["wined"].GetInt();
	}
	if (resp.HasMember("maxwin") && resp["maxwin"].IsInt64()) {
		MaxWin = resp["maxwin"].GetInt64();
	}
	if (resp.HasMember("maxlose") && resp["maxlose"].IsInt64()) {
		MaxLose = resp["maxlose"].GetInt64();
	}
	if (resp.HasMember("onlineSpree") && resp["onlineSpree"].IsInt()) {
		onlineSpree = resp["onlineSpree"].GetInt();
	}
	if (resp.HasMember("online_spree_time") && resp["online_spree_time"].IsInt()) {
		onlineSpreeTime = resp["online_spree_time"].GetInt();
	}
	if (resp.HasMember("nextSpreeRemaningTime") && resp["nextSpreeRemaningTime"].IsInt()) {
		nextSpreeRemaningTime = resp["nextSpreeRemaningTime"].GetInt();
	}
    if (resp.HasMember("online_spree_sum") && resp["online_spree_sum"].IsInt()){
        onlineSpreeSum = resp["online_spree_sum"].GetInt();
    }
	if (resp.HasMember("rank_time") && resp["rank_time"].IsInt()) {
		rankTime = resp["rank_time"].GetInt();
	}
	if (resp.HasMember("allday_maxwin") && resp["allday_maxwin"].IsInt64()) {
		dayMaxwin = resp["allday_maxwin"].GetInt64();
	}
    if (resp.HasMember("day_maxwin") && resp["day_maxwin"].IsInt64()) {
        thisDayMaxWin = resp["day_maxwin"].GetInt64();
    }
    if (resp.HasMember("day_caishen") && resp["day_caishen"].IsInt64()) {
        thisDayCaishen = resp["day_caishen"].GetInt64();

    }
    if (resp.HasMember("day_wanren") && resp["day_wanren"].IsInt64()) {
        thisDayWanRen = resp["day_wanren"].GetInt64();
    }
    if (resp.HasMember("day_payment") && resp["day_payment"].IsInt64()) {
        thisDayPayment = resp["day_payment"].GetInt64();
    }
    if (resp.HasMember("yesterday_payment") && resp["yesterday_payment"].IsInt64()) {
        yesterdayPayment = resp["yesterday_payment"].GetInt64();
    }    
    if (resp.HasMember("lastTimeOfDayMaxWin") && resp["lastTimeOfDayMaxWin"].IsInt()) {
        lastTimeOfDayMaxWin = resp["lastTimeOfDayMaxWin"].GetInt();
    }
    if (resp.HasMember("lastTimeOfDayCaishen") && resp["lastTimeOfDayCaishen"].IsInt()) {
        lastTimeOfDayCaishen = resp["lastTimeOfDayCaishen"].GetInt();
    }
    if (resp.HasMember("lastTimeOfWanRen") && resp["lastTimeOfWanRen"].IsInt()) {
        lastTimeOfDayWanRen = resp["lastTimeOfWanRen"].GetInt();
    }
    if (resp.HasMember("lastTimeOfDayPayment") && resp["lastTimeOfDayPayment"].IsInt()) {
        lastTimeOfDayPayment = resp["lastTimeOfDayPayment"].GetInt();
    }
	if (resp.HasMember("rank") && resp["rank"].IsInt()) {
		rank = resp["rank"].GetInt();
	}
	if (resp.HasMember("current_guarantee_basic_living_times") && resp["current_guarantee_basic_living_times"].IsInt()) {
		currentGuaranteeTimes = resp["current_guarantee_basic_living_times"].GetInt();
	}
	if (resp.HasMember("maxGuaranteeBasicLivingTimes") && resp["maxGuaranteeBasicLivingTimes"].IsInt()) {
		maxGuaranteeBasicLivingTimes = resp["maxGuaranteeBasicLivingTimes"].GetInt();
	}
	if (resp.HasMember("last_time_of_reset_guarantee_basic_living_times") && resp["last_time_of_reset_guarantee_basic_living_times"].IsInt64()) {
		lastTimeOfResetGuaranteeBasicLivingTimes = resp["last_time_of_reset_guarantee_basic_living_times"].GetInt64();
	}
	if (resp.HasMember("raffle") && resp["raffle"].IsInt()){
		raffle = resp["raffle"].GetInt();
	}
	if (resp.HasMember("raffle_time") && resp["raffle_time"].IsInt()) {
		raffleTime = resp["raffle_time"].GetInt();
	}
	if (resp.HasMember("maxFillCoins") && resp["maxFillCoins"].IsInt()) {
		maxFillCoins = resp["maxFillCoins"].GetInt();
	}
	if (resp.HasMember("nextSpreeValue") && resp["nextSpreeValue"].IsInt()) {
		nextSpreeValue = resp["nextSpreeValue"].GetInt();
	}
	if (resp.HasMember("lastTimeOfMoneyTreeRock") && resp["lastTimeOfMoneyTreeRock"].IsInt()) {
		lastTimeOfMoneyTreeRock = resp["lastTimeOfMoneyTreeRock"].GetInt();
	}
	if (resp.HasMember("settings") && resp["settings"].IsInt()) {
		settings = resp["settings"].GetInt();
	}
	if (resp.HasMember("last_match_type") && resp["last_match_type"].IsInt()) {
		LastMatchType = resp["last_match_type"].GetInt();
	}
	if (resp.HasMember("continuous_login") && resp["continuous_login"].IsInt()) {
		continuousLogin = resp["continuous_login"].GetInt();
	}

	if (resp.HasMember("propertyInfo")) {
		playerPropTable->FromJson(resp["propertyInfo"]);
		SyncProPertyToCharacterInfo();
	}
	else
	{
		LOG(ERROR) << "[PlayerCharacterInfo::FromJson]" << "this json doesnot have propertyInfo";
	}
	if (resp.HasMember("playerTaskInfoMap") && resp["playerTaskInfoMap"].IsString()) {
		playerTaskInfoMap->clear();
		*playerTaskInfoMap =  resp["playerTaskInfoMap"].GetString();
	}

	if (resp.HasMember("Title") && resp["Title"].IsString()) {
		Title = resp["Title"].GetString();
	}
	if (resp.HasMember("invite") && resp["invite"].IsString()) {
		Invite.MyInviteCount = resp["invite"].GetString();
	}
	if (resp.HasMember("invited") && resp["invited"].IsString()) {
		Invite.Invited = resp["invited"].GetString();
	}
    if (resp.HasMember("activity") && resp["activity"].IsObject()){
        all_activity_info->FromJson(resp["activity"]);
    }
}

void PlayerCharacterInfo::SyncProPertyToCharacterInfo() {
	if (playerPropTable != nullptr) {
		auto it = playerPropTable->propMap_.find(PROPERTY_GOLD_COINS);
		if (it != playerPropTable->propMap_.end()) {
			GoldCoins = it->second;
		} else {
			GoldCoins = 0;
		}
		it = playerPropTable->propMap_.find(PROPERTY_GOLD_NOTES);
		if (it != playerPropTable->propMap_.end()) {
			GoldNote = it->second;
		} else {
			GoldNote = 0;
		}
		it = playerPropTable->propMap_.find(PROPERTY_TICKET);
		if (it != playerPropTable->propMap_.end()) {
			raffle = it->second;
		} else {
			raffle = 0;
		}
		it = playerPropTable->propMap_.find(PROPERTY_DIAMONDS);
		if (it != playerPropTable->propMap_.end()) {
			diamonds = it->second;
		} else {
			diamonds = 0;
		}
	}
}

void AccountCharacterInfoResp::ToPacket(PacketTranscode& packet)
{
	assert (account != nullptr && character != nullptr);
	account->ToPacket(packet);
	character->ToPacket(packet);
	character->playerPropTable->ToPacket(packet);
	packet << clientId;
	options.ToPacket(packet);
}

void AccountCharacterInfoResp::ToPacketEx(PacketTranscode& packet)
{
	assert (account != nullptr && character != nullptr);
	account->ToPacket(packet);
	character->ToPacketEx(packet);
	character->playerPropTable->ToPacket(packet);
	packet << clientId;
	options.ToPacket(packet);
	packet << stay_hall;
	packet << rc6_key;
	packet << item_xml_version;
	packet << first_login_flag;
}

void AccountCharacterInfoResp::ToPacketEx64(PacketTranscode& packet)
{
    assert(account != nullptr && character != nullptr);
    account->ToPacket(packet);
    character->ToPacketEx64(packet);
    character->playerPropTable->ToPacketEx(packet);
    packet << clientId;
    options.ToPacket(packet);
    packet << stay_hall;
	packet << rc6_key;
	packet << item_xml_version;
	packet << first_login_flag;
	packet << cur_tm;
	packet << session_id;
	packet << recharge_rmb;
	packet << month_recharge_rmb;
	packet << recharge_sms_rmb;
    packet << is_gold_trader_;//for 2.8.0 version
    packet << max_trade_count;//for 2.8.0 version 
    packet << download_url;//for 2.9.0 version ;dynamic resource download
}

void  AccountCharacterInfoResp::ToPacketWithVersion( PacketTranscode& packet,  const std::string& version)
{
	if( version >= SVR_32_VERSION )
	{    
		return ToPacketEx64(packet);
	}    
	else if( version >= "2.6.0")
	{    
		return ToPacketEx(packet);
	}    
	return ToPacket(packet);
}

void AccountCharacterInfoResp::FromPacket(PacketTranscode& packet)
{

}

void AccountCharacterInfoResp::FromPacketEx(PacketTranscode& packet)
{
}

void AccountCharacterInfoResp::FromPacketEx64(PacketTranscode& packet)
{

}

void stOnePropertyChange::ToPacket(PacketTranscode& packet)
{
	packet << propertyId << uint32_t(currentValue) << int32_t(changedValue);
	int32_t otherValuesCount = otherValues.size();
	packet << otherValuesCount;
	for (std::vector<int64_t>::iterator vit = otherValues.begin(); vit != otherValues.end(); ++vit) 
	{
		packet << (uint32_t)(*vit);
	}
}

void stOnePropertyChange::FromPacket(PacketTranscode& packet)
{

}

void stOnePropertyChange::ToPacketEx(PacketTranscode& packet)
{
	packet << propertyId << currentValue << changedValue;
	int32_t otherValuesCount = otherValues.size();
	packet << otherValuesCount;
	for (std::vector<int64_t>::iterator vit = otherValues.begin(); vit != otherValues.end(); ++vit) 
	{
		packet << *vit;
	}
}

void stOnePropertyChange::FromPacketEx(PacketTranscode& packet)
{
	/*	
	packet >> propertyId >> currentValue >> changedValue;
	int64_t otherValuesCount = 0;
	packet >> otherValuesCount;
	for (int32_t i = 0; i < otherValuesCount; i++) 
	{
		int64_t v = 0;
		packet >> v;
		otherValues.push_back(v);
	}
	*/
}

void PlayerPropertyChange::ToPacket(PacketTranscode& packet)
{
	packet << uPlayerID << szTitle << uChangeReason;
	int32_t propsCount = changedProps.size();
	packet << propsCount;
	for (std::map<stPropertyId, stOnePropertyChange>::iterator it = changedProps.begin(); it != changedProps.end(); ++it)
	{
		it->second.ToPacket(packet);
	}
}

void PlayerPropertyChange::ToPacketEx(PacketTranscode& packet)
{
	packet << uPlayerID << szTitle << uChangeReason;
	int32_t propsCount = changedProps.size();
	packet << propsCount;
	for (std::map<stPropertyId, stOnePropertyChange>::iterator it = changedProps.begin(); it != changedProps.end(); ++it)
	{
		it->second.ToPacketEx(packet);
	}
}
     
 
void PlayerPropertyChange::FromPacketEx(PacketTranscode& packet)
{
	packet >> uPlayerID >> szTitle >> uChangeReason;
	int32_t propsCount = 0;
	packet >> propsCount;
	for (int32_t i = 0; i < propsCount; i++)
	{
		stOnePropertyChange v;
		v.FromPacketEx(packet);
		changedProps[v.propertyId] = v;
	}
}


void PlayerPropertyChange::FromPacket(PacketTranscode& packet)
{
	packet >> uPlayerID >> szTitle >> uChangeReason;

	int32_t propsCount = 0;
	packet >> propsCount;
	for (int32_t i = 0; i < propsCount; i++)
	{
		stOnePropertyChange v;
		v.FromPacket(packet);
		changedProps[v.propertyId] = v;
	}
}

void PlayerGetSpree::ToPacket(PacketTranscode& packet)
{
    packet << error_code << next_spree_time << next_spree_value;
}

void PlayerGetSpree::FromPacket(PacketTranscode& packet)
{
    packet >> error_code >> next_spree_time >> next_spree_value;
}

void PlayerGetSpreeLimited::ToPacket(PacketTranscode& packet)
{
    packet << error_code << next_spree_time << next_spree_value << spree_sum_now << spree_sum_limit;
}

void PlayerGetSpreeLimited::FromPacket(PacketTranscode& packet)
{
    packet >> error_code >> next_spree_time >> next_spree_value >> spree_sum_now >> spree_sum_limit;
}

void OfflinePlayerPropertyChange::ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator)
{
	request.SetObject();
	request.AddMember("playerId", player_id, allocator);
	request.AddMember("propertyId", property_id, allocator);
	request.AddMember("change", change, allocator);
	request.AddMember("changeType", static_cast<int32_t>(change_type), allocator);
	request.AddMember("flags", flags, allocator);
	request.AddMember("payMoney", pay_money, allocator);
	request.AddMember("id", id, allocator);
}

void OfflinePlayerPropertyChange::FromJson(rapidjson::Value& resp)
{
	if (!resp.IsObject()) {
		LOG(INFO) << "offline player property change info is not a object";
		return;
	}
	if (resp.HasMember("playerId") && resp["playerId"].IsInt()) {
		player_id = resp["playerId"].GetUint();
	}
	if (resp.HasMember("propertyId") && resp["propertyId"].IsInt()) {
		property_id = resp["propertyId"].GetInt();
	}
	if (resp.HasMember("change") && resp["change"].IsInt()) {
		change = resp["change"].GetInt();
	}
	if (resp.HasMember("changeType") && resp["changeType"].IsInt()) {		
		change_type = resp["changeType"].GetInt();
	}
	if (resp.HasMember("flags") && resp["flags"].IsInt()) {
		flags = resp["flags"].GetInt();
	}
	if (resp.HasMember("payMoney") && resp["payMoney"].IsNumber()) {
		pay_money = resp["payMoney"].GetDouble();
	}
	if (resp.HasMember("id") && resp["id"].IsNumber()) {
		id = resp["id"].GetInt();
	}
}

void GetOfflineChangePropertyResp::ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator)
{
	total_count = property_change_list.size();

	request.SetObject();
	request.AddMember("count", total_count, allocator);
	
	rapidjson::Value property_changes;
	property_changes.SetArray();

	for (auto it = property_change_list.begin(); it != property_change_list.end(); ++it)
	{
		rapidjson::Value property_change;
		property_change.SetObject();
		(*it).ToJson(property_change, allocator);
		property_changes.PushBack(property_change, allocator);
	}

	request.AddMember("propertyChanges", property_changes, allocator);
}

void GetOfflineChangePropertyResp::FromJson(rapidjson::Value& resp)
{
	if (!resp.IsObject()) {
		LOG(INFO) << "get offline player property change resp info is not a object";
		return;
	}
	if (resp.HasMember("count") && resp["count"].IsInt()) {
		total_count = resp["count"].GetInt();
	}
	if (resp.HasMember("propertyChanges") && resp["propertyChanges"].IsArray()) {
		for (rapidjson::SizeType i = 0; i < resp["propertyChanges"].Size(); ++i)
		{
			if (resp["propertyChanges"][i].IsObject())
			{
				OfflinePlayerPropertyChange property_change;
				property_change.FromJson(resp["propertyChanges"][i]);
				property_change_list.push_back(property_change);
			}
		}
	}

	if (total_count != (int32_t)property_change_list.size())
	{
		LOG(INFO) << "get offline player property change resp count is no match to propertys size";
	}
}

void ActivityInfo::ToJson(rapidjson::Value& json, rapidjson::Value::AllocatorType& allocator)
{
    for (auto &it : activity_detail){
        rapidjson::Value key(it.first.c_str(), allocator);
        rapidjson::Value value(it.second);
        json.AddMember(key, value, allocator);
    }
}

void ActivityInfo::FromJson(rapidjson::Value& json)
{
    for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it){
        if (!it->name.IsString() || !it->value.IsInt()){
            LOG(ERROR) << "[Activity] ActivityInfo::FromJson json value name or value not string";
            continue;
        }
        auto ret = activity_detail.insert(std::make_pair(it->name.GetString(), it->value.GetInt()));
        if (!ret.second){
            LOG(ERROR) << "[Activity] ActivityInfo::FromJson json value name has multi,insert failed";
        }
    }
}



void AllActivity::ToJson(rapidjson::Value& json, rapidjson::Value::AllocatorType& allocator)
{
    rapidjson::Value all_activity_json_value;
    all_activity_json_value.SetObject();
    for (auto& it : all_activity){
        rapidjson::Value activity_json;
        activity_json.SetObject();
        it.second.ToJson(activity_json, allocator);

        rapidjson::Value key(CbUtility::formatString("%d", it.first).c_str(), allocator);
        all_activity_json_value.AddMember(key, activity_json, allocator);
    }

    json.AddMember("activity", all_activity_json_value, allocator);
}

void AllActivity::FromJson(rapidjson::Value& json)
{
    if (!json.IsObject()){
        LOG(ERROR) << "[Activity] AllActivity::FromJson json not object";
    }
    for (auto it = json.MemberBegin(); it != json.MemberEnd(); ++it){
        if (it->name.IsString() && it->value.IsObject()){
            ActivityInfo activity_info;
            activity_info.FromJson(it->value);
            auto ret = all_activity.insert(std::make_pair(atoi(it->name.GetString()), std::move(activity_info)));
            if (!ret.second){
                LOG(ERROR) << "[Activity] AllActivity::FromJson activity_id has mutli value";
            }
        }
    }
}

TradeRecordInfo::TradeRecordInfo()
:acc_id_from_(0)
, acc_id_to_(0)
, trade_date_(0)
, property_id_(0)
, change_account_(0)
, from_nicheng_("")
, to_nicheng_(""),
trade_type_(0)
{
}

void TradeRecordInfo::ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator)
{
    request.SetObject();
    request.AddMember("acc_id_from", acc_id_from_, allocator);
    request.AddMember("acc_id_to", acc_id_to_, allocator);
    request.AddMember("property_id", property_id_, allocator);
    request.AddMember("change_amount", change_account_, allocator);
    rapidjson::Value from_nick_key("from_nick", allocator);
    rapidjson::Value from_nick_value(from_nicheng_.c_str(), allocator);
    request.AddMember(from_nick_key, from_nick_value, allocator);
    rapidjson::Value to_nick_key("to_nick", allocator);
    rapidjson::Value to_nick_value(to_nicheng_.c_str(), allocator);
    request.AddMember(to_nick_key, to_nick_value, allocator);
    request.AddMember("trade_date", trade_date_, allocator);
    request.AddMember("trade_type", trade_type_, allocator);
}

bool TradeRecordInfo::FromJson(rapidjson::Value& resp)
{
    if (!resp.IsObject()) {
        LOG(INFO) << "TradeRecordInfo is not a object";
        return false;
    }
    if (resp.HasMember("acc_id_from") && resp["acc_id_from"].IsUint()) {
        acc_id_from_ = resp["acc_id_from"].GetUint();
    }
    else{
        LOG(INFO) << "TradeRecordInfo:: FromJson " << "lack [acc_id_from]";
        return false;
    }
    if (resp.HasMember("acc_id_to") && resp["acc_id_to"].IsUint()) {
        acc_id_to_ = resp["acc_id_to"].GetUint();
    }
    else{
        LOG(INFO) << "TradeRecordInfo:: FromJson " << "lack [acc_id_to]";
        return false;
    }
    if (resp.HasMember("property_id") && resp["property_id"].IsUint()) {
        property_id_ = resp["property_id"].GetUint();
    }
    else{
        LOG(INFO) << "TradeRecordInfo:: FromJson " << "lack [property_id]";
        return false;
    }
    if (resp.HasMember("change_amount") && resp["change_amount"].IsUint64()) {
        change_account_ = resp["change_amount"].GetUint64();
    }
    else{
        LOG(INFO) << "TradeRecordInfo:: FromJson " << "lack [change_account_]";
        return false;
    }
    if (resp.HasMember("from_nick") && resp["from_nick"].IsString()) {
        from_nicheng_ = resp["from_nick"].GetString();
    }
    else{
        LOG(INFO) << "TradeRecordInfo:: FromJson " << "lack [from_nick]";
        return false;
    }
    if (resp.HasMember("to_nick") && resp["to_nick"].IsString()) {
        to_nicheng_ = resp["to_nick"].GetString();
    }
    else{
        LOG(INFO) << "TradeRecordInfo:: FromJson " << "lack [to_nick]";
        return false;
    }
    if (resp.HasMember("trade_date") && resp["trade_date"].IsUint()) {
        trade_date_ = resp["trade_date"].GetUint();
    }
    else{
        LOG(INFO) << "TradeRecordInfo:: FromJson " << "lack [trade_date]";
        return false;
    }
    if (resp.HasMember("trade_type") && resp["trade_type"].IsInt())
    {
        trade_type_ = resp["trade_type"].GetInt();
    }
    else
    {
        trade_type_ = eTradeType::trade_property_type;
    }
    return true;
}

void MultiTradeRecordInfo::ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator)
{
    int32_t total_count = tradeRecord_lst_.size();

    request.SetObject();
    request.AddMember("acc_id_request_", acc_id_request_, allocator);
    request.AddMember("count", total_count, allocator);

    rapidjson::Value array(rapidjson::kArrayType);

    for (auto it = tradeRecord_lst_.begin(); it != tradeRecord_lst_.end(); ++it)
    {
        TradeRecordInfo& r_info = *it;
        rapidjson::Value trade_record(rapidjson::kObjectType);
        r_info.ToJson(trade_record, allocator);
        array.PushBack(trade_record, allocator);
    }

    rapidjson::Value  records_value("records", allocator);
    request.AddMember(records_value, array, allocator);
}

void MultiTradeRecordInfo::FromJson(rapidjson::Value& resp)
{
    int32_t total_count = 0;
    if (!resp.IsObject())
    {
        LOG(ERROR) << "get MultiTradeRecordInfo from rabbitmq is not a object";
        return;
    }

    if (resp.HasMember("acc_id_request_") && resp["acc_id_request_"].IsUint())
    {
        acc_id_request_ = resp["acc_id_request_"].GetUint();
    }

    if (resp.HasMember("count") && resp["count"].IsInt())
    {
        total_count = resp["count"].GetInt();
    }

    if (resp.HasMember("records") && resp["records"].IsArray())
    {
        rapidjson::Value &dataArray = resp["records"];
        for (uint32_t i = 0; i < dataArray.Size(); i++)
        {
            rapidjson::Value&  trade_record = dataArray[i];
            TradeRecordInfo tradeRecordInfo;
            tradeRecordInfo.FromJson(trade_record);
            tradeRecord_lst_.push_back(tradeRecordInfo);
        }
    }

    if (total_count != (int32_t)tradeRecord_lst_.size())
    {
        LOG(ERROR) << "get MultiTradeRecordInfo from rabbitmq resp count is no match to trade record size";
    }
}

void MultiTradeRecordInfo::UpdateRecordsList(const TradeRecordInfo& tradeRecord)
{
    uint32_t record_count = tradeRecord_lst_.size();
    if (record_count<g_trade_records_limit) //insert the record at the begin of list
    {
        tradeRecord_lst_.insert(tradeRecord_lst_.begin(), tradeRecord);
    } 
    else//remove last record, and insert the record at the begin of list
    {
        tradeRecord_lst_.pop_back();
        tradeRecord_lst_.insert(tradeRecord_lst_.begin(), tradeRecord);
    }
}
MultiTradeRecordInfo& MultiTradeRecordInfo::operator = (const MultiTradeRecordInfo & multiTradeRecordInfo)
{
    for (auto it = multiTradeRecordInfo.tradeRecord_lst_.begin(); it != multiTradeRecordInfo.tradeRecord_lst_.end(); ++it)
    {
        tradeRecord_lst_.push_back(*it);
    }
    return *this;
}
TradeRecordsCache::TradeRecordsCache(int32_t propertyType) :Is_records_init_(false), query_semaphore_(0), propertyType_(propertyType)
{
}
TradeRecordsCache::~TradeRecordsCache()
{
}
TradeRecordsCacheManager::TradeRecordsCacheManager()
{
    trade_records_cache_lst_.push_back(TradeRecordsCache(PropertyType::PROPERTY_DIAMONDS));
}
MultiTradeRecordInfo* TradeRecordsCacheManager::GetTradeRecordsList(int32_t propertyType)
{
    TradeRecordsCache* tradeRecordsCache=GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        return &(tradeRecordsCache->tradeRecords_);
    } 
    return nullptr;
}
bool TradeRecordsCacheManager::IsCacheRecordsInit(int32_t propertyType)
{
    TradeRecordsCache* tradeRecordsCache = GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        return tradeRecordsCache->Is_records_init_;
    }
    return false;
}
void TradeRecordsCacheManager::SetRecordsInit(int32_t propertyType, bool isInit)
{
    TradeRecordsCache* tradeRecordsCache = GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        tradeRecordsCache->Is_records_init_ = isInit;
    }
}
uint8_t TradeRecordsCacheManager::GetQuerySemaphore(int32_t propertyType)
{
    TradeRecordsCache* tradeRecordsCache = GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        return tradeRecordsCache->query_semaphore_;
    }
    LOG(ERROR) << "can not find TradeRecordsCache contact with PropertyType " << propertyType;
    return 1;
}
void TradeRecordsCacheManager::IncreaseQuerySemaphore(int32_t propertyType)
{
    TradeRecordsCache* tradeRecordsCache = GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        tradeRecordsCache->query_semaphore_ += 1;
    }
}
void TradeRecordsCacheManager::DecreaseQuerySemaphore(int32_t propertyType)
{
    TradeRecordsCache* tradeRecordsCache = GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        tradeRecordsCache->query_semaphore_ -= 1;
    }
}
void TradeRecordsCacheManager::UpdateTradeRecordsList(int32_t propertyType, const TradeRecordInfo& tradeRecord)
{
    TradeRecordsCache* tradeRecordsCache = GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        tradeRecordsCache->tradeRecords_.UpdateRecordsList(tradeRecord);
    }
}
void TradeRecordsCacheManager::InitTradeRecordsList(uint32_t propertyType, const MultiTradeRecordInfo& multiTradeRecords)
{
    TradeRecordsCache* tradeRecordsCache = GetTradeRecordsCache(propertyType);
    if (tradeRecordsCache != nullptr)
    {
        tradeRecordsCache->tradeRecords_ = multiTradeRecords;
    }
}
TradeRecordsCache* TradeRecordsCacheManager::GetTradeRecordsCache(int32_t propertyType)
{
    for (auto it = trade_records_cache_lst_.begin(); it != trade_records_cache_lst_.end(); ++it)
    {
        if (it->propertyType_ == propertyType)
        {
            return &(*it);
        }
    }
    LOG(ERROR) << "TradeRecordsCacheManager::GetTradeRecordsCache get null TradeRecordsCache";
    return nullptr;
}
