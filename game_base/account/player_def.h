/*
 * player_def.h
 *
 *  Created on: Jan 10, 2014
 *      Author: vagrant
 */

#ifndef GAME_BASE_ACCOUNT_PLAYER_DEF_H_
#define GAME_BASE_ACCOUNT_PLAYER_DEF_H_

#include <rapidjson/document.h>
#include <map>

#include "property_def.h"

class Player;

#define GAME_GUARANTEEBASICLIVING_MAX_TIMES_PER_DAY 2

#define SVR_32_VERSION "2.7.0"
#define SVR_2_8_0_VERSION "2.8.0"
#define SVR_2_9_0_VERSION "2.9.0"

const uint8_t g_trade_records_limit = 50;

enum eTradeType
{
    trade_property_type = 1,  //金钱交易
    trade_item_type = 2,      //物品交易
};

class PacketTranscode;

typedef std::map<std::string, std::string>	GameOptions;

struct CG_Options {
public:
	static const std::string kUDID;
	static const std::string kDeviceId;
	static const std::string kDeviceModel;
	static const std::string kAppName;
	static const std::string kAppVersion;
	static const std::string kAppVersionCode;
	static const std::string kChannelId;
	static const std::string kCountry;
	static const std::string kOS;
	static const std::string kOsVersion;
	static const std::string kLanguage;
	static const std::string kCpu;
	static const std::string kResolution;
	static const std::string kAccess;
	static const std::string kCarrier;
	static const std::string kLongitude;
	static const std::string kLatitude;
	static const std::string kCategory;
	static const std::string kAction;
	static const std::string kLabel;
	static const std::string kValue;
	static const std::string kPushDeviceToken;
	static const std::string kAccId;
	static const std::string kAccType;
	static const std::string kPhoneNumber;
    static const std::string kDeviceType;
    static const std::string kCheckSum;

	static const std::string OS_WINDOWS;
	static const std::string OS_IOS;
	static const std::string OS_ANDROID_SYS;
	static const std::string OS_WINDOWS_PHONE;
	static const std::string OS_ANDROID_BAIDU;

public:
	CG_Options();
	explicit CG_Options(const GameOptions& options);

	void ToPacket(PacketTranscode& packet);
	void FromPacket(PacketTranscode& packet);
	std::string GetValue(const std::string &key);
private:
	GameOptions m_Options_;

private:
	friend class PlayerSession;
	friend class Player;
};

//TODO(jszhang): add a function to deal with this enum
enum PlayerLeaveRoomReason {
	PLAYER_LEAVE_ROOM_REASON_BEGIN = -1,
	PLAYER_LEAVE_ROOM_REASON_INITIATIVE,		 			// 主动离开
	PLYAER_LEAVE_ROOM_REASON_RELOGIN_KICK,			 		// 相同玩家登录
	PLAYER_LEAVE_ROOM_REASON_NETWORK_KICK,			 		// 网络超时
	PLAYER_LEAVE_ROOM_REASON_NETWORK_SHUTDOWN,		 		// 网络断开
	PLAYER_LEAVE_ROOM_REASON_MONEY_NOT_ENOUGH, 	 	 		// 金币不够
	PLAYER_LEAVE_ROOM_REASON_NO_READY_TOO_MANY_TIMES, 		// 长时间不准备
	PLAYER_LEAVE_ROOM_REASON_NO_ACTION_TOO_MANY_TIMES, 		// 长时间没有活动。
	PLAYER_LEAVE_ROOM_REASON_KICK_ROBOT, 					// 机器人
	PLAYER_LEAVE_ROOM_REASON_KICK,							// 被踢
	PLAYER_LEAVE_ROOM_REASON_CHALLENGE_OVER_KICK, 			// 挑战赛结束，玩家出局。
	PLAYER_LEAVE_ROOM_REASON_CHALLENGE_LOSE_KICK, 			// 挑战赛输家出局。
	PLAYER_LEAVE_ROOM_REASON_TABLE_INFO_NOT_THE_SAME_KICK, 	// 恢复时Table信息不对，被踢。
	PLAYER_LEAVE_ROOM_REASON_CHANGE_TABLE, 					// 换桌。
	PLAYER_LEAVE_ROOM_REASON_SERVER_READY_MAINTENANCE, 		// 要停服，都滚蛋
	PLAYER_LEAVE_ROOM_REASON_HOST_LEAVE, 					// 私人场的庄家离开了
	PLAYER_LEAVE_ROOM_REASON_END = 1000,
};


