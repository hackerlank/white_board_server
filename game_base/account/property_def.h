/*
 * property_def.h
 *
 *  Created on: Jan 11, 2014
 *      Author: vagrant
 */

#ifndef GAME_BASE_ACCOUNT_PROPERTY_DEF_H_
#define GAME_BASE_ACCOUNT_PROPERTY_DEF_H_

#include <set>
#include <map>

#include "transcode_packet.h"


enum PropertyType {
	PROPERTY_DEFAULT = 0,
	PROPERTY_GOLD_COINS = 1,	// 金币
	PROPERTY_GOLD_NOTES,		// 金券
	PROPERTY_DIAMONDS,			// 钻石
	PROPERTY_HORN,				// 小喇叭
	PROPERTY_TOTAL_CHARGE_FEE,	// 充值金额
	PROPERTY_TICKET,			// 抽奖券
    PROPERTY_MESSAGE_CHARGE_FEE,// 短信充值额度
    PROPERTY_VIP=1001,				// vip
    PROPERTY_CAISHEN_PLAYED_TIMES,//玩财神次数
    PROPERTY_WANREN_PLAYED_TIMES,//玩万人场次数
	PROPERTY_ALL = 0xFFFFFFFF,
};

enum PLAYER_MONEY_CHANGE_TYPE
{
  PLAYER_MONEY_CHANGE_NOT_SPECIFIED,				// 未指定和归类的
  PLAYER_MONEY_CHANGE_GAME_WIN,					// 胜
  PLAYER_MONEY_CHANGE_GAME_LOSE,					// 负
  PLAYER_MONEY_CHANGE_RECHARGE,					// 充值 购买
  PLAYER_MONEY_CHANGE_GUARANTEEBASICLIVING,		// 低保
  PLAYER_MONEY_CHANGE_SPREE,						// 定时奖励
  PLAYER_MONEY_CHANGE_ANTE,						// 下注
  PLAYER_MONEY_CHANGE_MATCH_FEE,					// 比赛基本费用
  PLAYER_MONEY_CHANGE_MATCH_REWARD,				// 比赛奖励
  PLAYER_MONEY_CHANGE_TASK_REWARD,				// 任务奖励
  PLAYER_MONEY_CHANGE_INVITE_REWARD,				// 推荐奖励
  PLAYER_MONEY_CHANGE_INVITED_REWARD,				// 被推荐奖励
  PLAYER_MONEY_CHANGE_BUY_LOTTERY,				// 购买彩票
  PLAYER_MONEY_CHANGE_WIN_LOTTERY,				// 彩票中奖
  PLAYER_MONEY_CHANGE_WIN_RAFFLE,					// 抽奖中奖
  PLAYER_MONEY_CHANGE_BUY_ITEM,					// 购买了道具
  PLAYER_MONEY_CHANGE_LOGIN,						// 登陆奖励
  PLAYER_MONEY_CHANGE_CHANGE_CARD,				// 换牌消耗
  PLAYER_MONEY_CHANGE_OP,							// OP消耗
  PLAYER_MONEY_CHANGE_MONEYTREE,					// 摇钱树
  PLAYER_MONEY_CHANGE_TO_EXCHANGE_PROPERTY,		// 购买道具（换购）
  PLAYER_MONEY_CHANGE_RETURN_CHANGE_CARD_COST,	// 奉还换牌消耗
  PLAYER_MONEY_CHANGE_CAISHENTABLE,				// 财神系统
  PLAYER_MONEY_CHANGE_CAISHENTABLE_RETURN,		// 为了和1.2.5版本兼容, 需要额外添加一个类型

  PLAYER_MONEY_CHANGE_WANRENTABLE_WIN,         //wanren
  PLAYER_MONEY_CHANGE_WANRENTABLE_RETURN,
  PLAYER_MONEY_CHANGE_WANRENTABLE_ANTE,
  PLAYER_MONEY_CHANGE_CHAMPION_ANTE,
  PLAYER_MONEY_CHANGE_CHAMPION_WIN,

