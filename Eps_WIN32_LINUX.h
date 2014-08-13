// Eps_WIN32_LINUX.h: interface for the Eps_WIN32_LINUX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined _EPS_LINUX_H__
#define _EPS_LINUX_H__
//#define _LINUX_
//#define _X64_ //64位操作系统的定义
#define _PSIZE_ sizeof(char*) //指针长度 方便计算偏移地址
#ifdef WIN32 
 #if _MSC_VER > 1000
 #pragma once
 #endif // _MSC_VER > 1000
 #include <windows.h>
 #include <time.h>

//只在windows下使用，linux下使用clib自带的
static size_t  strnlen(const char * ps, size_t n) {
	const char * p = ps;
	while (*p && n) {
		-- n;
		p ++;
	}
	return  p - ps;
}

	#define int8_t						char
	#define uint8_t						unsigned char
	#define int16_t						short
	#define uint16_t						unsigned short
	#define int32_t						__int32
	#define uint32_t					unsigned __int32
	#define int64_t						__int64
	#define uint64_t					unsigned __int64


#else
    #include <time.h>
	#include <cerrno>
	#include <cstring>
	#include <cstdlib>
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <pthread.h>
	#include <fcntl.h>
	#include <string.h>
	#include <curses.h>

	#define CRITICAL_SECTION			pthread_mutex_t 
    #define __int64						long long
	#define __int32						int
	#define __int16						short
	#define TryEnterCriticalSection		pthread_mutex_trylock	
	#define EnterCriticalSection		pthread_mutex_lock
	#define LeaveCriticalSection		pthread_mutex_unlock
	#define DeleteCriticalSection		pthread_mutex_destroy
	#define InitializeCriticalSection	Eps_WIN32_LINUX::InitCri
	#define _atoi64						Eps_WIN32_LINUX::_atoi64_Eps   
    #define _MT
	#define SOCKET						int
	#define	SOCKADDR_IN					sockaddr_in
	#define INVALID_SOCKET				(SOCKET)(~0)
	#define Sleep						Eps_WIN32_LINUX::_Sleep_Eps
	#define closesocket					close
	#define	LPVOID						void*
	#define filelength					Eps_WIN32_LINUX::Eps_filelength 
	#define GetTickCount				Eps_WIN32_LINUX::Eps_GetTickCount
//	#define SLEEPMSECOND 
#endif
//usleep
class Eps_WIN32_LINUX  
{
public:
	static int InitCri(CRITICAL_SECTION* Sec,void* Para =NULL )
	{
#ifdef WIN32
		InitializeCriticalSection(Sec);
#else
		
		return pthread_mutex_init(Sec, NULL);
#endif	
		return 0;
	   
	};
#ifndef WIN32
	static  long long _atoi64_Eps(char *Str)
	{
		return strtoll(Str,NULL,10);
	};
	//毫秒的暂停函数
	static void _Sleep_Eps(int MSecond)
	{
#ifdef SLEEPMSECOND
		usleep(MSecond);
#else
		usleep(MSecond*1000);
#endif
		
	};
	//毫秒的获取函数
	static long Eps_GetTickCount()
	{
		timeval stval;
		gettimeofday(&stval, NULL);
		long lTime = stval.tv_sec*1000l + stval.tv_usec/1000;
		return lTime;
	};

#endif
public:
#ifndef WIN32
	static long Eps_filelength(int handle)
	{
		return lseek(handle,0,SEEK_END);
	};
#else
	static  __int64 _atoi64_Eps(char *Str)
	{
		
		__int64 lRet = 0 ;
		char *p =  Str ;
		while ( *p )
		{
			lRet *= 10 ;
			lRet += (*p-'0');
			p++ ;
		}
		return lRet ;
	};
	
	//毫秒的获取函数
	long Eps_GetTickCount()
	{
		// 		timeval stval;
		// 		gettimeofday(&stval,NULL);
		// 		long lTime = stval.tv_sec*1000l + stval.tv_usec/1000;
		return GetTickCount();
	};
#endif

public:
   Eps_WIN32_LINUX(){};
   virtual ~Eps_WIN32_LINUX(){};

};


// 输入时间的 ms 数量，返回两个时间的天数差
// 如果 $lPastMs > $lNowMs 则返回 0
inline long GetDayDiff(long lPastMs, long lNowMs = 0) {
	Eps_WIN32_LINUX  obj;
	long lTickNow = lNowMs;
	if (0 == lTickNow) {
		lTickNow = obj.Eps_GetTickCount();
	}
	if (lPastMs >= lTickNow) {
		return  0;
	}
	return  (lTickNow - lPastMs) / 86400000; // 一天是 86400s
}

// 输入时间的 ms 数量，返回两个时间的自然天数差，如果两个时间都处于同一日，则为 0 天
// 如果 $lPastMs > $lNowMs 则返回 0
// $lPastMs, $lNowMs 分别是较早的时间和较近的时间的毫秒数
inline long GetNatureDayDiff(long lPastMs, long lNowMs = 0) {
	time_t tmNow  = lNowMs / 1000;
	time_t tmPast = lPastMs / 1000;
	if (0 == tmNow) {
		tmNow = time(NULL);
	}
	if (tmPast >= tmNow) {
		return  0;
	}
	struct tm  stTmNow, stTmPast;
	stTmNow  = *localtime(&tmNow);
	stTmPast = *localtime(&tmPast);
	stTmNow.tm_hour = 0;
	stTmNow.tm_min  = 0;
	stTmNow.tm_sec  = 0;
	tmNow = mktime(&stTmNow);
	stTmPast.tm_hour = 0;
	stTmPast.tm_min  = 0;
	stTmPast.tm_sec  = 0;
	tmPast = mktime(&stTmPast);
	if (tmNow < 0 || tmPast < 0) {
		return  0;
	}
	return  (tmNow - tmPast) / 86400;
}
// 计算两个时间的简单天数差
inline long GetDaySmpDiff(time_t tmPast, time_t tmNow = 0) {
	Eps_WIN32_LINUX  obj;
	time_t tmTick = tmNow;
	if (0 == tmTick) {
		tmTick = time(NULL);
	}
	if (tmPast >= tmTick) {
		return  0;
	}
	return  (tmTick - tmPast) / 86400; // 一天是 86400s
}


#endif 
