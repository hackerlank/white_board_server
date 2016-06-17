//-------------------------------------------------------------*-C++-*---------
//							CbTime头文件
//-----------------------------------------------------------------------------
//	版本号 1.0
//-----------------------------------------------------------------------------
//	文件名: Time_span_time.h
//-----------------------------------------------------------------------------
//	
//	修改人员		修改日期		修改内容
//
//	陈厚道			2007.3			初步设计
//-----------------------------------------------------------------------------
//	时间,日期的基本操作
//

#ifndef FRAMEWORK_BASE_CB_SPAN_TIME_H
#define FRAMEWORK_BASE_CB_SPAN_TIME_H

#include <time.h>
#include <inttypes.h>
#include <string>

class CbSpanTime
{
public:
	inline CbSpanTime()
	{
	}	
	inline CbSpanTime( const CbSpanTime& time_span_src )
	{
		time_span_	=	time_span_src.time_span_;
	}	
	inline CbSpanTime( time_t time )
	{
		time_span_	=	time;
	}
	inline CbSpanTime( int32_t days, int hours, int mins, int secs )
	{
		time_span_ = secs + 60* (mins + 60* (hours + 24* days));
	}
	//! attribute
	inline int32_t GetDays()
	{
		return time_span_ / (24*3600);
	}
	inline int32_t	GetHours()
	{
		return (int)(GetTotalHours() - GetDays()*24);
	}
	inline int32_t	GetTotalHours()
	{
		return time_span_/3600;
	}
	inline int32_t	GetMinutes()
	{
		return (int)(GetTotalMinutes() - GetTotalHours()*60);
	}
	inline int32_t	GetTotalMinutes()
	{
		return time_span_/60;
	}
	inline int32_t	GetSeconds()
	{
		return (int)(GetTotalSeconds() - GetTotalMinutes()*60);
	}

	inline int32_t	GetTotalSeconds()
	{
		return time_span_;
	}

	//! conversion
	inline std::string	Format(char* pszChar) 
	{
		//! TO DO 
		return "";
	}

	//! operators
	
	inline const CbSpanTime& operator= ( const CbSpanTime& time_span_src )
	{
		time_span_	=	time_span_src.time_span_;
		return *this;
	}
	inline CbSpanTime operator+ ( CbSpanTime time_span ) 
	{
		return CbSpanTime(time_span_ + time_span.time_span_);
	}
	inline CbSpanTime operator- ( CbSpanTime time_span ) 
	{
		return CbSpanTime(time_span_ - time_span.time_span_); 
	}
	inline const CbSpanTime& operator+=	( CbSpanTime time_span )
	{
		time_span_ += time_span.time_span_;
		return *this;
	}
	inline const CbSpanTime& operator-=	( CbSpanTime time_span )
	{
		time_span_ -= time_span.time_span_;
		return *this;
	}
	inline bool operator ==( CbSpanTime time_span ) 
	{
		return time_span_ == time_span.time_span_;
	}
	inline bool operator !=( CbSpanTime time_span ) 
	{
		return time_span_ != time_span.time_span_;		
	}
	inline bool operator <( CbSpanTime time_span ) 
	{
		return time_span_ <	time_span.time_span_; 
	}
	inline bool operator >( CbSpanTime time_span ) 
	{
		return time_span_ >	time_span.time_span_; 
		
	}
	inline bool operator <=( CbSpanTime time_span ) 
	{
		return time_span_ <=	time_span.time_span_; 
	}
	inline bool operator >=( CbSpanTime time_span ) 
	{
		return time_span_ >=	time_span.time_span_; 
	}

private:
	time_t time_span_;
	friend class CbTime;
};



#endif //FRAMEWORK_BASE_CB_SPAN_TIME_H