  PLAYER_MONEY_CHANGE_HORN_BROADCAST = 100,		// 小喇叭广播
  PLAYER_MONEY_CHANGE_RAFFLE_COST,				// 抽奖消耗

  PLAYER_MONEY_CHANGE_MAIL_ATTACHMENT = 307,
  PLAYER_MONEY_CHANGE_NEW_PLAYER,

  PLAYER_MONEY_CHANGE_GM,                       //GM修改玩家钱
  PLAYER_MONEY_CHANGE_GAIN_RED_BOX,             //领取随机红包
  PLAYER_OPEN_LOTTERY_CHANGE,                   //开奖池
  PLAYER_OPEN_BULL_RED_BOX,                     //牛牛宝箱
  PLAYER_MONEY_CHANGE_CAISHENTABLE_ANTE,        //财神下注
  PLAYER_DIAMONDS_GOLDS_CHARGE,                 //钻石兑换
  PLAYER_TRADE_BETWEEN_PLAYER,                  //玩家之间交易

  PLAYER_CHALLENGE_FEE,                         //挑战赛报名费
  PLAYER_LUA_CHANGE, 
};

//yujiding 2014/06/06
enum PlayerDiamondChangeType
{
	kPlayerDiamondChangeVipMaturityTipConsumption = 305,  //vip到期提示消耗钻石
	kPlayerDiamondChangeTipConsumption,             //提示消耗钻石
};

typedef int32_t stPropertyId;

struct stRewardValues {
	uint32_t currentValue;
	uint32_t nextValue;
	uint32_t maxValue;

	stRewardValues() :
			  currentValue(0)
			, nextValue(0)
			, maxValue(0) {}

};

struct stPropertyRewards {
	std::map<stPropertyId, uint32_t>	nCurrentProps; // 本次增加道具数
	std::map<stPropertyId, uint32_t>	nNextProps;    // 下次增加道具数
	std::map<stPropertyId, uint32_t>	nMaxProps;     // 最大增加道具数

	void SetRewards(stPropertyId prop, uint32_t currentRewards, uint32_t nextRewards, uint32_t maxRewards) {
		nCurrentProps[prop] = currentRewards;
		nNextProps[prop] = nextRewards;
		nMaxProps[prop] = maxRewards;
	}

	std::set<stPropertyId> GetPropIds() {
		std::set<stPropertyId> propIds;
		for (std::map<stPropertyId, uint32_t>::const_iterator it = nCurrentProps.begin(); it != nCurrentProps.end(); ++it) {
			propIds.insert(it->first);
		}
		return propIds;
	}

	stRewardValues GetPropValues(stPropertyId propertyId) {
		stRewardValues v;
		v.currentValue = nCurrentProps[propertyId];
		v.nextValue = nNextProps[propertyId];
		v.maxValue = nMaxProps[propertyId];
		return v;
	}

	void ToPacket(PacketTranscode & pack)
	{
		pack <<  static_cast<int32_t>(nCurrentProps.size());

		std::map<stPropertyId, uint32_t> *propsList[3] = {&nCurrentProps, &nNextProps, &nMaxProps};
		for (int i=0; i<3; i++) {
			std::map<stPropertyId, uint32_t> *pPropsItem = propsList[i];
			for (std::map<stPropertyId, uint32_t>::const_iterator it = pPropsItem->begin(); it != pPropsItem->end(); ++it) {
				pack << it->first << it->second;
			}
		}
	}

	void FromPacket(PacketTranscode& pack)
	{
		int32_t propCount = 0;
		pack >> propCount;

		std::map<stPropertyId, uint32_t> *propsList[3] = {&nCurrentProps, &nNextProps, &nMaxProps};
		for (int i=0; i<3; i++) {
			std::map<stPropertyId, uint32_t> *pPropsItem = propsList[i];
			for (int j=0; j<propCount; j++) {
                stPropertyId propertyId = 0;
				uint32_t rewards =0;
				pack >> propertyId >> rewards;
				(*pPropsItem)[propertyId] = rewards;
			}
		}
	}

};


#endif  // GAME_BASE_ACCOUNT_PROPERTY_DEF_H_