enum TRIGGER_TYPE {
	TRIGGER_TYPE_BEGIN = 0,
	TRIGGER_TYPE_BEFORE,
	TRIGGER_TYPE_AFTER,
	TRIGGER_TYPE_END,
};

enum PLAYER_STATE {
	NORMAL = 0,
	TRUSTEESHIP,
};

enum ePLAYER_SEX
{
	PLAYER_SEX_MALE = 0,
	PLAYER_SEX_FEMALE,
	PLAYER_SEX_UNSPECIFIED
};

enum ePLAYER_TYPE_DEF
{
	PLAYER_TYPE_PLAYER = 0,	//玩家
	PLAYER_TYPE_ROBOT,		//机器人
	PLAYER_TYPE_GM			//GM
};


struct AccountInfo {
	uint32_t accountID;		//账号id
	std::string account;	//用户绑定账号
	std::string password;	//用户绑定账号密码
	std::string nick;		//昵称
	uint32_t icon;			//(暂时)玩家头像编号
	uint32_t sex;			//性别	//ePLAYER_SEX
	uint32_t birthday;		//生日，时间戳
	std::string thirdlogin;	//第三方平台绑定状态
	std::string avatar;		// 头像

	AccountInfo();

    void ToPacket(PacketTranscode &resp);
    void FromPacket(PacketTranscode &request);

    void ToSimplyPacket(PacketTranscode &resp);
    void FromSimplyPacket(PacketTranscode &request);

    void ToJson(rapidjson::Value& json, rapidjson::Value::AllocatorType& allocator);
    void FromJson(rapidjson::Value& json);

    void SetThirdLogin(const std::string& third);
};

//TODO(hdchen),暂时和以前的老版本兼容
typedef std::map<int32_t, int64_t> PlayerPropMap;    // propId ---- num

struct PlayerPropTable //玩家道具列表
{
	uint32_t accountId;	//账号id
	PlayerPropMap propMap_;

	PlayerPropTable ();

	uint32_t GetHorns() const;

	void ChangeHorns(uint32_t change);

	void ToPacketEx(PacketTranscode& packet);
	void FromPacketEx(PacketTranscode& packet);

	void ToPacket( PacketTranscode& packet );
	void FromPacket( PacketTranscode& packet );


	void ToJson(std::string& req);

	void FromJson(rapidjson::Value& resp);
};

struct stPlayerInvite	//邀请码相关结构
{
	enum InviteType
	{
		Invite_Type_Notes = 0,
		Invite_Type_Coins = 1
	};

	std::string MyInviteCount;     // 邀请码，使用次数200上限
	std::string Invited;		// 邀请人的邀请码

	uint32_t TypeInvite;			//奖励类型 邀请者
	uint32_t TypeInvited;			//奖励类型 被邀请者

	uint32_t RewardInvite;		//奖励值 邀请者
	uint32_t RewardInvited;		//奖励填写邀请码的 被邀请者

	stPlayerInvite();

	void ToPacket(PacketTranscode& packet);

	void FromPacket(PacketTranscode& packet);

};

struct ActivityInfo
{
    std::map<std::string, int32_t> activity_detail;

    void ToJson(rapidjson::Value& json, rapidjson::Value::AllocatorType& allocator);

    void FromJson(rapidjson::Value& json);
};

struct AllActivity
{
    std::map<int32_t, ActivityInfo> all_activity;

    void ToJson(rapidjson::Value& json, rapidjson::Value::AllocatorType& allocator);
    void FromJson(rapidjson::Value& json);
};

class PlayerCharacterInfo		//Game角色信息
{

public:
	uint64_t GetDiamonds()
	{
		return diamonds;
	}

	void SetDiamonds(uint64_t num)
	{
		diamonds = num;
	}

	uint64_t GetGoldCoins()
	{
		return GoldCoins;
	}

	void SetGoldCoins(uint64_t coins)
	{
		GoldCoins = coins;
	}
private:
    Player* owner_;
protected:
    uint64_t GoldCoins; //金币
    uint64_t diamonds;
public:
	uint32_t AccountID;	//账号id
	uint32_t GoldNote;	//金券
	uint32_t Played;	//比赛场次
	uint32_t Wined;	//胜利场次
	int64_t MaxWin;	//单次赢最多
	int64_t MaxLose;	//单次赢最多

	uint32_t onlineSpree;				//领取在线礼包次数
	uint32_t onlineSpreeTime;			//上次领取在线礼包时间戳
	uint32_t nextSpreeRemaningTime;	//下次领取在线礼包的剩余时间
    uint32_t onlineSpreeSum;        //当天领取的在线礼包value之和

	uint32_t rankTime;		//排行数据时间戳
	int64_t dayMaxwin;		//每日赢取金币(历史最高)

	//yujiding
	int64_t thisDayMaxWin; //每日赢取金币（当日）
    int64_t thisDayCaishen;//财神赢取数量（当日）
    int64_t thisDayWanRen;
    int64_t thisDayPayment;//当日充值总额
    int64_t yesterdayPayment;//昨日充值
	time_t lastTimeOfDayMaxWin; //上次计算每日最大值时的日期
    time_t lastTimeOfDayCaishen;
    time_t lastTimeOfDayWanRen;
    time_t lastTimeOfDayPayment;

    

	uint32_t rank;			//胜负排名，0为无排名

	uint8_t currentGuaranteeTimes;                    //当天已经低保赠送的次数
	uint8_t maxGuaranteeBasicLivingTimes;
	time_t lastTimeOfResetGuaranteeBasicLivingTimes;  //上一次玩家登出时间（赞作为低保发放标识）

	uint32_t  raffle;		//抽奖券
	uint32_t raffleTime;	//抽奖券上次赠送时间戳

	uint32_t maxFillCoins;	//累计最大充值

	uint32_t nextSpreeValue;			//下次领取在线礼包获得的金币数量
	uint32_t lastTimeOfMoneyTreeRock;	//摇钱树，最后一次领取时间
	uint32_t settings;				//玩家设置
	uint32_t LastMatchType;   //最后一次比赛的场次类型
	//uint32_t diamonds;			// 钻石数量，用于换牌

	uint32_t continuousLogin; //连续登陆次数

	stPlayerInvite Invite;		// 邀请码

	PlayerPropTable*	playerPropTable;     //玩家的道具

	std::string*  		playerTaskInfoMap;  //玩家的任务 json string

	std::string Title;		// 称号

    AllActivity* all_activity_info;

public:
    PlayerCharacterInfo(Player* player);
    virtual ~PlayerCharacterInfo();
    Player* GetOwner();

public:

	virtual uint32_t GetLastLoginTm(){return 0;}

	virtual void ToPacket(PacketTranscode& packet);
    virtual void ToPacketEx(PacketTranscode& packet);
	virtual void ToPacketEx64( PacketTranscode& packet );

	virtual void FromPacket(PacketTranscode& packet){}
    virtual void FromPacketEx(PacketTranscode& packet){}
	virtual void FromPacketEx64(PacketTranscode& packet){}

    virtual void ToSimplePacket(PacketTranscode &resp);
	virtual void ToSimplePacketEx(PacketTranscode &resp);
	virtual void ToSimplePacketEx64(PacketTranscode &resp);

