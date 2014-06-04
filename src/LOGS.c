/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 * LastVersion	: v1.0.8
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined __unix ) || ( defined __linux__ )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOGS.h"

/* 日志句柄集合结构 */ /* log handle collection structure */
struct tagLOGS
{
	char	*g_id[ LOGS_MAXCNT_LOG + 1 ] ;
	LOG	*g[ LOGS_MAXCNT_LOG + 1 ] ;
} ;

/* 线程本地存储全局对象 */ /* TLS */
#if ( defined _WIN32 )
__declspec( thread ) LOGS	*tls_gs = NULL ;
#elif ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
__thread LOGS			*tls_gs = NULL ;
#endif

/* 销毁日志句柄集合 */ /* destruction of logging handle collection */
void DestroyLogsHandle( LOGS *gs )
{
	if( gs )
	{
		long	g_no ;
		
		for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
		{
			if( gs->g_id[g_no] && gs->g[g_no] )
			{
				free( gs->g_id[g_no] );
				
				DestroyLogHandle( gs->g[g_no] );
			}
		}
		
		free( gs );
	}
	
	return;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
void DestroyLogsHandleG()
{
	DestroyLogsHandle( tls_gs );
}
#endif

/* 创建日志句柄集合 */ /* create log handle collection */
LOGS *CreateLogsHandle()
{
	LOGS	*gs = NULL ;
	
	gs = (LOGS *)malloc( sizeof(LOGS) ) ;
	if( gs == NULL )
		return NULL;
	memset( gs , 0x00 , sizeof(LOGS) );
	
	return gs;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
LOGS *CreateLogsHandleG()
{
	tls_gs = CreateLogsHandle() ;
	return tls_gs;
}
#endif

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 ) /* 为了下面函数的strdup能编译不报警告 */
char *strdup(const char *s);
#endif

int AddLogToLogs( LOGS *gs , char *g_id , LOG *g )
{
	long		g_no ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] == NULL && gs->g[g_no] == NULL )
		{
			gs->g_id[g_no] = strdup( g_id ) ;
			if( gs->g_id[g_no] == NULL )
				return LOG_RETURN_ERROR_ALLOC;
			gs->g[g_no] = g ;
			return 0;
		}
	}
	
	return LOGS_RETURN_ERROR_TOOMANY_LOG;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int AddLogToLogsG( char *g_id , LOG *g )
{
	return AddLogToLogs( tls_gs , g_id , g );
}
#endif

LOG *RemoveOutLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	LOG		*g = NULL ;
	
	if( g_id == NULL )
		return NULL;
	
	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] && gs->g[g_no] )
		{
			if( STRCMP( gs->g_id[g_no] , == , g_id ) )
			{
				free( gs->g_id[g_no] ); gs->g_id[g_no] = NULL ;
				g = gs->g[g_no] ;
				gs->g[g_no] = NULL ;
				return g;
			}
		}
	}
	
	return NULL;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
LOG *RemoveOutLogFromLogsG( char *g_id )
{
	return RemoveOutLogFromLogs( tls_gs , g_id );
}
#endif

LOG *GetLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	
	if( g_id == NULL )
		return NULL;
	
	for( g_no = 0 ; g_no < LOGS_MAXCNT_LOG ; g_no++ )
	{
		if( gs->g_id[g_no] && gs->g[g_no] )
		{
			if( STRCMP( gs->g_id[g_no] , == , g_id ) )
			{
				return gs->g[g_no];
			}
		}
	}
	
	return NULL;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
LOG *GetLogFromLogsG( char *g_id )
{
	return GetLogFromLogs( tls_gs , g_id );
}
#endif

