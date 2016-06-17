#ifndef FRAMEWORK_BASE_CBFORMAT_H_
#define FRAMEWORK_BASE_CBFORMAT_H_

#include "CbCommonInclude.h"

typedef std::vector<std::string> StringArr;


template<class Ty>
std::string toString(Ty value)
{
	std::ostringstream stream;
	stream << value;
	return stream.str();
};

class    CbUtility
{
public:
	
	static std::string			const _PROGRAM_ROOT_PATH;		
	static char					const  hexChars[];					
	static char					const  base64Char[];
	static std::string			const  FILE_SEPARATOR;			
	static size_t				const  Size_MaxStringLen;

	static int64_t				_PROGRAM_START_TIME;			
	static int64_t				m_tPrevLoopTime;				
	static int64_t				m_tCurLoopTime;					
	static char					base64DecodeTable[256];


	static inline int64_t getCurLoopTime()
	{
		return m_tCurLoopTime;
	}
	static inline int64_t getPrevLoopTime()
	{
		return m_tPrevLoopTime;
	}
	static inline int64_t getLoopTimeSpan()
	{
		return m_tCurLoopTime - m_tPrevLoopTime;
	} 
	static inline void setCurLoopTime(int64_t t)
	{
			m_tCurLoopTime = t;
	}
	static inline void setPrevLoopTime(int64_t t)
	{
			m_tPrevLoopTime = t;
	}
	static inline void updateCurLoopTime(int64_t t)
	{
			m_tPrevLoopTime = m_tCurLoopTime;
			m_tCurLoopTime	= t;
	}

    
	//-------------------------------------------------------------------------
private:
	//! �պ���,��ֹ��ǰ�౻ʵ��
	virtual void noop() = 0;

public:

	//��Դ�ַ�����Ŀ��,���ѿ�ͷ�ͽ�β��trimChar��ȥ��.

	/*
	static int32_t strtrim(char  *pDest, char const *pStr, char trimChar)
	{
		assert((NULL != pDest) && (NULL != pStr));

		char const *pStart;
		char const *pEnd;

		pStart = pStr;
		while(*pStart == trimChar)
			pStart++;

		if(*pStart == '\0')
		{
			*pDest = '\0';
			return 0;
		}
		pEnd = pStr + strlen(pStr)-1;
		while(*pEnd == trimChar)
			pEnd--;

		int32_t iLen = pEnd - pStart + 1;

		strncpy(pDest,pStart,iLen);
		pDest[iLen]='\0';		
		return iLen;
	}

	// �ж�һ���ַ��ǲ���ֻ�ǰ�ո��ַ�,���Ŀո��ַ����' ', '\t'
	static bool isWhiteString(char const *pStr)
	{
		assert(NULL != pStr);

		while((*pStr == ' ')|| (*pStr == '\t'))
			pStr ++;
		if(*pStr == '\0')
			return true;
		else
			return false;
	}

	inline static std::string byteArrayToString(char const * pData, int32_t iLen)
	{
		assert(NULL != pData);

		std::string strResult;
		for (int32_t i = 0; i < iLen; i++)
		{
			if(i%30 == 0)
				strResult.append("\n-->");
			strResult.append(1,(int8_t)(hexChars[pData[i]>>4 & 0x0F]));
			strResult.append(1, (int8_t)(hexChars[pData[i] & 0x0F]));
			strResult.append(" ");
		}
		return strResult;
	}

	inline static std::string byteArrayToHex(int8_t const * pData, int32_t iLen)
	{
		assert(NULL != pData);
		std::string strResult;
		for(int32_t i = 0; i < iLen; i++)
		{			
			strResult.append(1,(int8_t)(hexChars[pData[i]>>4 & 0x0F]));
			strResult.append(1,(int8_t)(hexChars[pData[i] & 0x0F]));
		}
		return strResult;
	}
	*/





	//!   ��ȡ����ĸ�·��

	/*
	inline static std::string getRootDir()
	{
		return _PROGRAM_ROOT_PATH;
	}

	//!  ��ȡ����������ļ�����·��
	inline static std::string getConfDir()
	{
		return _PROGRAM_ROOT_PATH + FILE_SEPARATOR + "conf"		+  FILE_SEPARATOR;
	}

	//! ��ȡ�����ִ���ļ�����·��
	inline static std::string getExeDir()
	{
		return _PROGRAM_ROOT_PATH + FILE_SEPARATOR + "bin"		+ FILE_SEPARATOR;
	}

	//! ��ȡ�������ʱ����ļ����·��
	inline static std::string getWorkTmpDir()
	{
		return _PROGRAM_ROOT_PATH + FILE_SEPARATOR + "work_tmp" + FILE_SEPARATOR;
	}

	//! ��ȡ����ĳ־�������ļ����·��
	inline static std::string getDataDir()
	{
		return _PROGRAM_ROOT_PATH + FILE_SEPARATOR + "data"		+ FILE_SEPARATOR;
	}
	//!���ģ��Ŀ¼��ȡ�ļ��б��ļ�
	static  StringArr			getFileListByTmpFile(std::string&	tmpDir); 
	//!��ȡ�ļ���md5��
	static	std::string			getMD5SumByFile(std::string&		fileName);
	*/


#ifdef CBWINDOWS
	inline static int64_t		getRelativeTime()
	{
		return ::timeGetTime() - _PROGRAM_START_TIME;
	}
	inline  static  int64_t	getCurrentTime()
	{
		return (CBUINT32)time(NULL); 
	}
#else
	inline static int64_t getRelativeTime()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return  ( (tv.tv_sec * 1000  +  tv.tv_usec / 1000 ) - _PROGRAM_START_TIME ) ;
	}
	inline  static  int64_t getCurrentTime()
	{
		return  time(NULL); 
	}