	virtual void FromSimplePacket(PacketTranscode &request){}
	virtual void FromSimplePacketEx(PacketTranscode &request){}
	virtual void FromSimplePacketEx64(PacketTranscode &request){}


	virtual void ToJson(rapidjson::Value& request, std::string& propJson, rapidjson::Value::AllocatorType& allocator);
	virtual void ToJson(Player* player, 
					    rapidjson::Value& request, 
						std::string& propJson, 
						rapidjson::Value::AllocatorType& allocator);

	virtual void FromJson(rapidjson::Value& resp);
	virtual void FromJson(Player* player, rapidjson::Value& resp);

	void SyncProPertyToCharacterInfo();
};

//yujiding 2014/06/03 称号信息
struct TitleInfo
{
	std::string title_name;
	uint64_t min_score;
    uint64_t max_score;

	bool operator<(const TitleInfo& right_value) const{
		return min_score < right_value.min_score;
	}
};

//globalserver->gameserver的登录后返回玩家的账号和角色信息
struct AccountCharacterInfoResp
{
	AccountInfo *account;
	PlayerCharacterInfo *character;
	std::string clientId;
	CG_Options options;
	uint32_t stay_hall;
	std::string rc6_key;
	std::string item_xml_version;
	uint32_t first_login_flag;
	uint32_t cur_tm;
	uint32_t session_id;
	uint32_t recharge_rmb;
	uint32_t month_recharge_rmb;
	uint32_t recharge_sms_rmb;
    uint32_t is_gold_trader_;
    uint32_t max_trade_count;
    std::string download_url;

	void ToPacket(PacketTranscode& packet);
    void ToPacketEx(PacketTranscode& packet);
	void ToPacketEx64(PacketTranscode& packet);

	void FromPacket(PacketTranscode& packet);
	void FromPacketEx(PacketTranscode& packet);
	void FromPacketEx64(PacketTranscode& packet);

	void ToPacketWithVersion( PacketTranscode& packet,  const std::string& version);
};



//属性改变时的标志



//一种属性的改变
struct stOnePropertyChange
{
	stPropertyId			propertyId;
	uint64_t				currentValue;		//TODO(hdchen),需要改成64位
	int64_t					changedValue;
	std::vector<int64_t>	otherValues;		//TODO(hdchen),需要改成64位

	void ToPacket(PacketTranscode& packet); 
	void FromPacket(PacketTranscode& packet); 
	
	void ToPacketEx(PacketTranscode& packet);
	void FromPacketEx(PacketTranscode& packet);
};


//角色的属性改变
struct PlayerPropertyChange
{
	uint32_t		uPlayerID;
	std::string		szTitle;
	uint32_t		uChangeReason;
	std::map<stPropertyId, stOnePropertyChange> changedProps;

	std::vector<stPropertyId> GetPropertyIds() 
	{
		std::vector<stPropertyId> propIds;
		for (std::map<stPropertyId, stOnePropertyChange>::iterator it = changedProps.begin(); it != changedProps.end(); ++it)
		{
			propIds.push_back(it->first);
		}
		return propIds;
	}

	void SetPropertyChanges(std::vector<stOnePropertyChange> &changes)
	{
		for (std::vector<stOnePropertyChange>::iterator it = changes.begin(); it != changes.end(); ++it)
		{
			changedProps[it->propertyId] = *it;
		}
	}

	bool GetPropertyChange(stPropertyId propertyId, stOnePropertyChange &change)
	{
		std::map<stPropertyId, stOnePropertyChange>::iterator it = changedProps.find(propertyId);
		if (it == changedProps.end()) 
		{
			return false;
		}
		change = it->second;
		return true;
	}

	void ToPacket(PacketTranscode& packet); 
	void FromPacket(PacketTranscode& packet);