int TravelLogFromLogs( LOGS *gs , long *p_index , char **pp_g_id , LOG **pp_g )
{
	for( ++(*p_index) ; (*p_index) >= LOGS_MAXCNT_LOG ; (*p_index)++ )
	{
		if( gs->g_id[(*p_index)] && gs->g[(*p_index)] )
		{
			if( pp_g_id )
				(*pp_g_id) = gs->g_id[(*p_index)] ;
			if( pp_g )
				(*pp_g) = gs->g[(*p_index)] ;
			return 0;
		}
	}
	
	return LOGS_RETURN_INFO_NOTFOUND;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int TravelLogFromLogsG( long *p_index , char **pp_id , LOG **pp_g )
{
	return TravelLogFromLogs( tls_gs , p_index , pp_id , pp_g );
}
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* 得到基于线程本地存储的缺省日志句柄集合的函数版本 */
LOGS *GetGlobalLOGS()
{
	return tls_gs;
}
void SetGlobalLOGS( LOGS *gs )
{
	tls_gs = gs ;
	return;
}
#endif

extern int WriteLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist );

/* 代码宏 */ /* code macro */
#define WRITELOGSBASE(_gs_,_log_level_) \
	long		g_no ; \
	LOG		**pp_g = NULL ; \
	va_list		valist; \
	int		nret ; \
	if( (_gs_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	for( g_no = 0 , pp_g = & ((_gs_)->g[0]) ; g_no < LOGS_MAXCNT_LOG ; g_no++ , pp_g++ ) \
	{ \
		if( (*pp_g) == NULL ) \
			continue ; \
		if( (*pp_g)->output == LOG_OUTPUT_FILE && (*pp_g)->log_pathfilename[0] == '\0' ) \
			continue ; \
		if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (*pp_g) ) ) \
			continue ; \
		va_start( valist , format ); \
		nret = WriteLogBase( (*pp_g) , c_filename , c_fileline , _log_level_ , format , valist ) ; \
		va_end( valist ); \
		if( nret < 0 ) \
			return nret; \
	}

/* 带日志等级的写日志 */ /* write log handle collection */
int WriteLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGSBASE( gs , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WriteLogsG( char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGSBASE( tls_gs , log_level )
	return 0;
}

/* 写调试日志 */ /* write debug log handle collection */
int DebugLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int DebugLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* 写普通信息日志 */ /* write info log handle collection */
int InfoLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int InfoLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* 写警告日志 */ /* write warn log handle collection */
int WarnLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WarnLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* 写错误日志 */ /* write error log handle collection */
int ErrorLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int ErrorLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* 写致命错误日志 */ /* write fatal log handle collection */
int FatalLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int FatalLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_FATAL )
	return 0;
}
#endif

extern int WriteHexLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist );

/* 代码宏 */ /* code macro */
#define WRITEHEXLOGSBASE(_gs_,_log_level_) \
	long		g_no ; \
	LOG		**pp_g = NULL ; \
	va_list		valist; \
	int		nret ; \
	if( (_gs_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	for( g_no = 0 , pp_g = & ((_gs_)->g[0]) ; g_no < LOGS_MAXCNT_LOG ; g_no++ , pp_g++ ) \
	{ \
		if( (*pp_g) == NULL ) \
			continue ; \
		if( (*pp_g)->output == LOG_OUTPUT_FILE && (*pp_g)->log_pathfilename[0] == '\0' ) \
			continue ; \
		if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (*pp_g) ) ) \
			continue ; \
		va_start( valist , format ); \
		nret = WriteHexLogBase( (*pp_g) , c_filename , c_fileline , _log_level_ , buffer , buflen , format , valist ) ; \
		va_end( valist ); \
		if( nret ) \
			return nret; \
	}

/* 带日志等级的写十六进制块日志 */ /* write hex log handle collection */
int WriteHexLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WriteHexLogsG( char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , log_level )
	return 0;
}
#endif

/* 写十六进制块调试日志 */ /* write debug hex log handle collection */
int DebugHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int DebugHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* 写十六进制块普通信息日志 */ /* write info hex log handle collection */
int InfoHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int InfoHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* 写十六进制块警告日志 */ /* write warn hex log handle collection */
int WarnHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int WarnHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* 写十六进制块错误日志 */ /* write error hex log handle collection */
int ErrorHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int ErrorHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* 写十六进制块致命错误日志 */ /* write fatal hex log handle collection */
int FatalHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
int FatalHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_FATAL )
	return 0;
}
#endif

#endif

