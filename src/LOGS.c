/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
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

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
void DestroyLogsHandleG()
{
	DestroyLogsHandle( tls_gs ); tls_gs = NULL ;
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

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOGS *CreateLogsHandleG()
{
	tls_gs = CreateLogsHandle() ;
	return tls_gs;
}
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOGS *GetLogsHandleG()
{
	return tls_gs;
}

void GetLogsHandlePtrG( LOGS **pp_gs )
{
	pp_gs = & tls_gs ;
	return;
}

void SetLogsHandleG( LOGS *gs )
{
	tls_gs = gs ;
	return;
}
#endif

int AddLogToLogs( LOGS *gs , char *g_id , LOG *g )
{
	long		g_no ;
	
	if( gs == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
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

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int AddLogToLogsG( char *g_id , LOG *g )
{
	return AddLogToLogs( tls_gs , g_id , g );
}
#endif

LOG *RemoveOutLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	LOG		*g = NULL ;
	
	if( gs == NULL )
		return NULL;
	
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

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOG *RemoveOutLogFromLogsG( char *g_id )
{
	return RemoveOutLogFromLogs( tls_gs , g_id );
}
#endif

LOG *GetLogFromLogs( LOGS *gs , char *g_id )
{
	long		g_no ;
	
	if( gs == NULL )
		return NULL;
	
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

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOG *GetLogFromLogsG( char *g_id )
{
	return GetLogFromLogs( tls_gs , g_id );
}
#endif

int TravelLogFromLogs( LOGS *gs , long *p_index , char **pp_g_id , LOG **pp_g )
{
	if( gs == NULL )
		return LOGS_RETURN_INFO_NOTFOUND;
	
	for( ++(*p_index) ; (*p_index) < LOGS_MAXCNT_LOG ; (*p_index)++ )
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

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int TravelLogFromLogsG( long *p_index , char **pp_id , LOG **pp_g )
{
	return TravelLogFromLogs( tls_gs , p_index , pp_id , pp_g );
}
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
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
int WriteLevelLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGSBASE( gs , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteLevelLogsG( char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGSBASE( tls_gs , log_level )
	return 0;
}

/* 写调试日志 */ /* write debug log handle collection */
int WriteDebugLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteDebugLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* 写普通信息日志 */ /* write info log handle collection */
int WriteInfoLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteInfoLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* 写通知信息日志 */ /* write notice log handle collection */
int WriteNoticeLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_NOTICE )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteNoticeLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_NOTICE )
	return 0;
}
#endif

/* 写警告日志 */ /* write warn log handle collection */
int WriteWarnLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteWarnLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* 写错误日志 */ /* write error log handle collection */
int WriteErrorLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteErrorLogsG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( tls_gs , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* 写致命错误日志 */ /* write fatal log handle collection */
int WriteFatalLogs( LOGS *gs , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGSBASE( gs , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteFatalLogsG( char *c_filename , long c_fileline , char *format , ... )
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
int WriteLevelHexLogs( LOGS *gs , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteLevelHexLogsG( char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , log_level )
	return 0;
}
#endif

/* 写十六进制块调试日志 */ /* write debug hex log handle collection */
int WriteDebugHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteDebugHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* 写十六进制块普通信息日志 */ /* write info hex log handle collection */
int WriteInfoHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteInfoHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* 写十六进制块通知信息日志 */ /* write notice hex log handle collection */
int WriteNoticeHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_NOTICE )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteNoticeHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_NOTICE )
	return 0;
}
#endif

/* 写十六进制块警告日志 */ /* write warn hex log handle collection */
int WriteWarnHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteWarnHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* 写十六进制块错误日志 */ /* write error hex log handle collection */
int WriteErrorHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteErrorHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* 写十六进制块致命错误日志 */ /* write fatal hex log handle collection */
int WriteFatalHexLogs( LOGS *gs , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( gs , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteFatalHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGSBASE( tls_gs , LOG_LEVEL_FATAL )
	return 0;
}
#endif

#endif

