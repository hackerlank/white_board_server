/*
 * dn_poker.h
 *
 *  Created on: Apr 14, 2014
 *      Author: vagrant
 */

#ifndef DN_APPLICATION_DN_ACCOUNT_DN_POKER_H_
#define DN_APPLICATION_DN_ACCOUNT_DN_POKER_H_

#include <stdint.h>
#include <stdlib.h>
#include <cinttypes>
#include <cstring>
#include <vector>
#include <algorithm>
#include <functional>
#include <map>

#include "transcode_packet.h"

#define MAX_DNPOKER_COUNT	52			// 去掉大小王 Joker
#define MIN_DNPOKER_VALUE	1			//	A
#define MAX_DNPOKER_VALUE	13			//	K
#define DNPOKER_COLOR_COUNT 4

const int32_t kPokerNumOfGroup = 5;

//扑克牌的四种花色
struct  DNPokerColorType
{
	enum
	{
		DN_POKER_COLOR_DIAMONDS		 = 1,		//方块
		DN_POKER_COLOR_CLUBS		 = 2,		//樱花
		DN_POKER_COLOR_HEARTS		 = 3,		//红桃
		DN_POKER_COLOR_SPADES		 = 4,		//黑桃
	};
};

//一张扑克牌的属性
struct DNOnePoker
{
	int8_t	poker_value;
	int8_t	poker_color;

	DNOnePoker()
	{
		poker_color = 0;
		poker_value = 0;
	}

	DNOnePoker& operator=(const DNOnePoker& poker);
	bool operator==(const DNOnePoker& otherPoker) const;
	bool operator!=(const DNOnePoker& poker) const;

	void reset();
	bool isNullPoker() const;
	std::string friendlyName() const;
};

//按照扑克的值进行比较
bool DNPokerValueFirstCmp(const DNOnePoker& one, const DNOnePoker& other);
//按照花色进行比较
bool DNPokerColorFirstCmp(const DNOnePoker& one, const DNOnePoker& other);


//斗牛扑克牌牌型
struct DNCardType
{
	enum DN_TYPE
	{
		DN_TYPE_START = 0,
		MEI_NIU		  = 0,	//没牛
		NIU_DING	  = 1,	//牛丁
		NIU_ER		  = 2,	//牛二
		NIU_SAN		  = 3,	//牛三
		NIU_SI		  = 4,	//牛四
		NIU_WU		  = 5,	//牛五
		NIU_LIU		  = 6,	//牛六
		NIU_QI		  = 7,	//牛七
		NIU_BA		  = 8,	//牛八
		NIU_JIU		  = 9,	//牛九
		NIU_NIU		  = 10,	//牛牛

		SI_ZHA		  = 11,	//四炸
		WU_HUA_NIU	  = 12,	//五花牛
		WU_XIAO_NIU	  = 13,	//五小牛

		DN_TYPE_COUNT = 14,	// place holder.
	};
};


//一组斗牛牌(5张牌组成)
struct DNGroupPoker
{
	DNOnePoker		poker[5];				//5张牌
	int32_t			dn_type;				//5张牌的牌型
	DNOnePoker		*max_poker;				//5张牌的最大单牌

	bool			bCalc;					//牌型是否计算过

	int32_t			niu_poker_index[5];		//是组成牌型的索引/或者其他无牛/四炸/五花牛/小花牛
	int32_t			other_poker_index[2];	//剩余两张牌的索引


	void Confuse();

	bool has_deal()
	{
		for( int i = 0; i< 5; i++)
		{
			if( poker[i].poker_value > 0 && poker[i].poker_color > 0)
			{
				return true;
			}
		}
		return false;
	}

	bool HasSamePoker()
	{
		for (uint32_t i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
		{
			for( uint32_t j = i+1; j < sizeof(poker) / sizeof(poker[0]); j++)
			{
				if( poker[i] == poker[j] )
				{
					return true;
				}
			}
		}
		return false;
	}


	DNGroupPoker()
	{
		reset();
	}

	void reset()
	{
		for (uint32_t i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
		{
			poker[i].reset();
			niu_poker_index[i] = -1;
		}

		for (uint32_t i = 0; i < sizeof(other_poker_index) / sizeof(other_poker_index[0]); i++)
		{
			other_poker_index[i] = -1;
		}	
		bCalc						= false;
		max_poker					= nullptr;
		dn_type						= 0;
	}

	void calcByNoNiu()
	{
		for (uint32_t i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
		{
			niu_poker_index[i] = i;
		}
		for (uint32_t i = 0; i < sizeof(other_poker_index) / sizeof(other_poker_index[0]); i++)
		{
			other_poker_index[i] = -1;
		}
		dn_type = DNCardType::MEI_NIU;
	}

	bool hasPoker()
	{
		return !(poker[0].isNullPoker() || poker[1].isNullPoker() || poker[2].isNullPoker() || poker[3].isNullPoker() || poker[4].isNullPoker());
	}


	void setPoker(DNOnePoker newPokers[5])
	{
		memcpy(poker, newPokers, sizeof(DNOnePoker)* 5);
	}

	void setNiuPokerIndex(int32_t niuPokerIndex[5], int32_t otherPokerIndex[2])
	{
		memcpy(niu_poker_index, niuPokerIndex, sizeof(int32_t)* 5);
		memcpy(other_poker_index, otherPokerIndex, sizeof(int32_t)* 2);
	}

	void setMaxPoker(DNOnePoker * p)
	{
		max_poker = p;
	}

	bool isTypeIdentified()
	{
		return max_poker != NULL;
	}

	void ToPacket(PacketTranscode & p)
	{
		PacketTranscode packet;
		packet <<	dn_type;
		if (dn_type == DNCardType::MEI_NIU || dn_type > DNCardType::NIU_NIU)
		{
			//没有牛或者四炸/五花牛/小花牛
			for (uint32_t  i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
			{
				packet << poker[i].poker_color << poker[i].poker_value;
				//LOG(INFO) << "[DNGroupPoker::ToPacket] color:" << poker[i].poker_color << " value:" << poker[i].poker_value;
			}
		}
		else
		{
			//先显示3张牛牌型，再显示另外2张牌
			for (uint32_t i = 0; i < sizeof(niu_poker_index) / sizeof(niu_poker_index[0]); i++)
			{
				if (niu_poker_index[i] >= 0)
				{
					packet << poker[niu_poker_index[i]].poker_color << poker[niu_poker_index[i]].poker_value;
				}
			}
			for (uint32_t i = 0; i < sizeof(other_poker_index) / sizeof(other_poker_index[0]); i++)
			{
				if (other_poker_index[i] >= 0)
				{
					packet << poker[other_poker_index[i]].poker_color << poker[other_poker_index[i]].poker_value;
				}
			}
		}
		p << packet;
	}

	void ToPacket2(PacketTranscode & packet)
	{
		packet << dn_type;
		if (dn_type == DNCardType::MEI_NIU || dn_type > DNCardType::NIU_NIU)
		{
			//没有牛或者四炸/五花牛/小花牛
			for (uint32_t  i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
			{
				packet << poker[i].poker_color << poker[i].poker_value;
				//LOG(INFO) << "[DNGroupPoker::ToPacket2] color:" << poker[i].poker_color << " value:" << poker[i].poker_value;
			}
		}
		else
		{
			//先显示3张牛牌型，再显示另外2张牌
			for (uint32_t  i = 0; i < sizeof(niu_poker_index) / sizeof(niu_poker_index[0]); i++)
			{
				if (niu_poker_index[i] >= 0)
				{
					packet << poker[niu_poker_index[i]].poker_color << poker[niu_poker_index[i]].poker_value;
					//LOG(INFO) << "[DNGroupPoker::ToPacket2-1] color:" << poker[niu_poker_index[i]].poker_color << " value:" << poker[niu_poker_index[i]].poker_value;
				}
			}
			for (uint32_t i = 0; i < sizeof(other_poker_index) / sizeof(other_poker_index[0]); i++)
			{
				if (other_poker_index[i] >= 0)
				{
					packet << poker[other_poker_index[i]].poker_color << poker[other_poker_index[i]].poker_value;
				}
			}
		}
	}



	void FromPacket(PacketTranscode & p)
	{
		p >> dn_type;
		for (uint32_t i = 0; i < sizeof(poker) / sizeof(poker[0]); i++)
		{
			p >> poker[i].poker_color >> poker[i].poker_value;
		}
	}
};



//一副扑克牌
class DNOneDeckPoker
{

public:
	DNOneDeckPoker();
	~DNOneDeckPoker();

	// 空手牌
	static const int  DN_NOBRAND =  - 1;

	//一副扑克牌的数量
	inline int32_t getPokerNum()
	{
		return poker_num_;
	}

	//一副扑克牌的最小值
	inline int8_t getMinValue()
	{
		return poker_min_value_;
	}

	//一副扑克牌的最大值
	inline int8_t getMaxValue()
	{
		return poker_max_value_;
	}

	//初始化一副扑克:按照一定的顺序
	void initPoker(int8_t minPoker, int8_t maxPoker);
	void initPokerExceptGroup(int8_t minPoker, int8_t maxPoker, const DNGroupPoker& group_poker);
	//洗牌
	void shuffle(int16_t iTimes	=	1);

	//发牌
	bool dealPoker(DNOnePoker* poker);

	//从指定的牌中找到最大的牌
	static DNOnePoker *findMaxPoker(DNOnePoker poker[5]);
	//比较两张单牌:a > b : 1; a  == b :0 ;  a < b  : -1
	static bool			compareSinglePoker(DNOnePoker* poker_a, DNOnePoker* poker_b);
	//比较两组斗牛牌a > b : 1; a  == b :0 ;  a < b  : -1
	static bool			compareGroupPoker(DNGroupPoker & group_poker_a, bool byNiuType_a,  DNGroupPoker & group_poker_b, bool byNiuType_b);
	//比较两个牛型
	static int32_t		compareNiutype(int32_t a_niu_type, int32_t b_niu_type);

	//根据牛型得到字符串
	//static std::string	getDNTypeStringByType(const int32_t& tType);

	//返回牛型, int32_t pokerIndex[5],牛型对应的牌
	static int32_t		hasCow(DNOnePoker poker[5],	int32_t pokerIndex[5]);
	static int32_t		findCow(DNOnePoker poker[5], int32_t niuPokerIndex[5], int32_t otherPokerIndex[2]);
	static int32_t		changeDNPokerJQK(const int8_t& pokerValue);
	static void			hasCow(DNGroupPoker &groupPoker,  int32_t pokerIndex[5],	 int32_t otherPokerIndex[2]);
	static void			findCow(DNGroupPoker &groupPoker, int32_t niuPokerIndex[5], int32_t otherPokerIndex[2]);

	//给定的牌的信息，验证牛型是否是正确的
	static bool         CheckCowWithPokerInfo(int32_t niu_type, int32_t poker_index[kPokerNumOfGroup], DNGroupPoker& group_poker);

	//计算牛型 & 牛型牌对应的索引 & 其他牌的索引  & 最大牌
	static void			calcCowData(DNGroupPoker &groupPoker,bool bByPokerType = true);

	bool				dealPokers(DNOnePoker * poker[], int32_t playerNumber);

private:
	int32_t			    DNPokerCompare(DNOnePoker& pokerA, DNOnePoker& pokerB);

public:
	int32_t			poker_num_;
	int32_t			deal_position_;

	DNOnePoker		dn_Poker_[MAX_DNPOKER_COUNT];

	int8_t			poker_min_value_;
	int8_t			poker_max_value_;
};


class DNPokerSet
{
public:
	DNPokerSet(uint32_t card_type);
	DNPokerSet();
	~DNPokerSet();
public:
	void AddGroupPoker( DNGroupPoker& Poker);
	uint32_t  GetCardType();
	uint32_t  GetPokerGroupCount();
public:
	const DNGroupPoker* GetRandomGroupPoker();
private:
	uint32_t card_type_;
	std::vector<DNGroupPoker> GroupPokers_;
};



class PokerTypeRateConfig
{
public:
	PokerTypeRateConfig(uint32_t dn_poker_type,  uint32_t begin, uint32_t end);
	PokerTypeRateConfig();
public:
	bool IsInRate(uint32_t rand_num) const;
	uint32_t GetDNPokerType() const;
	uint32_t GetBeginRate() const;
	uint32_t GetEndRate() const;
private:
	uint32_t begin_rate_;
	uint32_t end_rate_;
	uint32_t dn_poker_type_;
};

class DNPokerGen
{
private:
	DNPokerGen();
public:	
	static DNPokerGen* GetInstance();
public:
	bool Init();
	bool Final();
public:
	void AddRateConfig(uint32_t dn_poker_type, uint32_t begin, uint32_t end);
	void ClearConfig();
	void UseRandPoker(uint32_t use);
	const DNGroupPoker* GetRandomGroupPoker();
    const DNGroupPoker* GetAssignGroupPoker(uint32_t poker_type);
	const DNGroupPoker* GetRandomGroupPoker(uint32_t poker_type_begin, uint32_t poker_type_end);
	const DNGroupPoker* GetRandomGroupPokerByRate(uint32_t rand_num);

	uint32_t GetUseRandFlag();
	const PokerTypeRateConfig* GetConfigByPokerType(uint32_t poker_type);

public:
	void AddGroupPoker(DNGroupPoker& group_poker);
private:
	std::map<uint32_t, DNPokerSet>	poker_maps_;

	std::vector<PokerTypeRateConfig> configs_;

	uint32_t use_rand_poker_;
};













#endif  // DN_APPLICATION_DN_ACCOUNT_DN_POKER_H_










