/*
 * dn_poker.cpp
 *
 *  Created on: Apr 14, 2014
 *      Author: vagrant
 */

#include "dn_poker.h"
#include <assert.h>


void DNGroupPoker::Confuse()
{

	for( int i = 0; i< 5; )
	{
		poker[i].poker_color = rand() % 4 + 1;
        bool flg_if_same = false;
        for(int j =0 ;j < i ;j++)
        {
            if (poker[i].poker_color == poker[j].poker_color && poker[i].poker_value == poker[j].poker_value)
            {
                flg_if_same= true;
                break;
            }
        }
        if (!flg_if_same)
        {
            i++;
        }

	}
	setMaxPoker( DNOneDeckPoker::findMaxPoker(poker) );
}



static bool pokerValueIndexPairComparator(const std::pair<int32_t, int32_t> &a, const std::pair<int32_t, int32_t> &b)
{
    return a.first < b.first;
}

/*delete by malong in 2015-3-26 define but not used
static int pokerIndexComparator(const void *a, const void *b)
{
    const int32_t *ia = static_cast<const int*>(a);
    const int32_t *ib = static_cast<const int*>(b);
    return *ia - *ib;
}
*/
//牌数相同比较花色
bool DNPokerValueFirstCmp(const DNOnePoker& one, const DNOnePoker& other)
{
    int16_t diff = one.poker_value - other.poker_value;
    if (0 == diff)
    {
        diff = one.poker_color - other.poker_color;
    }
    return diff < 0;
}

//花色相同比较牌数
bool DNPokerColorFirstCmp(const DNOnePoker& one, const DNOnePoker& other)
{
    int16_t diff = one.poker_color - other.poker_color;
    if (0 == diff)
    {
        diff = one.poker_value - other.poker_value;
    }
    return diff < 0;
}

DNOnePoker & DNOnePoker::operator=(const DNOnePoker &d1)
{
    if (d1.poker_color == this->poker_color && d1.poker_value == this->poker_value)
    {
        return *this;
    }
    this->poker_color = d1.poker_color;
    this->poker_value = d1.poker_value;
    return *this;
}

bool DNOnePoker::operator==(const DNOnePoker & other) const
{
    return (isNullPoker() && other.isNullPoker()) || (poker_color == other.poker_color && poker_value == other.poker_value);
}


bool DNOnePoker::operator!=(const DNOnePoker & other) const
{
    return !(*this == other);
}

bool DNOnePoker::isNullPoker() const
{
    return (poker_value==0||poker_color==0);
}

