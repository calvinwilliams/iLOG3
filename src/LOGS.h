#ifndef _H_LOGS_
#define _H_LOGS_

/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
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
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOG.h"

/* 函数返回值宏 */ /* function returns value macros */
#define LOGS_RETURN_ERROR_TOOMANY_LOG	-31	/* 太多日志句柄了 */ /* too many log handles */
#define LOGS_RETURN_ERROR_NOTFOUND	-32	/* 没有该日志句柄 */ /* log handle not found */
#define LOGS_RETURN_INFO_NOTFOUND	32	/* 没有该日志句柄了 */ /* log handle not found */

/* 其它宏 */ /* other macros */
#define LOGS_MAXCNT_LOG			10	/* 日志句柄集合中最大日志句柄数量 */ /* maximum amount of log handle in one log handle collection */

#define LOGS_MAXLEN_ID			64	/* 日志句柄集合中最长ID */

typedef struct tagLOGS LOGS ;

/* 管理日志句柄集合函数 */ /* log handle collection functions */
_WINDLL_FUNC LOGS *CreateLogsHandle();
_WINDLL_FUNC void DestroyLogsHandle( LOGS *gs );

_WINDLL_FUNC int AddLogToLogs( LOGS *gs , char *g_id , LOG *g );
_WINDLL_FUNC LOG *RemoveOutLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC LOG *GetLogFromLogs( LOGS *gs , char *g_id );
_WINDLL_FUNC int TravelLogFromLogs( LOGS *gs , long *p_index , char **pp_g_id , LOG **pp_g );

