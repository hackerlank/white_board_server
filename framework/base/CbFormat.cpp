#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "CbFormat.h"
#include "logger.h"
#include "logging.h"



//静态常量
	/*
#ifdef CBWINDOWS    
	std::string const   CbUtility::FILE_SEPARATOR				= "\\";
#else
	std::string const   CbUtility::FILE_SEPARATOR				= "/";
	#endif
*/
	std::string const   CbUtility::FILE_SEPARATOR				= "/";


	size_t		const   CbUtility::Size_MaxStringLen			= 524288;
	char		const	CbUtility::hexChars[]					= "0123456789ABCDEF";
	char		const	CbUtility::base64Char[]					= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//std::string const	CbUtility::_PROGRAM_ROOT_PATH			= CbUtility::getProgramHome();

	//静态变量
	int64_t			CbUtility::_PROGRAM_START_TIME			= CbUtility::get_start_time();
	int64_t			CbUtility::m_tPrevLoopTime				= CbUtility::get_start_time();
	int64_t			CbUtility::m_tCurLoopTime				= CbUtility::get_start_time(); 
	char 			CbUtility::base64DecodeTable[256]		= {0};


	std::string CbUtility::formatString(const char* fmt, ...)
	{
		char caMsg[Size_MaxStringLen] = { 0 };
		va_list argptr;
		va_start(argptr, fmt);
#ifdef _MSC_VER
		vsprintf_s(caMsg, sizeof(caMsg)-1, fmt, argptr);
#else
		vsprintf(caMsg, fmt, argptr);
#endif 
		va_end(argptr);
		return caMsg;
	}

	/*
	char *ctk_iconv(const char *fromStr, const int fromLen, char**toStr, const char *fromCode, const char *toCode)
	{
		char *buffer;
		iconv_t cd;
		const char *inbuf = NULL;
		size_t inbytesleft = 0;
		char *outbuf = NULL;
		size_t outbytesleft = 0;
		int errorCode = 0;
		int bufferSize = 0;
		size_t ret = 0;
		int done = 0;

		if (fromStr == NULL || fromStr[0] == '\0' || fromLen <= 0) return NULL;
		if (fromCode == NULL || fromCode[0] == '\0') return NULL;
		if (toCode == NULL || toCode[0] == '\0') return NULL;

		memset(&cd, 0x00, sizeof(iconv_t));
		inbuf = fromStr;
		inbytesleft = fromLen;

		errorCode = 0;
		bufferSize = fromLen * 4 + 1;
		buffer = (char*)malloc(sizeof(char)*bufferSize);
		memset(buffer, 0x00, bufferSize);

		outbuf = buffer;
		outbytesleft = bufferSize;

		if ((iconv_t)-1 == (cd = iconv_open(toCode, fromCode))) {
			return NULL;
		}

		while (inbytesleft > 0 && done != 1) {
			ret = iconv(cd, (char**)&inbuf, &inbytesleft, &outbuf, &outbytesleft);
			if ((size_t)-1 == ret) {
				errorCode = errno;
				switch (errorCode)
				{
				case EILSEQ:
				{
							   if ((outbuf < buffer + bufferSize) && (outbuf >= buffer))
							   {
								   memcpy(outbuf, inbuf, 1);
								   outbuf += 1;
								   outbytesleft -= 1;
								   inbuf += 1;
								   inbytesleft -= 1;
								   if (inbytesleft <= 0) break;
							   }
				}
					break;
				case EINVAL:
				{
							   done = 1;
				}
					break;
				case E2BIG:
				{
							  done = 1;
							  break;
				}
					break;
				default:
					done = 1;
				}
			}
		}
		if (NULL != toStr)
			*toStr = buffer;
		iconv_close(cd);
		return buffer;
	}
	*/

	StringArr CbUtility::split(const std::string& strData, const char* caSplit, uint32_t maxGroup)
	{
		StringArr rtn;
		if (caSplit == NULL)
		{
			rtn.push_back(strData);
		}
		else
		{
			std::string::size_type start = strData.find_first_not_of(caSplit);
			if (start != std::string::npos)
			{
				std::string::size_type end = strData.find_first_of(caSplit, start);
				while (end != std::string::npos && end > start)
				{
					rtn.push_back(strData.substr(start, end - start));
					start = strData.find_first_not_of(caSplit, end);
					end = strData.find_first_of(caSplit, start);
					if (--maxGroup == 0)
					{
						break;
					}
				}
				if (start != std::string::npos)
				{
					rtn.push_back(strData.substr(start));
				}
			}
			else
			{
				rtn.push_back(strData);
			}

		}

		return rtn;
	}



	std::string  CbUtility::ctk_iconv(const char *fromStr, const int fromLen, const char *fromCode, const char *toCode)
	{
		std::string returnResult; 
		char *buffer;
		iconv_t cd;
		const char *inbuf		= nullptr;
		size_t inbytesleft		= 0;
		char *outbuf = nullptr;
		size_t outbytesleft		= 0;
		int errorCode			= 0;
		int bufferSize			= 0;
		
		int done				= 0;

		if (fromStr == nullptr || fromStr[0] == '\0' || fromLen <= 0)
		{
			return returnResult;
		}
		if (fromCode == nullptr || fromCode[0] == '\0')
		{
			return returnResult; 
		}
		if (toCode == nullptr || toCode[0] == '\0')
		{
			return returnResult; 
		}

		memset(&cd, 0x00, sizeof(iconv_t));
		inbuf		= fromStr;
		inbytesleft = fromLen;

		if ((iconv_t)-1 == (cd = iconv_open(toCode, fromCode)))
		{
			return nullptr;
		}

		errorCode = 0;
		bufferSize = fromLen * 4 + 1;
		buffer = (char*)malloc(sizeof(char)*bufferSize);
		memset(buffer, 0x00, bufferSize);

		outbuf = buffer;
		outbytesleft = bufferSize;


		while (inbytesleft > 0 && done != 1) 
		{
            size_t ret = 0;
			ret = iconv(cd, (char**)&inbuf, &inbytesleft, &outbuf, &outbytesleft);
			if ((size_t)-1 == ret) 
			{
				errorCode = errno;
				switch (errorCode)
				{
				case EILSEQ:
				{
					if ((outbuf < buffer + bufferSize) && (outbuf >= buffer))
					{
						memcpy(outbuf, inbuf, 1);
						outbuf += 1;
						outbytesleft -= 1;
						inbuf += 1;
						inbytesleft -= 1;
						if (inbytesleft <= 0)
							break;
					}
				}
				break;
				case EINVAL:
				{
					 done = 1;
				}
				break;
				case E2BIG:
				{
					done = 1;
					break;
				}
					break;
				default:
					done = 1;
				}
			}
		}	
        returnResult.append(buffer);
		free(buffer); 
		iconv_close(cd);
		return returnResult;
	}




	std::string CbUtility::toUTF8(const char* caData)
	{
		std::string dataUtf8;
		size_t dataLen = strlen(caData);
		dataUtf8 += caData; 
		return dataUtf8; 
#ifdef _MSC_VER
		std::wstring dataUnicode;
		dataUnicode.resize(dataLen);
		MultiByteToWideChar(CP_ACP, 0
			, caData, -1
			, &dataUnicode[0], dataUnicode.size()
			);
		dataUtf8.resize(dataUnicode.size() * 3);
		WideCharToMultiByte(CP_UTF8, 0
			, dataUnicode.c_str(), dataUnicode.size()
			, &dataUtf8[0], dataUtf8.size(), NULL, NULL);
#else
		//GB18030,GBK

		iconv_t cd;
		cd = iconv_open("utf-8", "gbk");
		size_t utf8Len = dataLen * 3 + 1;

		char* pOutputPtr = new char[utf8Len]; 
		memset(pOutputPtr, 0, utf8Len); 
		char* pTempOutBuffer = pOutputPtr; 
		int32_t nRes = iconv(cd, (char**)&caData, &dataLen, &pTempOutBuffer, &utf8Len);
		dataUtf8 += pOutputPtr; 
		delete[]  pOutputPtr; 

		//assert(dataLen  == 0); 
		//无效的或不完整的多字节字符或宽字符,EILSEQ:84
		LOG(INFO) << " Origin String   " << caData << " , LeftLength " << dataLen << ", Res " << nRes << ",  outLen " << utf8Len << " error " << errno;
		LOG(INFO) << " OutString   " << dataUtf8.c_str() << " length ," << strlen(dataUtf8.c_str());  

		iconv_close(cd); 
#endif // #ifndef _MSC_VER
		return dataUtf8;
	}

	std::string CbUtility::toANSIIFromUTF8(const char* caData)
	{
		std::string dataANSII;
		
		dataANSII += caData;
		return dataANSII;

#ifdef _MSC_VER
		std::wstring dataUnicode;
		dataUnicode.resize(strlen(caData));
		MultiByteToWideChar(CP_UTF8, 0
			, caData, -1
			, &dataUnicode[0], dataUnicode.size()
			);
		std::string dataANSII;
		dataANSII.resize(dataUnicode.size() * 2);
		WideCharToMultiByte(CP_ACP, 0
			, dataUnicode.c_str(), dataUnicode.size()
			, &dataANSII[0], dataANSII.size(), NULL, NULL);
		return dataANSII.c_str();
#else
		
        size_t dataLen = strlen(caData);
		dataANSII.resize(dataLen * 4);
		size_t dataANSIISize = dataANSII.size();
		iconv_t cd;
		cd = iconv_open("gb18030", "utf-8");
		char* outptr = &dataANSII[0];
		iconv(cd, (char**)&caData, &dataLen, &outptr, &dataANSIISize);
		iconv_close(cd);

		assert(dataLen == 0);
		return dataANSII;
#endif
	}

    std::string CbUtility::getLocalIp()
    {
        int sock_get_ip;
        char ipaddr[50];

        struct   sockaddr_in *sin;
        struct   ifreq ifr_ip;

        if ((sock_get_ip = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("socket create failse...GetLocalIp!/n");
            return "";
        }

        memset(&ifr_ip, 0, sizeof(ifr_ip));
        strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);

        if (ioctl(sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0)
        {
            return "";
        }
        sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
        strcpy(ipaddr, inet_ntoa(sin->sin_addr));

        close(sock_get_ip);

        return std::string(ipaddr);
    }

	/*
	std::string& CbUtility::appendANSII(std::string& outStr, const void* data, uint32_t len, const char* caSplit)
	{
		const uint8_t* caOriginal = (const uint8_t*)data;
		while(len-- > 0)
		{
			if (caSplit)
			{
				outStr += caSplit;
			}
			outStr += hexChars[*caOriginal >> 4];
			outStr += hexChars[*caOriginal & 15];
			++caOriginal;
		}

		return outStr;
	}
	std::string CbUtility::toANSII(const void* data, uint32_t len, const char* caSplit)
	{
		std::string rtnValue;
		appendANSII(rtnValue, data, len, caSplit);
		return rtnValue;
	}

	std::string CbUtility::toBase64(const void* data, uint32_t len)
	{
		if (data == NULL || len == 0) return "";
		const uint8_t* orig = (const uint8_t*)data;

		uint32_t numOrig24BitValues = len/3;
		bool havePadding = len > numOrig24BitValues*3;
		bool havePadding2 = len == numOrig24BitValues*3 + 2;
		unsigned const numResultBytes = 4*(numOrig24BitValues + havePadding);

		std::string rtnValue;
		rtnValue.resize(numResultBytes);
		char* result = &rtnValue[0];

		// Map each full group of 3 input bytes into 4 output base-64 characters:
		uint32_t i = 0;
		for (; i < numOrig24BitValues; ++i) 
		{
			result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];
			result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
			result[4*i+2] = base64Char[((orig[3*i+1]<<2) | (orig[3*i+2]>>6))&0x3F];
			result[4*i+3] = base64Char[orig[3*i+2]&0x3F];
		}

		// Now, take padding into account.  (Note: i == numOrig24BitValues)
		if (havePadding) 
		{
			result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];
			if (havePadding2)
			{
				result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
				result[4*i+2] = base64Char[(orig[3*i+1]<<2)&0x3F];
			} 
			else 
			{
				result[4*i+1] = base64Char[((orig[3*i]&0x3)<<4)&0x3F];
				result[4*i+2] = '=';
			}
			result[4*i+3] = '=';
		}

		return rtnValue;
	}
	void CbUtility::initBase64DecodeTable()
	{
		if (base64DecodeTable[0] == 0x80)
		{
			return;
		}
		int i;
		for (i = 0; i < 256; ++i) base64DecodeTable[i] = (char)0x80;
		for (i = 'A'; i <= 'Z'; ++i) base64DecodeTable[i] = 0 + (i - 'A');
		for (i = 'a'; i <= 'z'; ++i) base64DecodeTable[i] = 26 + (i - 'a');
		for (i = '0'; i <= '9'; ++i) base64DecodeTable[i] = 52 + (i - '0');
		base64DecodeTable[(unsigned char)'+'] = 62;
		base64DecodeTable[(unsigned char)'/'] = 63;
		base64DecodeTable[(unsigned char)'='] = 0;
	}
	std::string CbUtility::fromBase64(const void* data, uint32_t len)
	{
		CbUtility::initBase64DecodeTable();
		std::string rtnValue;
		const uint8_t* caOriginal = (const uint8_t*)data;
		for (uint32_t i = 0; i < len - 3; i += 4)
		{
			rtnValue += (base64DecodeTable[*(caOriginal + 0)] << 2 ) | (base64DecodeTable[*(caOriginal + 1)] >> 4 );
			rtnValue += (base64DecodeTable[*(caOriginal + 1)] << 4 ) | (base64DecodeTable[*(caOriginal + 2)] >> 2 );
			rtnValue += (base64DecodeTable[*(caOriginal + 2)] << 6 ) | (base64DecodeTable[*(caOriginal + 3)] );
		}
		return rtnValue;
	}

	std::string CbUtility::toURLEncode(const void* data, uint32_t len, bool isPath)
	{
		std::string rtnValue;
		const uint8_t* caOriginal = (const uint8_t*)data;
		while(len-- > 0)
		{
			if (!isPath && *caOriginal == ' ')
			{
				rtnValue += '+';
			}
			else if ((*caOriginal < '0' && *caOriginal != '-' && *caOriginal != '.')
				||(*caOriginal < 'A' && *caOriginal > '9')
				||(*caOriginal > 'Z' && *caOriginal < 'a' && *caOriginal != '_')
				||(*caOriginal > 'z'))
			{
				appendANSII(rtnValue, caOriginal, 1, "%");
			}
			else
			{
				rtnValue += *caOriginal;
			}
			++caOriginal;
		}

		return rtnValue;
	}
	std::string CbUtility::fromURLEncode(const void* data, uint32_t len)
	{
		std::string rtnValue;
		const uint8_t* caOriginal = (const uint8_t*)data;
		while(len-- > 0)
		{
			if (*caOriginal == '+')
			{
				rtnValue += ' ';
			}
			else if(*caOriginal == '%' && len >= 2
				&& isxdigit(*(caOriginal+1)) 
				&& isxdigit(*(caOriginal+2)) 
				)
			{
				int c = *(caOriginal+1);
				if (isupper(c))	c = tolower(c);
				int v = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
				c = *(caOriginal+2);
				if (isupper(c))	c = tolower(c);
				v += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

				rtnValue += (char)v ;
				caOriginal += 2;
				len -= 2;
			}
			else
			{
				rtnValue += *caOriginal;
			}
			++caOriginal;
		}
		return rtnValue;
	}
	std::string CbUtility::formatString(const char* fmt, ...)
	{
		char caMsg[Size_MaxStringLen] = {0};
		va_list argptr;
		va_start(argptr, fmt);
#ifdef _MSC_VER
		vsprintf_s(caMsg, sizeof(caMsg )- 1, fmt, argptr);
#else
		vsprintf(caMsg, fmt, argptr);
#endif 
		va_end(argptr);
		return caMsg;
	}

	void CbUtility::replace(std::string& strSource, const std::string& strReplace, const std::string& strNew)
	{
		for(size_t curPos = strSource.find(strReplace)
			; curPos != std::string::npos
			; curPos = strSource.find(strReplace, curPos + strNew.size()))
		{
			strSource.replace(curPos, strReplace.size(), strNew);
		}
	}

	std::string CbUtility::replace(const char* strSourceData, const char* strReplaceData, const char* newData)
	{
		if ( strSourceData == NULL )
		{
			return "";
		}
		if (strReplaceData == NULL)
		{
			return strSourceData;
		}
		if ( newData == NULL )
		{
			newData = "";
		}
		std::string strSource = strSourceData;
		std::string strReplace = strReplaceData;
		std::string strNew = newData;
		replace(strSource, strReplace, strNew);
		return strSource;
	}

	StringArr CbUtility::split(const std::string& strData, const char* caSplit, uint32_t maxGroup)
	{
		StringArr rtn;
		if (caSplit == NULL)
		{
			rtn.push_back(strData);
		}
		else
		{
			std::string::size_type start = strData.find_first_not_of(caSplit);
			if (start != std::string::npos)
			{
				std::string::size_type end = strData.find_first_of(caSplit, start);
				while(end != std::string::npos && end > start)
				{
					rtn.push_back(strData.substr(start, end - start));
					start = strData.find_first_not_of(caSplit, end);
					end = strData.find_first_of(caSplit, start);
					if (--maxGroup == 0)
					{
						break;
					}
				}
				if (start != std::string::npos)
				{
					rtn.push_back(strData.substr(start));
				}
			}
			else
			{
				rtn.push_back(strData);
			}

		}

		return rtn;
	}

std::string CbUtility::toUTF8(const char* caData)
{
		std::string dataUtf8;
		size_t dataLen = strlen(caData);
		dataUtf8.resize(dataLen * 3);
#ifdef _MSC_VER
		std::wstring dataUnicode;
		dataUnicode.resize(dataLen);
		MultiByteToWideChar(CP_ACP, 0
		, caData, -1
		, &dataUnicode[0], dataUnicode.size()
		);
		dataUtf8.resize(dataUnicode.size()*3);
		WideCharToMultiByte(CP_UTF8,0
		, dataUnicode.c_str(), dataUnicode.size()
		, &dataUtf8[0], dataUtf8.size(), NULL, NULL);
#else
		iconv_t cd;
		cd = iconv_open("utf-8", "gb2312");
		size_t utf8Len = dataUtf8.size();
		char* outptr = &dataUtf8[0];
		size_t res = iconv(cd,(char**)&caData, &dataLen, &outptr, &utf8Len);
	
#endif // #ifndef _MSC_VER
	return dataUtf8.c_str();
}

std::string CbUtility::toANSIIFromUTF8(const char* caData)
{
#ifdef _MSC_VER
		std::wstring dataUnicode;
		dataUnicode.resize(strlen(caData));
		MultiByteToWideChar(CP_UTF8, 0
			, caData, -1
			, &dataUnicode[0], dataUnicode.size()
			);
		std::string dataANSII;
		dataANSII.resize(dataUnicode.size()*2);
		WideCharToMultiByte(CP_ACP,0
			, dataUnicode.c_str(), dataUnicode.size()
			, &dataANSII[0], dataANSII.size(), NULL, NULL);
		return dataANSII.c_str();
#else
		size_t dataLen = strlen(caData);
		std::string dataANSII;
		dataANSII.resize(dataLen * 2); 
		size_t dataANSIISize = dataANSII.size(); 
		iconv_t cd;
		cd = iconv_open("gb2312", "utf-8");
		char* outptr = &dataANSII[0];
		size_t res = iconv(cd,(char**)&caData, &dataLen, &outptr, &dataANSIISize);				
		return dataANSII.c_str(); 
#endif
}

std::string CbUtility::toLower(const char* caData)
{
		if ( caData == NULL)
		{
			return "";
		}
		std::string str = caData;
		transform(str.begin(),str.end(), str.begin(), ::tolower);
		return str;
}

std::string CbUtility::toUpper(const char* caData)
{
		if ( caData == NULL)
		{
			return "";
		}
		std::string str = caData;
		transform(str.begin(),str.end(), str.begin(), ::toupper);
		return str;
}

std::string CbUtility::getDateStringA(void)
{
		char caRtn[Size_MaxStringLen] = {0};
        

		time_t t		=	time(NULL); 
		struct tm * pt	=	localtime(&t);


		snprintf(caRtn, sizeof(caRtn), "%04d-%02d-%02d", pt->tm_year  + 1900,pt->tm_mon + 1,pt->tm_mday);
		return caRtn;;
	}

	std::string CbUtility::getTimeStringA(bool commonFormat)
	{


		char caRtn[Size_MaxStringLen] = {0};
		#ifdef CBWINDOWS
			SYSTEMTIME  st;
			GetLocalTime(&st);
			if (commonFormat)
			{
				snprintf(caRtn
					, sizeof(caRtn)
					, "%04d-%02d-%02d %02d:%02d:%02d:%03d"
					, st.wYear,st.wMonth,st.wDay
					, st.wHour,st.wMinute,st.wSecond,st.wMilliseconds
				);
			}
			else
			{
				snprintf(caRtn
					, sizeof(caRtn)
					, "%04d-%02d-%02d_%02d.%02d.%02d.%03d"
					, st.wYear,st.wMonth,st.wDay
					, st.wHour,st.wMinute,st.wSecond,st.wMilliseconds
				);
			}
			return  caRtn; 
			#else
				time_t t			=	time(NULL); 
				struct tm * pt		 =	 localtime(&t);
				struct timeval tv;
				gettimeofday(&tv, NULL);
				if  (commonFormat)
				{
					snprintf(caRtn, sizeof(caRtn), "%04d-%02d-%02d %02d:%02d:%02d:%03d"
						,pt->tm_year  + 1900,pt->tm_mon + 1,pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec,tv.tv_usec / 1000);
				}
				else
				{
					snprintf(caRtn, sizeof(caRtn), "%04d-%02d-%02d_%02d.%02d.%02d.%03d",pt->tm_year  + 1900,pt->tm_mon + 1,pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec, tv.tv_usec / 1000);
				}
			return caRtn;
			#endif
}

//根据文件名取文件名所在的路径
bool	CbUtility::getPathByFullName(string& strPath, string& strFullName)
{

	std::string file = strFullName;
	strPath = "";
	size_t pos = file.find_last_of("/");
	if (pos != std::string::npos)
	{
		strPath = file.substr(0, pos+1);
		return true; 
	}
	else
	{
		return false; 
	}
}



std::string CbUtility::getExePathA(void)
{
	return  getProgramHome(); 
}

std::string CbUtility::getExeFullNameA(void)
{
	char caRtn[Size_MaxStringLen] = {0};			
	#ifdef CBWINDOWS
	if (caRtn[0] == 0)
	{
		GetModuleFileNameA( NULL, caRtn, sizeof(caRtn) - 1);
		char * end = strrchr(caRtn, '.');
		if (end)
		{
			*end = 0;
		}
	}
	return caRtn;
	#else
		std::string sDir      =  getProgramHome(); 
		std::string sExecName =  getExeNameA(); 
		sDir.append(sExecName); 
		return sDir; 		
	#endif		
}

#ifndef CBWINDOWS
//!根据模板目录获取文件列表文件
StringArr CbUtility::getFileListByTmpFile(std::string&	tmpDir)
{
	std::vector<std::string>	fileVect;
	std::string cmdStr =  formatString("ls -l %s  ", tmpDir.c_str()); 
	FILE * fp = popen(cmdStr.c_str() ,"r");
	if  (fp !=	NULL)
	{
		while(!feof(fp))
		{
			char	buffer[512]	=	{0};
			fscanf(fp, "%s\n", buffer);
			fileVect.push_back(buffer); 
		}
		fclose(fp);
	}
	return fileVect; 
}




//!获取文件的md5码
std::string		 CbUtility::getMD5SumByFile(std::string& fileName)
{
	std::string	md5Str;
	std::string	cmdStr; 
	cmdStr = formatString("md5sum %s  |  awk {'print $1'} ", fileName.c_str()); 
	FILE * fp = popen(cmdStr.c_str() ,"r");
	if  (fp !=	NULL)
	{
		char	buffer[512]	=	{0};
		fscanf(fp,"%s",buffer);
		fclose(fp);
		md5Str	=	buffer; 
	}
	return md5Str; 
}
#else
//!根据模板目录获取文件列表文件
StringArr		 CbUtility::getFileListByTmpFile(std::string&	tmpDir)
{
	std::vector<std::string> vect; 
	return vect; 
}

//!获取文件的md5码
std::string		 CbUtility::getMD5SumByFile(std::string& fileName)
{
	std::string	str; 
	return		str; 	
}
#endif


std::string		CbUtility::generateRandomDigitalString(int32_t nStringCount)
{
	string  str; 
	for (int32_t i = 1;  i <= nStringCount ; i ++)
	{
		int32_t j = rand() % 9 + 1;
		string s = CbUtility::formatString("%d", j); 
		str.append(s);
	}
	return str; 
}
*/