std::string DNOnePoker::friendlyName() const
{
    char name[16] = { 0 };
    if ((poker_color ==0)||(poker_color > 4)||(poker_value == 0)||(poker_value > 13)){
        snprintf(name, sizeof(name), "%s", "invalid_poker");
    }else{
        static const char * color_names[] = { NULL, "方块", "樱花", "红桃", "黑桃" };
        static const char * value_names[] = { NULL, "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
        snprintf(name, sizeof(name), "%s%s", color_names[poker_color], value_names[poker_value]);
    }
    return std::string(name);
}

void DNOnePoker::reset()
{
    poker_value = 0;
    poker_color = 0;
}

DNOneDeckPoker::DNOneDeckPoker() : poker_num_(0), deal_position_(0),poker_min_value_(0), poker_max_value_(0)
{
    initPoker(MIN_DNPOKER_VALUE, MAX_DNPOKER_VALUE);
}


DNOneDeckPoker::~DNOneDeckPoker()
{

}


void DNOneDeckPoker::initPokerExceptGroup(int8_t minPoker, int8_t maxPoker, const DNGroupPoker& group_poker)
{
		poker_num_         = 0;
		deal_position_   = 0;
		poker_min_value_ = 0;
		poker_max_value_ = 0;

		assert(minPoker <= maxPoker);

		int poker_count_color = maxPoker - minPoker + 1;
		int total_poker_count = DNPOKER_COLOR_COUNT * poker_count_color;

		for (int i=0; i<total_poker_count; ++i)
		{   
			int8_t byValue = (i % poker_count_color) + minPoker;
			int8_t byColor = (i / poker_count_color) + 1;

			dn_Poker_[poker_num_].poker_value = byValue;
			dn_Poker_[poker_num_].poker_color = byColor;
			if( dn_Poker_[poker_num_] == group_poker.poker[0] ||
				dn_Poker_[poker_num_] == group_poker.poker[1] ||
				dn_Poker_[poker_num_] == group_poker.poker[2] ||
				dn_Poker_[poker_num_] == group_poker.poker[3] ||
				dn_Poker_[poker_num_] == group_poker.poker[4])
			{
				continue;
			}
			else
			{
				++poker_num_;
			}
		}   

		poker_min_value_ = minPoker;
		poker_max_value_ = maxPoker;

		return;
}

void DNOneDeckPoker::initPoker(int8_t minPoker, int8_t maxPoker)
{
    poker_num_         = 0;
    deal_position_   = 0;
    poker_min_value_ = 0;
    poker_max_value_ = 0;

    assert(minPoker <= maxPoker);

    int poker_count_color = maxPoker - minPoker + 1;
    int total_poker_count = DNPOKER_COLOR_COUNT * poker_count_color;

    for (int i=0; i<total_poker_count; ++i)
    {
        int8_t byValue = (i % poker_count_color) + minPoker;
        int8_t byColor = (i / poker_count_color) + 1;

        dn_Poker_[poker_num_].poker_value = byValue;
        dn_Poker_[poker_num_].poker_color = byColor;

        ++poker_num_;
    }

    poker_min_value_ = minPoker;
    poker_max_value_ = maxPoker;

    return;
}

void DNOneDeckPoker::shuffle(int16_t iTimes)
{
    static int x = 0;

    for (int iT=0; iT<iTimes; ++iT)
    {
        DNOnePoker median;
        for(int index = 0; index < getPokerNum(); index ++)
        {
            if (x > 1000000)
            {
                x = 0;
            }
            x+=10000;

            int iValue = index + rand() % (getPokerNum() - index);

            median              = dn_Poker_[index];
            dn_Poker_[index]  = dn_Poker_[iValue];
            dn_Poker_[iValue] = median;
        }
    }
    deal_position_ = 0;
    return;
}

bool DNOneDeckPoker::dealPoker(DNOnePoker* poker)
{
    if (deal_position_ < 0 || deal_position_ > getPokerNum() - 5)
    {
        return false;
    }
    poker[0] = dn_Poker_[deal_position_++];
    poker[1] = dn_Poker_[deal_position_++];
    poker[2] = dn_Poker_[deal_position_++];
    poker[3] = dn_Poker_[deal_position_++];
    poker[4] = dn_Poker_[deal_position_++];;
    return true;
}

DNOnePoker * DNOneDeckPoker::findMaxPoker(DNOnePoker poker[5])
{
    DNOnePoker * max_poker = &poker[0];
    for (int i = 1; i < 5; i++)
    {
        DNOnePoker * current_poker = &poker[i];

        if ((max_poker->poker_value < current_poker->poker_value) ||
            (max_poker->poker_value == current_poker->poker_value && max_poker->poker_color < poker[i].poker_color)
            )
        {
            max_poker = &poker[i];
        }
    }

    return max_poker;
}

bool DNOneDeckPoker::compareSinglePoker(DNOnePoker* poker_a, DNOnePoker* poker_b)
{
    if (poker_a->poker_value == poker_b->poker_value)
    {
        return poker_a->poker_color > poker_b->poker_color;
    }
    else
    {
        return poker_a->poker_value > poker_b->poker_value;
    }
}


bool DNOneDeckPoker::compareGroupPoker(DNGroupPoker & group_poker_a, bool byNiuType_a, DNGroupPoker & group_poker_b, bool byNiuType_b)
{
    //如果没有识别牌型，则识别牌型

    /*
    if (group_poker_a.isTypeIdentified() == false)
    {
        DNOneDeckPoker::findCow(group_poker_a, NULL, NULL);
    }
    if (group_poker_b.isTypeIdentified() == false)
    {
        DNOneDeckPoker::findCow(group_poker_b, NULL, NULL);
    }
    */

    //计算牌型：
    calcCowData(group_poker_a, byNiuType_a);
    calcCowData(group_poker_b, byNiuType_b);

    bool a_lose = false;
    //比较牛型，如果牛型相同，比较最大的手牌大小，直到花色
    int ret = compareNiutype(group_poker_a.dn_type, group_poker_b.dn_type);
    if (ret == 0)
    {
        //从5张牌中找到最大的牌,判断.左小于右边
        if (!compareSinglePoker(group_poker_a.max_poker, group_poker_b.max_poker))
        {
            a_lose = true;
        }
    }
    else if (ret == -1)
    {
        a_lose = true;
    }
    return a_lose;
}

int DNOneDeckPoker::compareNiutype(int32_t a_niu_type, int32_t b_niu_type)
{
    if (a_niu_type < b_niu_type)
    {
        return -1;
    }
    else if (a_niu_type == b_niu_type)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


int32_t DNOneDeckPoker::findCow(DNOnePoker poker[5], int32_t niuPokerIndex[5], int32_t otherPokerIndex[2])
{
    static const int32_t all_niu_poker_index[5] = { 0, 1, 2, 3, 4 };
    static const int32_t index[5]                = { -1, -1, -1, -1, -1 };
    int32_t     Niu_type    = DNCardType::MEI_NIU;

    int wuxiao_num = 0;
    int wuxiao_sum = 0;
    int wuhua_num = 0;

    int all_poker_sum = 0;

    //判断是否为五小牛、五花牛
    for (int i = 0; i < 5; ++i)
    {
        all_poker_sum += changeDNPokerJQK(poker[i].poker_value);

        if (poker[i].poker_value < 5)
        {
            ++wuxiao_num;
            wuxiao_sum += poker[i].poker_value;
        }
        else if (poker[i].poker_value > 10)
        {
            ++wuhua_num;
        }
    }

    if ( (wuxiao_num == 5)&&(wuxiao_sum <= 10))
    {
        // 判断是否为五小牛
        memcpy(niuPokerIndex, all_niu_poker_index, sizeof(int32_t)*5);
        return DNCardType::WU_XIAO_NIU;

    }
    else if (wuhua_num == 5)
    {
        //判断是否为五花牛
        memcpy(niuPokerIndex, all_niu_poker_index, sizeof(int32_t)*5);
        return DNCardType::WU_HUA_NIU;
    }

    //判断是否为四炸
    std::vector<std::pair<int32_t, int32_t> > sorted_pokers;
    for (int i=0;i<5;i++)
    {
        sorted_pokers.push_back(std::pair<int32_t, int32_t>(poker[i].poker_value, i));
    }

    std::sort(sorted_pokers.begin(), sorted_pokers.end(), pokerValueIndexPairComparator);

    int si_zha_count = 0;
    for (int i=1; i<5; i++)
    {
        std::pair<int32_t, int32_t> & cur  = sorted_pokers[i - 1];
        std::pair<int32_t, int32_t> & curPair = sorted_pokers[i];
        if (sorted_pokers[i-1].first == curPair.first)
        {
            niuPokerIndex[si_zha_count] = cur.second;
            si_zha_count++;
            if (si_zha_count == 3)
            {
                niuPokerIndex[si_zha_count] = curPair.second;
                return DNCardType::SI_ZHA;
            }
        }
        else
        {
            if (i >= 2)
            {
                // Fast quit.
                break;
            }
            si_zha_count = 0;
            otherPokerIndex[0] = cur.second;
        }
    }


    int nSum = 0;
    int i = 0, j = 0, k = 0;
    for(i = 0; i < 5; ++ i)
    {
        for(j = i + 1; j < 5; ++ j)
        {
            for(k = j + 1; k < 5; ++ k)
            {
                nSum = changeDNPokerJQK(poker[i].poker_value) + changeDNPokerJQK(poker[j].poker_value) + changeDNPokerJQK(poker[k].poker_value);

                if(nSum % 10 == 0)
                {
                    niuPokerIndex[0] = i;
                    niuPokerIndex[1] = j;
                    niuPokerIndex[2] = k;

                    int n = 0; 
                    int otherPokerIndex_index = 0;
                    for (int m = 0; m < 5; m++)
                    {
                        for (n = 0; n < 3; n++)
                        {
                            if (m == niuPokerIndex[n])
                            {
                                break; 
                            }
                        }
                        if (n >= 3)
                        {
                            otherPokerIndex[otherPokerIndex_index] = m; 
                            otherPokerIndex_index++; 
                            if (otherPokerIndex_index >= 2)
                            {
                                break; 
                            }
                        }
                    }
                    int32_t type = all_poker_sum % 10;
                    if (type == 0)
                    {
                        type = DNCardType::NIU_NIU;
                    }
                    return  type;
                    /*
                    // OtherPokerIndex.
                    qsort(niuPokerIndex, 3, sizeof(int32_t), pokerIndexComparator);
                    if (niuPokerIndex[0] == 2)
                    {
                        otherPokerIndex[0] = 0;
                        otherPokerIndex[1] = 1;
                    }
                    else
                    {
                        int otherPokerIndex_index = 0;
                        for (int i=1; i<3; i++)
                        {
                            int delta = niuPokerIndex[i] - niuPokerIndex[i-1];
                            for (int z=1;z<delta;z++)
                            {
                                otherPokerIndex[otherPokerIndex_index++] = niuPokerIndex[i-1] + z;
                            }
                            if (otherPokerIndex_index == 2)
                            {
                                break;
                            }
                        }
                        if (niuPokerIndex[0] == 1)
                        {
                            otherPokerIndex[1] = 0;
                        }
                    }

                    int32_t type = all_poker_sum % 10;
                    if (type == 0)
                    {
                        type = DNCardType::NIU_NIU;
                    }
                    return  type;
                    */
                }
            }
        }
    }
    memcpy(niuPokerIndex, index, sizeof(int32_t)*5);
    return Niu_type;
}

int32_t DNOneDeckPoker::hasCow(DNOnePoker poker[5], int32_t pokerIndex[5])
{
    //int32_t type = DNCardType::MEI_NIU;
    // TODO(fbzhong): Dupe check in PokerIndex.
    int pokerIndexCount = 0;
    for (int i=0;i<5;i++)
    {
        if (pokerIndex[i] >= 0 && pokerIndex[i] < 5)
        {
            pokerIndexCount++;
        }
        else
        {
            break;
        }
    }
    if (pokerIndexCount == 5)
    {
        // 特殊牌型：五花牛、五小牛
        //判断是否为五小牛
        int wuxiao_num = 0;
        int wuxiao_snum = 0;
        for (int i = 0; i < 5; ++i)
        {
            if (poker[i].poker_value < 5)
            {
                ++wuxiao_num;
                wuxiao_snum = wuxiao_snum + poker[i].poker_value;
            }
        }
        if ((wuxiao_num == 5)&& (wuxiao_snum <= 10))
        {
            return DNCardType::WU_XIAO_NIU;
        }

        //判断是否为五花牛
        int wuhua_num = 0;
        for (int i = 0; i < 5; ++i)
        {
            if (poker[i].poker_value > 10)
            {
                ++wuhua_num;
            }
        }

        if (wuhua_num == 5)
        {
            return DNCardType::WU_HUA_NIU;
        }
    }
    else if (pokerIndexCount == 4)
    {
        //判断是否为四炸
        for (int i = 1; i< pokerIndexCount; i++)
        {
            if (poker[pokerIndex[i]].poker_value != poker[pokerIndex[0]].poker_value)
            {
                return DNCardType::MEI_NIU;
            }
        }
        return DNCardType::SI_ZHA;
    }
    else if (pokerIndexCount == 3)
    {
        int nSum = 0;
        int totalSum = 0;

        for (int i=0; i<5; i++)
        {
            totalSum += changeDNPokerJQK(poker[i].poker_value);
        }
        for (int i=0; i<pokerIndexCount; i++)
        {
            nSum +=        changeDNPokerJQK(poker[pokerIndex[i]].poker_value);
        }

        if(nSum % 10 == 0)
        {
            int s = totalSum % 10;
            if (s == 0)
            {
                return DNCardType::NIU_NIU;
            }
            else
            {
                return s;
            }
        }
    }
    return DNCardType::MEI_NIU;
}

void DNOneDeckPoker::hasCow(DNGroupPoker &groupPoker, int32_t pokerIndex[5], int32_t otherPokerIndex[2])
{
    groupPoker.dn_type = hasCow(groupPoker.poker, pokerIndex);
    groupPoker.setNiuPokerIndex(pokerIndex, otherPokerIndex);
    groupPoker.setMaxPoker(findMaxPoker(groupPoker.poker));
}


void DNOneDeckPoker::findCow(DNGroupPoker &groupPoker, int32_t niuPokerIndex[5], int32_t otherPokerIndex[2])
{
    int32_t n_index[5] = {-1, -1, -1, -1, -1};
    int32_t o_index[2] = {-1, -1};

    if (niuPokerIndex == NULL)
    {
        niuPokerIndex = n_index;
    }
    if (otherPokerIndex == NULL)
    {
        otherPokerIndex = o_index;
    }

    groupPoker.dn_type = findCow(groupPoker.poker, niuPokerIndex, otherPokerIndex);
    groupPoker.setNiuPokerIndex(niuPokerIndex, otherPokerIndex);
    groupPoker.setMaxPoker(findMaxPoker(groupPoker.poker));
}

//计算牛型/牛型牌对应的索引/其他索引
void    DNOneDeckPoker::calcCowData(DNGroupPoker &groupPoker, bool bByPokerType)
{
    if (!groupPoker.bCalc)
    {
        if (bByPokerType)
        {
            //按照牛型来计算牌
            groupPoker.dn_type = findCow(groupPoker.poker, groupPoker.niu_poker_index, groupPoker.other_poker_index);
            groupPoker.setMaxPoker(findMaxPoker(groupPoker.poker));
            groupPoker.bCalc = true;
            return;
        }
        else
        {
            //按照单手牌来计算牌
            groupPoker.calcByNoNiu();
            groupPoker.setMaxPoker(findMaxPoker(groupPoker.poker));
            groupPoker.bCalc = true;
            return;
        }
    }
}



int32_t DNOneDeckPoker::changeDNPokerJQK(const int8_t& pokerValue)
{
    if (pokerValue>10)
    {
        return 10;
    }
    return pokerValue;
}


int32_t DNOneDeckPoker::DNPokerCompare(DNOnePoker& pokerA, DNOnePoker& pokerB)
{
    if (pokerA.poker_value == pokerB.poker_value)
    {
        if (pokerA.poker_color == pokerB.poker_color)
        {
            return 0;
        }
        else if (pokerA.poker_color > pokerB.poker_color)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }
    else if (pokerA.poker_value > pokerB.poker_value)
    {
        return 1;
    }
    else if (pokerA.poker_value < pokerB.poker_value)
    {
        return -1;
    }

    return 0;
}

bool DNOneDeckPoker::dealPokers(DNOnePoker * poker[], int32_t playerNumber)
{
    for (int i=0; i<playerNumber; i++)
    {
        dealPoker(poker[i]);
    }
    return true;
}

bool DNOneDeckPoker::CheckCowWithPokerInfo(int32_t niu_type, int32_t poker_index[kPokerNumOfGroup], DNGroupPoker& group_poker)
{
    //14-9-22更新：poker_index 前两个为非牛，后三个为牛， 若为没牛或四炸以上全是0， 故可知上述情况一定返回false，
    //检查给定5张牌的索引的正确性，索引在【0,4】，且不能重复
    int32_t poker_check[kPokerNumOfGroup] = {0};
    int32_t index = kPokerNumOfGroup - 1;
    for (; index >= 0; --index){
        //LOG(ERROR) << "index " << poker_index[index];
        if (poker_index[index] >= 0 && poker_index[index] < kPokerNumOfGroup){
            ++poker_check[poker_index[index]];
        }
    }
    for (index = 0; index < kPokerNumOfGroup; ++index){
        if (poker_check[index] != 1){
            return false;
        }
    }
    
    int32_t sum_of_poker = 0;
    int32_t num_of_xiao_niu_poker = 0;//小于5的牌数量
    int32_t num_of_hua_niu_poker = 0;//JQK的牌数量
    int32_t poker_value[MAX_DNPOKER_VALUE] = { 0 };//存放牌的个数，如有3张2则poker[2]=3
    
    for (index = 0; index < kPokerNumOfGroup; ++index){
        ++poker_value[group_poker.poker[index].poker_value];
        //LOG(ERROR) << "poker " << group_poker.poker[index].poker_value;
        if (group_poker.poker[index].poker_value > 10){
            sum_of_poker += 10;
            ++num_of_hua_niu_poker;
        }else{
            sum_of_poker += group_poker.poker[index].poker_value;
            if (group_poker.poker[index].poker_value < 5){
                ++num_of_xiao_niu_poker;
            }
        }
    }

    
    if (niu_type == DNCardType::WU_XIAO_NIU && num_of_xiao_niu_poker != kPokerNumOfGroup){
        //LOG(ERROR) << niu_type <<"non wu xiao niu";
        return false;
    }
    else if (niu_type == DNCardType::WU_HUA_NIU && num_of_hua_niu_poker != kPokerNumOfGroup){
        //LOG(ERROR) << niu_type << "non wu hua niu";
        return false;
    }
    else if (niu_type == DNCardType::SI_ZHA){
        bool    is_sizha = false;
        for (index = 0; index < kPokerNumOfGroup; ++index){
            if (poker_value[group_poker.poker[index].poker_value] == 4){
                is_sizha = true;
            }
        }
        if (!is_sizha){
            //LOG(ERROR) << niu_type << "non si zha";
            return false;
        }
    }
    else if (sum_of_poker % 10 != niu_type % 10){
        //LOG(ERROR) << niu_type << "non niu" << sum_of_poker;
        return false;
    } else {

    }

    //LOG(ERROR) << niu_type << "success" << sum_of_poker;
    group_poker.setMaxPoker(DNOneDeckPoker::findMaxPoker(group_poker.poker));
    group_poker.dn_type = niu_type;
    group_poker.bCalc = true;

    const int32_t no_niu_count = 2;
    for (index = 0; index < kPokerNumOfGroup; ++index){
        if (index < no_niu_count){
            group_poker.other_poker_index[index] = poker_index[index];
        } else {
            group_poker.niu_poker_index[index - no_niu_count] = poker_index[index];
        }
    }
    
    return true;
}

/*--------------------------------------------------------------*/

DNPokerSet::DNPokerSet(uint32_t card_type)
{
	card_type_ = card_type;
}

DNPokerSet::DNPokerSet()
{
	card_type_ = 0;
}

DNPokerSet::~DNPokerSet()
{
	card_type_ = 0;
	GroupPokers_.clear();
}

void DNPokerSet::AddGroupPoker( DNGroupPoker& Poker)
{
	GroupPokers_.push_back(Poker);	
}


uint32_t DNPokerSet::GetCardType()
{
	return card_type_;
}

uint32_t DNPokerSet::GetPokerGroupCount()
{
	return GroupPokers_.size();
}

const DNGroupPoker* DNPokerSet::GetRandomGroupPoker()
{
	LOG(DEBUG)<<" DNPokerSet::GetRandomGroupPoker ";
	uint32_t num = GroupPokers_.size();
	if( num == 0)
	{
		LOG(ERROR)<<" DNPokerSet::GetRandomGroupPoker Count == 0";
		return nullptr;
	}

    uint32_t index = num-1 - rand() % num;
	DNGroupPoker* group_poker = &GroupPokers_[index];
	group_poker->Confuse();
	return group_poker;
}
/*-------------------------------------------------------------------*/

DNPokerGen* DNPokerGen::GetInstance()
{
	static DNPokerGen obj;
	return &obj;
}

DNPokerGen::DNPokerGen()
{
	use_rand_poker_ = 0;
}

bool DNPokerGen::Init()
{
    LOG(DEBUG) << "DNPokerGen::Init BEGIN";
	for( char i = MIN_DNPOKER_VALUE; i<= MAX_DNPOKER_VALUE; i++)
	{
		for( char j = MIN_DNPOKER_VALUE; j <= MAX_DNPOKER_VALUE; j ++)
		{
			for( char m = MIN_DNPOKER_VALUE; m <= MAX_DNPOKER_VALUE; m ++)
			{
				for( char n = MIN_DNPOKER_VALUE; n <= MAX_DNPOKER_VALUE; n ++ )
				{
					for( char o = MIN_DNPOKER_VALUE; o <= MAX_DNPOKER_VALUE; o ++ )	
					{	
						DNGroupPoker group_poker;

						group_poker.poker[0].poker_value = i;
						group_poker.poker[0].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[1].poker_value = j;
						group_poker.poker[1].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[2].poker_value = m;
						group_poker.poker[2].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[3].poker_value = n;
						group_poker.poker[3].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						group_poker.poker[4].poker_value = o;
						group_poker.poker[4].poker_color = DNPokerColorType::DN_POKER_COLOR_DIAMONDS;

						if( group_poker.poker[0].poker_value == group_poker.poker[1].poker_value &&
							group_poker.poker[0].poker_value == group_poker.poker[2].poker_value &&
							group_poker.poker[0].poker_value == group_poker.poker[3].poker_value &&
							group_poker.poker[0].poker_value == group_poker.poker[4].poker_value )
							{
								continue;	
							}

						DNOneDeckPoker::calcCowData(group_poker, true);	


						DNPokerGen::GetInstance()->AddGroupPoker( group_poker );
					}
				}
			}
		}
	}
    LOG(DEBUG) << "DNPokerGen::Init END";

	std::map<uint32_t, DNPokerSet>::iterator pItr = poker_maps_.begin();
	for(; pItr != poker_maps_.end(); ++pItr)
	{
		DNPokerSet& r_set = pItr->second;
        LOG(DEBUG) << "DNPokerGen poker_type = " << r_set.GetCardType() << " poker_count = " << r_set.GetPokerGroupCount();
	}
	return true;	
}

bool DNPokerGen::Final()
{
	use_rand_poker_ = 0;
	return true;
}



void DNPokerGen::AddGroupPoker(DNGroupPoker& group_poker)
{
	uint32_t dn_type = group_poker.dn_type;
	assert( dn_type >= DNCardType::MEI_NIU  && dn_type < DNCardType::DN_TYPE_COUNT);
	std::map<uint32_t, DNPokerSet>::iterator pItr = poker_maps_.find( dn_type );
	if(  pItr != poker_maps_.end())
	{
		DNPokerSet& poker_set = pItr->second;
		poker_set.AddGroupPoker(group_poker);
	}
	else
	{
		DNPokerSet poker_set(dn_type);
		poker_set.AddGroupPoker(group_poker);
		poker_maps_[dn_type] = poker_set;
	}
}

void DNPokerGen::AddRateConfig(uint32_t dn_poker_type, uint32_t begin, uint32_t end)
{
    LOG(DEBUG) << " DNPokerGen::AddRateConfig poker_type = " << dn_poker_type << " begin_rate = " << begin << " end_rate = " << end;
	assert( end >= begin );
	PokerTypeRateConfig config( dn_poker_type, begin ,end);

	configs_.push_back(config);
}

void DNPokerGen::ClearConfig()
{
    LOG(DEBUG) << "DNPokerGen::ClearConfig";
	configs_.clear();
}

void DNPokerGen::UseRandPoker(uint32_t use)
{
    LOG(DEBUG) << " DNPokerGen::UseRandPoker use = " << use;
	use_rand_poker_ = use;
}

uint32_t DNPokerGen::GetUseRandFlag()
{
	return use_rand_poker_;
}


const PokerTypeRateConfig* DNPokerGen::GetConfigByPokerType(uint32_t poker_type)
{
	for( size_t i = 0; i < configs_.size(); i++)
	{
		const PokerTypeRateConfig& r_config = configs_[i];
		if( r_config.GetDNPokerType() == poker_type)
		{
			return &r_config;
		}
	}
	return nullptr;
}

const DNGroupPoker* DNPokerGen::GetRandomGroupPoker(uint32_t poker_type_begin, uint32_t poker_type_end)
{
	assert(poker_type_begin < poker_type_end);
	assert( poker_type_begin >= DNCardType::MEI_NIU && poker_type_begin <= DNCardType::WU_XIAO_NIU );
	assert( poker_type_end >= DNCardType::MEI_NIU && poker_type_end <= DNCardType::WU_XIAO_NIU );

	const PokerTypeRateConfig* p_begin_config = GetConfigByPokerType(poker_type_begin);
	const PokerTypeRateConfig* p_end_config = GetConfigByPokerType(poker_type_end);

	if(p_begin_config == nullptr)
	{
		LOG(ERROR)<<" DNPokerGen::GetRandomGroupPoker poker_type_begin = "<<poker_type_begin;
		return nullptr;
	}

	if(p_end_config == nullptr)
	{
		LOG(ERROR)<<" DNPokerGen::GetRandomGroupPoker poker_type_end = "<<poker_type_end;
		return nullptr;
	}

	uint32_t begin_rate = p_begin_config->GetBeginRate();
	uint32_t end_rate   = p_end_config->GetEndRate();

	if( end_rate <= begin_rate )
	{
		return nullptr;
	}


	int rand_num = begin_rate + rand() %  (end_rate - begin_rate );

	return GetRandomGroupPokerByRate(rand_num);
}

const DNGroupPoker* DNPokerGen::GetRandomGroupPoker()
{
	int rand_num = rand() % 10000;
	return GetRandomGroupPokerByRate(rand_num);
}

const DNGroupPoker* DNPokerGen::GetRandomGroupPokerByRate(uint32_t rand_num)
{
	LOG(DEBUG) << " DNPokerGen::GetRandomGrouPokerByRate rand_num = " << rand_num;
	uint32_t dn_poker_type = 0; 
	for( uint32_t i = 0; i< configs_.size(); i++) 
	{    
		PokerTypeRateConfig& r_config = configs_[i];
		if( r_config.IsInRate(  rand_num ) )
		{    
			dn_poker_type = r_config.GetDNPokerType();
			break;
		}    
	}    

	std::map<uint32_t, DNPokerSet>::iterator pItr = poker_maps_.find(dn_poker_type);
	if( pItr == poker_maps_.end() )
	{    
		LOG(ERROR)<<" Can Not Find DNPokerSet poker_type = "<<dn_poker_type;
		return nullptr;
	}    

	LOG(DEBUG) << " DNPokerGen::GetRandomGrouPoker poker_type = " << dn_poker_type;

	DNPokerSet& r_set = pItr->second;
	return r_set.GetRandomGroupPoker();
}

const DNGroupPoker* DNPokerGen::GetAssignGroupPoker(uint32_t poker_type)
{
    LOG(INFO) << " DNPokerGen::GetAssignGroupPoker poker_type = " << poker_type;
    std::map<uint32_t, DNPokerSet>::iterator pItr = poker_maps_.find(poker_type);
    if (pItr == poker_maps_.end())
    {
        LOG(ERROR) << " Can Not Find DNPokerSet poker_type = " << poker_type;
        return nullptr;
    }
    DNPokerSet& r_set = pItr->second;
    return r_set.GetRandomGroupPoker();
}


/*-------------------------------------------------------------------------*/
uint32_t PokerTypeRateConfig::GetDNPokerType() const
{
	return dn_poker_type_;
}


PokerTypeRateConfig::PokerTypeRateConfig(uint32_t dn_poker_type, uint32_t begin, uint32_t end)
{
	dn_poker_type_ = dn_poker_type;
	begin_rate_ = begin;
	end_rate_ = end;
}

PokerTypeRateConfig::PokerTypeRateConfig()
{
	dn_poker_type_ = 0;
	begin_rate_ = 0;
	end_rate_ = 0;
}

bool PokerTypeRateConfig::IsInRate(uint32_t rand_num) const
{
	return rand_num >= begin_rate_ && rand_num < end_rate_;
}


uint32_t PokerTypeRateConfig::GetBeginRate() const
{
	return begin_rate_;
}


uint32_t PokerTypeRateConfig::GetEndRate() const
{
	return end_rate_;
}