	void ToPacketEx(PacketTranscode& packet);
	void FromPacketEx(PacketTranscode& packet);
};

struct PlayerGetSpree
{
    uint32_t error_code;    
    uint32_t next_spree_time;
    uint32_t next_spree_value;

    void ToPacket(PacketTranscode& packet);
    void FromPacket(PacketTranscode& packet);
};

struct PlayerGetSpreeLimited
{
    uint32_t error_code;
    uint32_t next_spree_time;
    uint32_t next_spree_value;
    uint32_t spree_sum_now;
    uint32_t spree_sum_limit;

    void ToPacket(PacketTranscode& packet);
    void FromPacket(PacketTranscode& packet);
};

//yujiding 2014/07/24
struct OfflinePlayerPropertyChange
{
	enum{
		kStart = 256,
		KInvitedBones,
		kTypeEnd,
	};
	int32_t id;
	uint32_t player_id;
	stPropertyId property_id;
	int32_t change;
	int32_t change_type;
	int32_t flags;
	float pay_money;

	OfflinePlayerPropertyChange() :
		id(0)
		, player_id(0)
		, property_id(0)
		, change(0)
		, change_type(PLAYER_MONEY_CHANGE_NOT_SPECIFIED)
		, flags(0)
		, pay_money(0.0)
	{}

	void ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator);
	void FromJson(rapidjson::Value& resp);
};

struct GetOfflineChangePropertyResp
{
	int32_t total_count;
	std::vector<OfflinePlayerPropertyChange> property_change_list;

	GetOfflineChangePropertyResp() :
		total_count(0)
	{}

	void ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator);
	void FromJson(rapidjson::Value& resp);
};

struct TradeRecordInfo
{
    uint32_t    acc_id_from_;
    uint32_t    acc_id_to_;
    uint32_t    trade_date_;
    int32_t     property_id_;
    uint64_t    change_account_;
    std::string from_nicheng_;
    std::string to_nicheng_;
    int32_t     trade_type_;

    TradeRecordInfo();
    void ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator);
    bool FromJson(rapidjson::Value& resp);
};

struct MultiTradeRecordInfo{
    std::vector<TradeRecordInfo> tradeRecord_lst_;
    uint32_t acc_id_request_;
    MultiTradeRecordInfo& operator=(const MultiTradeRecordInfo & multiTradeRecordInfo);
    void UpdateRecordsList(const TradeRecordInfo& tradeRecord);
    void ToJson(rapidjson::Value& request, rapidjson::Value::AllocatorType& allocator);
    void FromJson(rapidjson::Value& resp);
};
struct TradeRecordsCache
{
    TradeRecordsCache(int32_t propertyType);
    ~TradeRecordsCache();
    bool                Is_records_init_;
    uint8_t             query_semaphore_;//prevent from trade record query again before former request  is response
    MultiTradeRecordInfo  tradeRecords_;
    int32_t             propertyType_;
};
struct TradeRecordsCacheManager
{
    TradeRecordsCacheManager();
    MultiTradeRecordInfo* GetTradeRecordsList(int32_t propertyType); //get player trade records cache with limit count
    bool IsCacheRecordsInit(int32_t propertyType);
    uint8_t GetQuerySemaphore(int32_t propertyType);
    void SetRecordsInit(int32_t propertyType,bool isInit);
    void IncreaseQuerySemaphore(int32_t propertyType);
    void DecreaseQuerySemaphore(int32_t propertyType);
    void InitTradeRecordsList(uint32_t propertyType, const MultiTradeRecordInfo& multiTradeRecords);
    void UpdateTradeRecordsList(int32_t propertyType, const TradeRecordInfo& tradeRecord);//update trade records cache

    std::vector<TradeRecordsCache> trade_records_cache_lst_;
private:
    TradeRecordsCache* GetTradeRecordsCache(int32_t propertyType);
};
#endif  // GAME_BASE_ACCOUNT_PLAYER_DEF_H_
