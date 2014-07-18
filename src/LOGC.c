#include "LOGC.h"

/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 * LastVersion	: v1.0.9
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

/* 日志文件名 */
TLS char	g_log_pathfilename[ MAXLEN_FILENAME + 1 ] = "" ;
TLS int		g_log_level = LOGLEVEL_INFO ;

const char log_level_itoa[][6] = { "DEBUG" , "INFO" , "WARN" , "ERROR" , "FATAL" } ;

/* 设置日志文件名 */
void SetLogFile( char *format , ... )
{
	va_list		valist ;
	
	va_start( valist , format );
	VSNPRINTF( g_log_pathfilename , sizeof(g_log_pathfilename)-1 , format , valist );
	va_end( valist );
	
	return;
}

/* 设置日志等级 */
void SetLogLevel( int log_level )
{
	g_log_level = log_level ;
	
	return;
}

/* 输出日志 */
int OutputLog( int log_level , char *c_filename , long c_fileline , char *format , va_list valist )
{
	char		c_filename_copy[ MAXLEN_FILENAME + 1 ] ;
	char		*p_c_filename = NULL ;
	
	struct timeval	tv ;
	struct tm	stime ;
	size_t		len ;
	
	char		log_buffer[ 4096 + 1 ] ;
	
	memset( c_filename_copy , 0x00 , sizeof(c_filename_copy) );
	strncpy( c_filename_copy , c_filename , sizeof(c_filename_copy)-1 );
	p_c_filename = strrchr( c_filename_copy , '\\' ) ;
	if( p_c_filename )
		p_c_filename++;
	else
		p_c_filename = c_filename_copy ;

#if ( defined __linux__ ) || ( defined __unix )
	gettimeofday( & tv , NULL );
	localtime_r( &(tv.tv_sec) , & stime );
#elif ( defined _WIN32 )
	{
	SYSTEMTIME	stNow ;
	GetLocalTime( & stNow );
	tv.tv_usec = stNow.wMilliseconds * 1000 ;
	stime.tm_year = stNow.wYear - 1900 ;
	stime.tm_mon = stNow.wMonth - 1 ;
	stime.tm_mday = stNow.wDay ;
	stime.tm_hour = stNow.wHour ;
	stime.tm_min = stNow.wMinute ;
	stime.tm_sec = stNow.wSecond ;
	}
#endif

	memset( log_buffer , 0x00 , sizeof(log_buffer) );
	len = strftime( log_buffer , sizeof(log_buffer) , "%Y-%m-%d %H:%M:%S" , & stime ) ;
	len += SNPRINTF( log_buffer+len , sizeof(log_buffer)-1-len , ".%06ld" , (long)(tv.tv_usec) ) ;
	len += SNPRINTF( log_buffer+len , sizeof(log_buffer)-1-len , " | %-5s" , log_level_itoa[log_level] ) ;
	len += SNPRINTF( log_buffer+len , sizeof(log_buffer)-1-len , " | %ld:%ld:%s:%ld | ", PROCESSID , THREADID , p_c_filename , c_fileline ) ;
	len += VSNPRINTF( log_buffer+len , sizeof(log_buffer)-1-len , format , valist );
	
	if( g_log_pathfilename[0] == '\0' )
	{
		WRITE( 1 , log_buffer , len );
	}
	else
	{
		int		fd ;
		
#if ( defined __linux__ ) || ( defined __unix )
		fd = OPEN( g_log_pathfilename , O_CREAT | O_WRONLY | O_APPEND , S_IRWXU | S_IRWXG | S_IRWXO ) ;
#elif ( defined _WIN32 )
		fd = OPEN( g_log_pathfilename , _O_CREAT | _O_WRONLY | _O_APPEND , _S_IREAD | _S_IWRITE ) ;
#endif
		if( fd == -1 )
			return -1;
		
		WRITE( fd , log_buffer , len );
		
		CLOSE( fd );
	}
	
	return 0;
}

int FatalLog( char *c_filename , long c_fileline , char *format , ... )
{
	va_list		valist ;
	
	if( LOGLEVEL_FATAL < g_log_level )
		return 0;
	
	va_start( valist , format );
	OutputLog( LOGLEVEL_FATAL , c_filename , c_fileline , format , valist );
	va_end( valist );
	
	return 0;
}

int ErrorLog( char *c_filename , long c_fileline , char *format , ... )
{
	va_list		valist ;
	
	if( LOGLEVEL_ERROR < g_log_level )
		return 0;
	
	va_start( valist , format );
	OutputLog( LOGLEVEL_ERROR , c_filename , c_fileline , format , valist );
	va_end( valist );
	
	return 0;
}

int WarnLog( char *c_filename , long c_fileline , char *format , ... )
{
	va_list		valist ;
	
	if( LOGLEVEL_WARN < g_log_level )
		return 0;
	
	va_start( valist , format );
	OutputLog( LOGLEVEL_WARN , c_filename , c_fileline , format , valist );
	va_end( valist );
	
	return 0;
}

int InfoLog( char *c_filename , long c_fileline , char *format , ... )
{
	va_list		valist ;
	
	if( LOGLEVEL_INFO < g_log_level )
		return 0;
	
	va_start( valist , format );
	OutputLog( LOGLEVEL_INFO , c_filename , c_fileline , format , valist );
	va_end( valist );
	
	return 0;
}

int DebugLog( char *c_filename , long c_fileline , char *format , ... )
{
	va_list		valist ;
	
	if( LOGLEVEL_DEBUG < g_log_level )
		return 0;
	
	va_start( valist , format );
	OutputLog( LOGLEVEL_DEBUG , c_filename , c_fileline , format , valist );
	va_end( valist );
	
	return 0;
}

