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
#include <pthread.h>
#include <sys/wait.h>
#endif

static int _WriteLogBase( char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	int		fd ;
	
	char		buf[ 1024 + 1 ] ;
	long		len ;
	time_t		tt ;
	struct tm	stime ;
	pid_t		pid ;
	unsigned long	tid ;		
	
	int		n ;
	
	fd = open( "bench_tiny.log" , O_CREAT|O_APPEND|O_WRONLY , S_IRWXU|S_IRWXG|S_IRWXO ) ;
	if( fd == -1 )
		return -1;
	
	time( & tt );
	localtime_r( & tt , & stime );
	pid = getpid() ;
	tid = (unsigned long)pthread_self() ;
	memset( buf , 0x00 , sizeof(buf) );
	len = snprintf( buf , sizeof(buf) , "%04d-%02d-%02d %02d:%02d:%02d | INFO  | %d:%lu:%s:%ld | %s\n" , stime.tm_year+1900 , stime.tm_mon+1 , stime.tm_mday , stime.tm_hour , stime.tm_min , stime.tm_sec , pid , tid , c_filename , c_fileline , "log" ) ;
	
	n = write( fd , buf , len ) ;
	if( n == -1 )
		return -1;
	
	close( fd );
	
	return 0;
}

#define LOG_LEVEL_INFO		1

static int InfoLog( char *c_filename , long c_fileline , char *format , ... )
{
	va_list		valist ;
	
	va_start( valist , format );
	_WriteLogBase( c_filename , c_fileline , LOG_LEVEL_INFO , format , valist );
	va_end( valist );
	
	return 0;
}


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

#define LOG_STYLES_PRESS	( LOG_STYLE_DATETIME | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_press( long count )
{
	LongToLong	llBegin , llEnd , llDiff ;
	double		dPerf ;
	long		l ;
	
	llBegin = GetUnixSecondsExactly() ;
	for( l = 1 ; l <= count ; l++ )
	{
		InfoLog( __FILE__ , __LINE__ , "log" );
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
	/* printf( "总耗时[%ld.%03ld] 平均每秒输出行日志[%.2lf]条\n" , llDiff.high , llDiff.low , dPerf ); */
	
	return 0;
}

#if defined(_WIN32)

DWORD WINAPI _test_press( void *p )
{
	long		count = *((long*)p) ;
	
	return test_press( count );
}

int test_press_mpt( long processes , long threads , long count )
{
	long		lThreadNo ;
	HANDLE		*ahdThread = NULL ;
	DWORD		dwThreadId ;
	
	ahdThread = (HANDLE*)malloc( sizeof(HANDLE) * count ) ;
	if( ahdThread == NULL )
	{
		printf( "alloc failed\n" );
		return -1;
	}
	memset( ahdThread , 0x00 , sizeof(HANDLE) * count );
	
	for( lThreadNo = 0 ; lThreadNo < threads ; lThreadNo++ )
	{
		ahdThread[lThreadNo] = CreateThread( NULL , 0 , _test_press , & count , 0 , & dwThreadId ) ;
		if( ahdThread[lThreadNo] == NULL )
		{
			printf( "CreateThread failed\n" );
			free(ahdThread);
			return -1;
		}
	}
	
	for( lThreadNo = 0 ; lThreadNo < threads ; lThreadNo++ )
	{
		WaitForSingleObject( ahdThread[lThreadNo] , INFINITE );
	}
	
	free(ahdThread);
	
	return 0;
}

#elif defined(__linux__) || defined(__unix)

void *_test_press( void *p )
{
	long		count = *((long*)p) ;
	
	test_press( count );
	
	return NULL;
}

int test_press_mt( long threads , long count )
{
	long		lThreadNo ;
	pthread_t	*ahdThread = NULL ;
	
	int		nret ;
	
	ahdThread = (pthread_t*)malloc( sizeof(pthread_t) * count ) ;
	if( ahdThread == NULL )
	{
		printf( "alloc failed\n" );
		return -1;
	}
	memset( ahdThread , 0x00 , sizeof(pthread_t) * count );
	
	for( lThreadNo = 0 ; lThreadNo < threads ; lThreadNo++ )
	{
		nret = pthread_create( & (ahdThread[lThreadNo]) , NULL , & _test_press , & count ) ;
		if( nret )
		{
			printf( "CreateThread failed\n" );
			free(ahdThread);
			return -1;
		}
	}
	
	for( lThreadNo = 0 ; lThreadNo < threads ; lThreadNo++ )
	{
		pthread_join( ahdThread[lThreadNo] , NULL );
	}
	
	free(ahdThread);
	
	return 0;
}

int test_press_mpt( long processes , long threads , long count )
{
	long		lProcessNo ;
	pid_t		pid ;
	
	for( lProcessNo = 0 ; lProcessNo < processes ; lProcessNo++ )
	{
		pid = fork() ;
		if( pid < 0 )
		{
			printf( "fork failed\n" );
			return -1;
		}
		else if( pid == 0 )
		{
			test_press_mt( threads , count );
			exit(0);
		}
	}
	
	for( lProcessNo = 0 ; lProcessNo < processes ; lProcessNo++ )
	{
		wait( NULL );
	}
	
	return 0;
}

#endif

int main( int argc , char *argv[] )
{
	if( argc == 1 + 3 )
	{
		test_press_mpt( atol(argv[1]) , atol(argv[2]) , atol(argv[3]) );
	}
	else
	{
		printf( "USAGE : test_press_mpt processes threads count\n" );
		return 7;
	}
	
	return 0;
}
