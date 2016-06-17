#include "cb_time.h"

#include <assert.h>
#include <string.h>

#define HOUR_SECONDS (60*60)
#define DAY_SECONDS (24*60*60)
#define WEEK_SECONDS (24*60*60*7)

CbTime::CbTime(int year, int month, int day, int hour, int min, int sec,
	int dst)
{
	struct tm atm;
	atm.tm_sec = sec;
	atm.tm_min = min;
	atm.tm_hour = hour;
	assert(day >= 1 && day <= 31);
	atm.tm_mday = day;
	assert(month >= 1 && month <= 12);
	atm.tm_mon = month - 1;        // tm_mon is 0 based
	assert(year >= 1900);
	atm.tm_year = year - 1900;     // tm_year is 1900 based
	atm.tm_isdst = dst;
	time_ = mktime(&atm);
	assert(time_ != -1);			// indicates an illegal input time
}

CbTime CbTime::GetCurrentTime()
// return the current system time
{
	return CbTime(::time(NULL));
}

struct tm* CbTime::GetGmtTm(struct tm* ptm) 
{
	if (ptm != NULL)
	{
		*ptm = *gmtime(&time_);
		return ptm;
	}
	else
		return gmtime(&time_);
}

struct tm* CbTime::GetLocalTm(struct tm* ptm) 
{
	if (ptm != NULL)
	{
		struct tm* ptmTemp = localtime(&time_);
		if (ptmTemp == NULL)
			return NULL;    // indicates the time_ was not initialized!

		*ptm = *ptmTemp;
		return ptm;
	}
	else
		return localtime(&time_);
}

bool	CbTime::IsSameYear(CbTime time_src)
{
	if  (this->GetYear() ==	time_src.GetYear())
	{
		return true; 
	}
	else
	{
		return false; 
	}
}


bool	CbTime::IsSameMonth(CbTime time_src)
{
	if  (this->GetYear()	==	time_src.GetYear())
	{
		if  (this->GetMonth()	==	time_src.GetMonth())
		{
			return true; 
		}
		else
		{
			return false; 
		}
	}
	else
	{
		return false; 
	}
	
}

bool	CbTime::IsSameDay(CbTime time_src)
{
	if  (this->GetYear()	==	time_src.GetYear())
	{
		if  (this->GetMonth()	==	time_src.GetMonth())
		{
			if  (this->GetDay()	==	time_src.GetDay())
			{
				return true; 
			}
			else
			{
				return false; 
			}
		}
		else
		{
			return false; 
		}
	}
	else
	{
		return false; 
	}
}


std::string CbTime::Format(const char* format)
{
	//!TO DO 
	return "";
}

//!与字符串的互相转换，目前仅仅支持标准格式"XXXX-XX-XX XX:XX:XX",如"2007-10-09 09:12:05"
std::string	CbTime::ToDefaultString()
{
	return TimeToDefaultString(time_);
}
bool		CbTime::FromDefaultString(const char* time)
{
	time_ = TimeFromDefaultString(time);
	return time_ > 0;
}

std::string	CbTime::TimeToDefaultString(time_t t)
{
	struct tm * pt = localtime(&t);
	char str[30];
	sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d", 
		pt->tm_year + 1900,
		pt->tm_mon + 1,
		pt->tm_mday,
		pt->tm_hour,
		pt->tm_min,
		pt->tm_sec);

	return std::string(str);

}
time_t	CbTime::TimeFromDefaultString(const char* time)
{
	time_t t = 0;
	if(time && strlen(time) == 19)
	{
		char* psz = (char*)time;

		psz[4]		= 0;
		int year	= atoi(psz);
		psz[4]		= '-';
		psz			+= 5;

		psz[2]		= 0;
		int month	= atoi(psz);
		psz[2]		= '-';
		psz			+= 3;

		psz[2]		= 0;
		int day	= atoi(psz);
		psz[2]		= ' ';
		psz			+= 3;

		psz[2]		= 0;
		int hour	= atoi(psz);
		psz[2]		= ':';
		psz			+= 3;

		psz[2]		= 0;
		int min	= atoi(psz);
		psz[2]		= ':';
		psz			+= 3;

		int sec	= atoi(psz);

		CbTime ct(year,month,day,hour,min,sec);
		t = ct.time_;
	}

	return t;
}

int32_t GetTimeZone(){
    time_t time_utc;
    struct tm tm_local;

    time(&time_utc);
 
    localtime_r(&time_utc, &tm_local);

   // time_t time_local;
    struct tm tm_gmt;

    //time_local = mktime(&tm_local);

    gmtime_r(&time_utc, &tm_gmt);

    int32_t time_zone = tm_local.tm_hour - tm_gmt.tm_hour;
    if (time_zone < -12) {
        time_zone += 24;
    }
    else if (time_zone > 12) {
        time_zone -= 24;
    }
    return time_zone;
}

bool IsSameDay(time_t tm1, time_t tm2)
{
    tm1 += HOUR_SECONDS*GetTimeZone();
    tm2 += HOUR_SECONDS*GetTimeZone();
	return (tm1 / DAY_SECONDS) == (tm2/ DAY_SECONDS);
}
 
bool IsSameMonth(time_t tm1, time_t tm2)
{
	struct tm  stm1 = {0};
	struct tm  stm2 = {0};

	if( localtime_r(&tm1, &stm1) == nullptr)
	{
		return false;
	}

	if( localtime_r(&tm2, &stm2) == nullptr)
	{
		return false;
	}
	return  ((stm1.tm_year == stm2.tm_year) && (stm1.tm_mon == stm2.tm_mon));
}
 
bool IsSameWeek(time_t tm1, time_t tm2)
{
    tm1 += HOUR_SECONDS*GetTimeZone();
    tm2 += HOUR_SECONDS*GetTimeZone();
	return (tm1/ WEEK_SECONDS) == (tm2/ WEEK_SECONDS);
}

bool IsSameYear(time_t tm1,  time_t tm2)
{
	struct tm  stm1 = {0};
	struct tm  stm2 = {0};

	if( localtime_r(&tm1, &stm1) == nullptr)
	{
		return false;
	}
	
	if( localtime_r(&tm2, &stm2) == nullptr)
	{
		return false;
	}

	return  ((stm1.tm_year == stm2.tm_year) );
}


bool IsYesterDay(time_t tm_yesterday, time_t tm_today){
    tm_yesterday += HOUR_SECONDS*GetTimeZone();
    tm_today += HOUR_SECONDS*GetTimeZone();
    if ((tm_today / DAY_SECONDS) - (tm_yesterday / DAY_SECONDS) ==1)
    {
        return true;
    }
    return false;

}