#endif
	inline static void reset_start_time()
	{
		_PROGRAM_START_TIME = get_start_time();
	}

#ifdef  CBWINDOWS
	static int64_t  get_start_time()
	{
		return ::timeGetTime();
	}
#else
	static int64_t get_start_time()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (int64_t)(tv.tv_sec) * 1000  +  (int64_t)(tv.tv_usec) / 1000; 
	};
#endif


	inline static std::string byteArrayToString(uint8_t * pData, size_t iLen)
	{
		assert(NULL != pData);

		std::string strResult;
		for (size_t i = 0; i < iLen; i++)
		{
			//if (i % 5000 == 0)
				//strResult.append("\n-->");
			strResult.append(1, (int8_t)(hexChars[pData[i] >> 4 & 0x0F]));
			strResult.append(1, (int8_t)(hexChars[pData[i] & 0x0F]));
			strResult.append(" ");
		}
		return strResult;
	}


public:
	static	std::string		formatString(const char* fmt, ...);
	static	std::string		toUTF8(const char* caData);
	static	std::string		toANSIIFromUTF8(const char* caData);
	static  std::string		ctk_iconv(const char *fromStr, const int fromLen, const char *fromCode, const char *toCode);
	static	StringArr		split(const std::string& strData, const char* caSplit, uint32_t maxGroup = 10);
    static  std::string getLocalIp();


	/*
	 static std::string&	appendANSII(std::string& outStr, const void* data, uint32_t len, const char* caSplit = NULL);
	 static std::string		toANSII(const void* data, uint32_t len, const char* caSplit = NULL);

	 static std::string		toBase64(const void* data, uint32_t len);
	 static std::string		fromBase64(const void* data, uint32_t len);

	 static std::string		toURLEncode(const void* data, uint32_t len, bool isPath = false);
	 static std::string		fromURLEncode(const void* data, uint32_t len);

	 static	std::string		formatString(const char* fmt, ...);

	 static	void			replace(std::string& strSource, const std::string& strReplace, const std::string& strNew);
	 static	std::string		replace(const char* sourceData, const char* replaceData, const char* newData = NULL);

	 static	StringArr		split(const std::string& strData, const char* caSplit, uint32_t maxGroup = 10);

	 static	std::string		toLower(const char* caData);
	 static	std::string		toUpper(const char* caData);
	 static	std::string		toUTF8(const char* caData);
	 static	std::string		toANSIIFromUTF8(const char* caData);

	 static	std::string		getDateStringA(void);
	 static	std::string		getTimeStringA(bool commonFormat = true);

	 static	std::string		getExePathA(void);
	 static	std::string		getExeFullNameA(void);


	 //����ļ���ȡ�ļ������ڵ�·��
	 static	bool			getPathByFullName(string& strPath, string& strFullName); 

	 //����������ַ�
	 static std::string		generateRandomDigitalString(int32_t nStringCount); 






#ifdef  CBWINDOWS
	 static std::string		getProgramHome()
	 {
		 char path[1024] = {0};
		 ::GetModuleFileNameA(NULL, path, sizeof(path) - 1);
		 std::string strHomePath(path);
		 std::string root = strHomePath.substr(0,strHomePath.rfind('\\',
			 strHomePath.rfind('\\', strHomePath.length())-1));
		 _putenv(("program_home=" + root).c_str());
		 return root;
	 }
	 static std::string		getExeNameA(void)
	 {
		 char caRtn[1024] = {0};
		 if (caRtn[0] == 0)
		 {
			 GetModuleFileNameA( NULL, caRtn, sizeof(caRtn) - 1);
			 char* end = strrchr(caRtn, '.');
			 if (end)
			 {
				 *end = 0;
			 }
			 char* begin = strrchr(caRtn, '\\');
			 return begin + 1; 
		 }
		 return caRtn;
	 }	
#else
	 static std::string  getProgramHome()
	 {
		string str; 
		#if defined TARGET_OS_IPHONE ||  defined  TARGET_OS_ANDROID
		 return   str; 
		#endif

		 char *pHomePath = getenv("program_home");
		 if((NULL == pHomePath) || ( 0 == strcmp("",pHomePath)))
		 {
			printf("Error, env var $program_home should be set before run the program.\n");
		 }
		 return std::string(pHomePath);
	 }

	 static std::string getExeNameA(void)
	 {
		 string str; 
		 #if defined TARGET_OS_IPHONE ||  defined  TARGET_OS_ANDROID
		  return   str; 
		 #endif

		 char *pExecName = getenv("exec_name");
		 if((NULL == pExecName) || ( 0 == strcmp("",pExecName)))
		 {
			 printf("Error, env var $exec_name should be set before run the program.\n");
		 }
		 return std::string(pExecName);	
	 }
#endif

private:
	static	void			initBase64DecodeTable();
	*/
};


#endif  // FRAMEWORK_BASE_CBFORMAT_H_
