#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#elif defined(__linux__) || defined(__unix)
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include "LOG.h"

typedef struct tagLONGLONG
{
	long high;
	long low;
}
LongToLong;

#if defined(_WIN32)
LongToLong GetUnixSecondsExactly()
{
	SYSTEMTIME	stNow;
	
	time_t		ttNow;
	struct tm	tmNow;
	
	time_t		ttUnix;
	struct tm	tmUnix;
	
	LongToLong	ll;
	
	GetLocalTime( &stNow );
	
	tmNow.tm_year = (int)(stNow.wYear) - 1899 ;
	tmNow.tm_mon = (int)(stNow.wMonth-1) ;
	tmNow.tm_mday = (int)(stNow.wDay) ;
	
	tmNow.tm_hour = (int)(stNow.wHour) ;
	tmNow.tm_min = (int)(stNow.wMinute) ;
	tmNow.tm_sec = (int)(stNow.wSecond) ;
	
	ttNow = mktime( &tmNow ) ;
	
	tmUnix.tm_year = 1970 - 1899 ;
	tmUnix.tm_mon = 0 ;
	tmUnix.tm_mday = 1 ;
	
	tmUnix.tm_hour = 0 ;
	tmUnix.tm_min = 0 ;
	tmUnix.tm_sec = 0 ;
	
	ttUnix = mktime( &tmUnix ) ;
	
	ll.high = (long)difftime( ttNow , ttUnix ) ;
	ll.low = stNow.wMilliseconds * 1000 ;
	
	return ll;
}
#elif defined(__linux__) || defined(__unix)
LongToLong GetUnixSecondsExactly()
{
	struct timeval	tv;
	
	LongToLong		ll;
	
	gettimeofday( &tv , NULL );
	
	ll.high = tv.tv_sec ;
	ll.low = tv.tv_usec ;
	
	return ll;
}
#endif

#define LOG_STYLES_PRESS	( LOG_STYLE_DATETIME | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_press( long count )
{
	LOG		*press = NULL ;
	
	LongToLong	llBegin , llEnd , llDiff ;
	double		dPerf ;
	long		l ;
	
	press = CreateLogHandle() ;
	if( press == NULL )
	{
		printf( "创建press日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	else
	{
		printf( "创建press日志句柄成功\n" );
	}
	
	/* SetLogOptions( press , LOG_OPTION_OPEN_ONCE ); */
	SetLogOutput( press , LOG_OUTPUT_FILE , "test_press.log" , LOG_NO_OUTPUTFUNC );
	SetLogLevel( press , LOG_LEVEL_INFO );
	SetLogStyles( press , LOG_STYLES_PRESS , LOG_NO_STYLEFUNC );
	SetLogRotateMode( press , LOG_ROTATEMODE_SIZE );
	SetLogRotateSize( press , 10*1024*1024 );
	
	llBegin = GetUnixSecondsExactly() ;
	for( l = 1 ; l <= count ; l++ )
	{
		InfoLog( press , __FILE__ , __LINE__ , "log" );
	}
	llEnd = GetUnixSecondsExactly() ;
	llDiff.high = llEnd.high - llBegin.high ;
	llDiff.low = llEnd.low - llBegin.low ;
	if( llDiff.low < 0 )
	{
		llDiff.low += 1000000 ;
		llDiff.high--;
	}
	dPerf = (double)(count) / ( (double)(llDiff.high) + (double)(llDiff.low) / 1000000 ) ;
	printf( "总耗时[%ld.%03ld] 平均每秒输出行日志[%.2lf]条\n" , llDiff.high , llDiff.low , dPerf );
	
	DestroyLogHandle( press );
	printf( "销毁press句柄环境\n" );
	
	return 0;
}

int main( int argc , char *argv[] )
{
	if( argc == 1 + 1 )
	{
		test_press( atol(argv[1]) );
	}
	else
	{
		printf( "USAGE : test_press count\n" );
		return 7;
	}
	
	return 0;
}
