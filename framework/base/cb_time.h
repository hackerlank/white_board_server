//-------------------------------------------------------------*-C++-*---------
//							CbTimeͷ�ļ�
//-----------------------------------------------------------------------------
//	�汾�� 1.0
//-----------------------------------------------------------------------------
//	�ļ���: Time_time.h
//-----------------------------------------------------------------------------
//	
//	�޸���Ա		�޸�����		�޸�����
//
//	�º��			2007.3			�������
//-----------------------------------------------------------------------------
//	ʱ��,���ڵĻ�������
//

#ifndef	FRAMEWORK_BASE_CBTime_H_
#define FRAMEWORK_BASE_CBTime_H_

#include <time.h>
#include "cb_span_time.h"
//
class CbTime
{

public:
	static CbTime GetCurrentTime();	
	CbTime()
	{
	}
	CbTime( const CbTime& time_src )
	{
		time_ = time_src.time_;
	}
	CbTime( time_t time )
	{
		time_ =	time;
	}	
	CbTime( int year, int month, int day, int hour, int min, int sec,int dst = -1);

	//! conversion
	struct tm* GetGmtTm( struct tm* ptm = NULL ) ;
	struct tm* GetLocalTm( struct tm* ptm = NULL ) ;
	

	//! attribute
	time_t	GetTime()
	{
		return time_;
	}
	int32_t	GetYear()
	{
		return (GetLocalTm(NULL)->tm_year) + 1900; 		
	}
	int32_t	GetMonth()
	{
		return GetLocalTm(NULL)->tm_mon + 1;
	}
	int32_t	GetDay()
	{
		return GetLocalTm(NULL)->tm_mday;
	}
	int32_t	GetHour()
	{
		return GetLocalTm(NULL)->tm_hour;
	}
	int32_t	GetMinute()
	{
		return GetLocalTm(NULL)->tm_min;
	}
	int32_t	GetSecond()
	{
		return GetLocalTm(NULL)->tm_sec;
	}
	int32_t	GetDayOfWeek()
	{
		return GetLocalTm(NULL)->tm_wday + 1;
	}

	//!�Ƚ�����
	bool	IsSameYear(CbTime time_src);

	bool	IsSameMonth(CbTime time_src);

	bool	IsSameDay(CbTime time_src);
	

	//!���ַ����Ļ���ת����Ŀǰ����֧�ֱ�׼��ʽ"XXXX-XX-XX XX:XX:XX",��"2007-10-09 09:12:05"
	std::string		ToDefaultString();	
	bool			FromDefaultString(const char* time);	
	static std::string	TimeToDefaultString(time_t t);	
	static time_t	TimeFromDefaultString(const char* time);	
	
	std::string	Format(const char* format);	

	//! operator

	const CbTime& operator =( const CbTime& time_src )
	{
		time_	=	time_src.time_;
		return *this; 
	}
	const CbTime& operator =( time_t t )
	{
		time_	=	t;
		return *this;
	}
	CbTime operator +( CbSpanTime time_span ) 
	{
		return CbTime(time_ + time_span.time_span_); 
	}
	CbTime operator -( CbSpanTime time_span ) 
	{
		return CbTime(time_ - time_span.time_span_); 
	}
	CbSpanTime operator -( CbTime time ) 
	{
		return CbSpanTime(time_ - time.time_); 
	}
	const CbTime& operator +=( CbSpanTime time_span )
	{
		time_  += time_span.time_span_; 
		return *this; 
	}
	const CbTime& operator -=( CbSpanTime time_span )
	{
		time_ -= time_span.time_span_;
		return *this; 
	}

	bool operator ==( CbTime time ) 
	{
		return time_ == time.time_; 
	}
	bool operator !=( CbTime time ) 
	{
		return time_ != time.time_; 
	}
	bool operator <( CbTime time ) 
	{
		return time_ < time.time_;
	}
	bool operator >( CbTime time ) 
	{
		return time_ > time.time_;
	}
	bool operator <=( CbTime time ) 
	{
		return time_ <= time.time_; 
	}
	bool operator >=( CbTime time ) 
	{
		return time_ >= time.time_; 
	}

private:
	
	time_t time_;

};


int32_t GetTimeZone();
bool IsSameDay(time_t tm1, time_t tm2);
bool IsSameWeek(time_t tm1, time_t tm2);
bool IsSameMonth(time_t tm1, time_t tm2);
bool IsSameYear(time_t tm1,  time_t tm2);
bool IsYesterDay(time_t tm_yesterday, time_t tm_today);

#endif //FRAMEWORK_BASE_CBTime_H_