/* 写日志集合函数 */ /* output logs */
_WINDLL_FUNC int WriteLevelLogs( LOGS *g , char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int WriteDebugLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteInfoLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteNoticeLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteWarnLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteErrorLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteFatalLogs( LOGS *g , char *c_filename , long c_fileline , char *format , ... );

/* 写十六进制块日志集合函数 */ /* output hex log collection */
_WINDLL_FUNC int WriteLevelHexLogs( LOGS *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteDebugHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteInfoHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteNoticeHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteWarnHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteErrorHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteFatalHexLogs( LOGS *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )

/* 写日志集合函数的可变参数宏 */ /* output log collection macros */
#define WRITELOGS( _g_ , _log_level_ , ... )	WriteLevelLogs( _g_ , __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define DEBUGLOGS( _g_ , ... )			WriteDebugLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOGS( _g_ , ... )			WriteInfoLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define NOTICELOGS( _g_ , ... )			WriteNoticeLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOGS( _g_ , ... )			WriteWarnLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOGS( _g_ , ... )			WriteErrorLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define FATALLOGS( _g_ , ... )			WriteFatalLogs( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );

/* 写十六进制块日志集合的可变参数宏 */ /* output hex log collection macros */
#define WRITEHEXLOGS( _g_ , _log_level_ , _buf_ , _buf_size_ , ... )	WriteLevelHexLogs( _g_ , __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			WriteDebugHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			WriteInfoHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define NOTICEHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			WriteNoticeHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define WARNHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			WriteWarnHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			WriteErrorHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define FATALHEXLOGS( _g_ , _buf_ , _buf_size_ , ... )			WriteFatalHexLogs( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );

#endif

/* 风格替换宏 */
#define create_logs_handle		CreateLogsHandle
#define destroy_logs_handle		DestroyLogsHandle

#define add_log_to_logs			AddLogToLogs
#define remove_out_log_from_logs	RemoveOutLogFromLogs
#define get_log_from_logs		GetLogFromLogs
#define travel_log_from_logs		TravelLogFromLogs

#define write_level_logs		WriteLevelLogs
#define write_debug_logs		WriteDebugLogs
#define write_info_logs			WriteInfoLogs
#define write_notice_logs		WriteNoticeLogs
#define write_warn_logs			WriteWarnLogs
#define write_error_logs		WriteErrorLogs
#define write_fatal_logs		WriteFatalLogs

#define write_level_hex_logs		WriteLevelLogs
#define write_debug_hex_logs		WriteDebugHexLogs
#define write_info_hex_logs		WriteInfoHexLogs
#define write_notice_hex_logs		WriteNoticeHexLogs
#define write_warn_hex_logs		WriteWarnHexLogs
#define write_error_hex_logs		WriteErrorHexLogs
#define write_fatal_hex_logs		WriteFatalHexLogs

#define LOG_TRAVELLOG_INDEX_INIT	-1

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )

_WINDLL_FUNC LOGS *CreateLogsHandleG();
_WINDLL_FUNC void DestroyLogsHandleG();
_WINDLL_FUNC LOGS *GetLogsHandleG();
_WINDLL_FUNC void GetLogsHandlePtrG( LOGS **pp_gs );
_WINDLL_FUNC void SetLogsHandleG( LOGS *gs );

_WINDLL_FUNC int AddLogToLogsG( char *g_id , LOG *g );
_WINDLL_FUNC LOG *RemoveOutLogFromLogsG( char *g_id );
_WINDLL_FUNC LOG *GetLogFromLogsG( char *g_id );
_WINDLL_FUNC int TravelLogFromLogsG( long *p_index , char **pp_g_id , LOG **pp_g );

/* 写日志集合函数（基于线程本地存储的缺省日志句柄的函数集合版本） */ /* output log collection functions for TLS */
_WINDLL_FUNC int WriteLevelLogsG( char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int WriteDebugLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteInfoLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteNoticeLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteWarnLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteErrorLogsG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WriteFatalLogsG( char *c_filename , long c_fileline , char *format , ... );

/* 写十六进制块日志集合函数（基于线程本地存储的缺省日志句柄的函数集合版本） */ /* output hex log collection functions for TLS */
_WINDLL_FUNC int WriteLevelHexLogsG( char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteDebugHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteInfoHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteNoticeHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteWarnHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteErrorHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WriteFatalHexLogsG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )

/* 写日志集合函数的可变参数宏（基于线程本地存储的缺省日志句柄的函数集合版本） */ /* output log collection macros for TLS */
#define WRITELOGSG( _log_level_ , ... )	WriteLevelLogsG( __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define DEBUGLOGSG( ... )		WriteDebugLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOGSG( ... )		WriteInfoLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define NOTICELOGSG( ... )		WriteNoticeLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOGSG( ... )		WriteWarnLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOGSG( ... )		WriteErrorLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define FATALLOGSG( ... )		WriteFatalLogsG( __FILE__ , __LINE__ , __VA_ARGS__ );

/* 写十六进制块日志集合的可变参数宏（基于线程本地存储的缺省日志句柄的函数集合版本） */ /* output hex log collection macros for TLS */
#define WRITEHEXLOGSG( _log_level_ , _buf_ , _buf_size_ , ... )	WriteLevelHexLogsG( __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOGSG( _buf_ , _buf_size_ , ... )		WriteDebugHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOGSG( _buf_ , _buf_size_ , ... )		WriteInfoHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define NOTICEHEXLOGSG( _buf_ , _buf_size_ , ... )		WriteNoticeHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define WARNHEXLOGSG( _buf_ , _buf_size_ , ... )		WriteWarnHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOGSG( _buf_ , _buf_size_ , ... )		WriteErrorHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define FATALHEXLOGSG( _buf_ , _buf_size_ , ... )		WriteFatalHexLogsG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );

#endif

/* 风格替换宏 */
#define create_logs_handle_g		CreateLogsHandleG
#define destroy_logs_handle_g		DestroyLogsHandleG

#define add_log_to_logs_g		AddLogToLogsG
#define remove_out_log_from_logs_g	RemoveOutLogFromLogsG
#define get_log_from_logs_g		GetLogFromLogsG
#define travel_log_from_logs_g		TravelLogFromLogsG

#define write_level_logs_g		WriteLevelLogsG
#define write_debug_logs_g		WriteDebugLogsG
#define write_info_logs_g		WriteInfoLogsG
#define write_notice_logs_g		WriteNoticeLogsG
#define write_warn_logs_g		WriteWarnLogsG
#define write_error_logs_g		WriteErrorLogsG
#define write_fatal_logs_g		WriteFatalLogsG

#define write_hex_logs_g		WriteHexLogsG
#define write_debug_hex_logs_g		WriteDebugHexLogsG
#define write_info_hex_logs_g		WriteInfoHexLogsG
#define write_notice_hex_logs_g		WriteNoticeHexLogsG
#define write_warn_hex_logs_g		WriteWarnHexLogsG
#define write_error_hex_logs_g		WriteErrorHexLogsG
#define write_fatal_hex_logs_g		WriteFatalHexLogsG

/* 得到基于线程本地存储的缺省日志句柄集合的函数版本 */ /* log handle collection get/set function for TLS */
_WINDLL_FUNC LOGS *GetGlobalLOGS();
_WINDLL_FUNC void SetGlobalLOGS( LOGS *gs );
#define get_global_logs		GetGlobalLOGS
#define set_global_logs		SetGlobalLOGS

#endif

#ifdef __cplusplus
}
#endif

#endif
