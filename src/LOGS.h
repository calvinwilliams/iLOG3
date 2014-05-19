#ifndef _H_LOGS_
#define _H_LOGS_

/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 * LastVersion	: v1.0.6
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#ifdef __cplusplus
extern "C" {
#endif

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined __unix ) || ( defined __linux__ )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOG.h"

/* 函数返回值宏 */
#define LOGS_RETURN_ERROR_TOOMANY_LOG	-31	/* 太多日志句柄了 */
#define LOGS_RETURN_ERROR_NOTFOUND	-32	/* 没有该日志句柄 */
#define LOGS_RETURN_INFO_NOTFOUND	32	/* 没有该日志句柄了 */

/* 其它宏 */
#define LOGS_MAXCNT_LOG			10	/* 日志句柄集合中最大日志句柄数量 */

typedef struct tagLOGS LOGS ;

/* 管理日志句柄集合函数 */
_WINDLL_FUNC LOGS *CreateLogsHandle();
_WINDLL_FUNC void DestroyLogsHandle( LOGS *gs );

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
_WINDLL_FUNC LOGS *CreateLogsHandleG();
_WINDLL_FUNC void DestroyLogsHandleG();
#endif

#define LOG_TRAVELLOG_INDEX_INIT	-1

_WINDLL_FUNC int AddLogToLogs( LOGS *gs , char *g_id , LOG *g );
_WINDLL_FUNC LOG *RemoveOutLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC LOG *GetLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC int TravelLogFromLogs( LOGS *gs , long *p_index , char **pp_g_id , LOG **pp_g );

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
_WINDLL_FUNC int AddLogToLogsG( char *g_id , LOG *g );
_WINDLL_FUNC LOG *RemoveOutLogFromLogsG( char *g_id );
_WINDLL_FUNC LOG *GetLogFromLogsG( char *g_id );
_WINDLL_FUNC int TravelLogFromLogsG( long *p_index , char **pp_g_id , LOG **pp_g );
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* 得到基于线程本地存储的缺省日志句柄集合的函数版本 */
_WINDLL_FUNC LOGS *GetGlobalLOGS();
_WINDLL_FUNC void SetGlobalLOGS( LOGS *gs );
#endif

/* 写日志集合函数 */
_WINDLL_FUNC int WriteLogs( LOGS *g , char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int DebugLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int InfoLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WarnLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int ErrorLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int FatalLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );

/* 写日志集合函数的可变参数宏 */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITELOGS( _g_ , _log_level_ , ... )	WriteLogs( _g_ , __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define DEBUGLOGS( _g_ , ... )			DebugLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOGS( _g_ , ... )			InfoLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOGS( _g_ , ... )			WarnLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOGS( _g_ , ... )			ErrorLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define FATALLOGS( _g_ , ... )			FatalLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#endif

/* 写十六进制块日志集合函数 */
_WINDLL_FUNC int WriteHexLogs( LOGS *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int DebugHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int InfoHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WarnHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int ErrorHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int FatalHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
/* 写十六进制块日志集合的可变参数宏 */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITEHEXLOGS( _g_ , _log_level_ , _buf_ , _buf_size_ , ... )	WriteHexLogs( _g_ , __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			DebugHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			InfoHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define WARNHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			WarnHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			ErrorHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define FATALHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			FatalHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* 写日志集合函数（基于线程本地存储的缺省日志句柄的函数集合版本） */
_WINDLL_FUNC int WriteLogsG( char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int DebugLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int InfoLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WarnLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int ErrorLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int FatalLogsG( char *c_filename , long c_fileline , char *format , ... );
/* 写日志集合函数的可变参数宏（基于线程本地存储的缺省日志句柄的函数集合版本） */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITELOGSG( _log_level_ , ... )	WriteLogsG( __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define DEBUGLOGSG( ... )		DebugLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOGSG( ... )			InfoLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOGSG( ... )			WarnLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOGSG( ... )		ErrorLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define FATALLOGSG( ... )		FatalLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#endif
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* 写十六进制块日志集合函数（基于线程本地存储的缺省日志句柄的函数集合版本） */
_WINDLL_FUNC int WriteHexLogsG( char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int DebugHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int InfoHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WarnHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int ErrorHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int FatalHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
/* 写十六进制块日志集合的可变参数宏（基于线程本地存储的缺省日志句柄的函数集合版本） */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITEHEXLOGSG( _log_level_ , _buf_ , _buf_size_ , ... )	WriteHexLogsG( __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOGSG( _buf_ , _buf_size_ , ... )		DebugHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOGSG( _buf_ , _buf_size_ , ... )			InfoHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define WARNHEXLOGSG( _buf_ , _buf_size_ , ... )			WarnHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOGSG( _buf_ , _buf_size_ , ... )		ErrorHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define FATALHEXLOGSG( _buf_ , _buf_size_ , ... )		FatalHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif
