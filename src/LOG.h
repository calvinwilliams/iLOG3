#ifndef _H_LOG_
#define _H_LOG_

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#if ( defined _WIN32 )
#include <windows.h>
#include <share.h>
#include <io.h>
#include <fcntl.h>
#elif ( defined __unix ) || ( defined __linux__ )
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <syslog.h>
#include <pthread.h>
#endif

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
struct tm *localtime_r(const time_t *timep, struct tm *result);
#endif

/* 函数返回值宏 */
#define LOG_RETURN_ERROR_ALLOC		-11	/* 申请内存失败 */
#define LOG_RETURN_ERROR_INTERNAL	-12	/* 内部错误 */
#define LOG_RETURN_ERROR_ALLOC_MAX	-13	/* 内存使用超限 */
#define LOG_RETURN_ERROR_PARAMETER	-14	/* 参数错误 */
#define LOG_RETURN_ERROR_NOTSUPPORT	-17	/* 暂不支持 */
#define LOG_RETURN_ERROR_CREATEFILE	-21	/* 创建文件失败 */
#define LOG_RETURN_ERROR_OPENFILE	-22	/* 打开文件失败 */
#define LOG_RETURN_ERROR_WRITEFILE	-23	/* 写文件失败 */

/********************************************************/
/*                    以下为基本特性                    */
/********************************************************/

/* 日志输出类型 */
#define LOG_OUTPUT_NOSET	-1	/* 不设置 */
#define LOG_OUTPUT_STDOUT	1	/* 标准输出 */
#define LOG_OUTPUT_STDERR	2	/* 标准错误输出 */
#define LOG_OUTPUT_SYSLOG	3	/* UNIX&Linux的syslog 或 Windows的WINDOWS EVENT */
#define LOG_OUTPUT_FILE		11	/* 文件 */
#define LOG_OUTPUT_CALLBACK	21	/* （使用自定义日志输出回调函数） */

/* 日志等级宏 */
#define LOG_LEVEL_DEBUG		0	/* 调试等级 */
#define LOG_LEVEL_INFO		1	/* 普通信息等级 */
#define LOG_LEVEL_WARN		2	/* 警告等级 */
#define LOG_LEVEL_ERROR		3	/* 错误等级 */
#define LOG_LEVEL_FATAL		4	/* 严重错误等级 */
#define LOG_LEVEL_NOLOG		5	/* 不需要输出日志 */

#define LOG_LEVEL_DEFAULT	LOG_LEVEL_NOLOG

/* 行日志风格组合宏 */
#define LOG_STYLE_DATE		1	/* 日期"YYYY-MM-DD" */
#define LOG_STYLE_DATETIME	2	/* 日期时间"YYYY-MM-DD hh:mm:ss" */
#define LOG_STYLE_DATETIMEMS	4	/* 日期时间毫秒"YYYY-MM-DD hh:mm:ss.6ms"（日期时间类宏互斥，优先自动选用信息量最多的） */
#define LOG_STYLE_LOGLEVEL	8	/* 日志等级 */
#define LOG_STYLE_PID		16	/* 进程id */
#define LOG_STYLE_TID		32	/* 线程id */
#define LOG_STYLE_SOURCE	64	/* "源代码文件名:源代码行号" */
#define LOG_STYLE_FORMAT	128	/* 应用日志段 */
#define LOG_STYLE_NEWLINE	256	/* 换行 */
#define LOG_STYLE_CUSTLABEL1	512	/* 自定义标签1 */
#define LOG_STYLE_CUSTLABEL2	1024	/* 自定义标签2 */
#define LOG_STYLE_CUSTLABEL3	2048	/* 自定义标签3 */
#define LOG_STYLE_CALLBACK	4096	/* （使用自定义行日志风格回调函数） */

#define LOG_STYLES_DEFAULT	0

/* 其它宏 */
#ifndef MAXLEN_FILENAME
#define MAXLEN_FILENAME		256
#endif

typedef struct tagLOG LOG ;
typedef struct tagLOGBUF LOGBUF ;

/* 管理日志句柄函数 */
_WINDLL_FUNC LOG *CreateLogHandle();
_WINDLL_FUNC void DestroyLogHandle( LOG *g );

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
_WINDLL_FUNC LOG *CreateLogHandleG();
_WINDLL_FUNC void DestroyLogHandleG();
#endif

/* 自定义打开、输出、关闭日志函数类型 */
typedef int funcOpenLog( LOG *g , char *log_pathfilename , void **open_handle );
typedef int funcWriteLog( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen );
typedef int funcChangeTest( LOG *g , void **test_handle );
typedef int funcCloseLog( LOG *g , void **open_handle );

#define LOG_NO_OUTPUTFUNC	NULL , NULL , NULL , NULL , NULL , NULL

/* 自定义行日志风格函数类型 */
typedef int funcLogStyle( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist );

#define LOG_NO_STYLEFUNC	NULL

/* 句柄环境设置函数 */
_WINDLL_FUNC int SetLogOutput( LOG *g , int output , char *log_pathfilename , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally );
_WINDLL_FUNC int SetLogLevel( LOG *g , int log_level );
_WINDLL_FUNC int SetLogStyles( LOG *g , long log_styles , funcLogStyle *pfuncLogStyle );

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
_WINDLL_FUNC int SetLogOutputG( int output , char *log_pathfilename , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally );
_WINDLL_FUNC int SetLogLevelG( int log_level );
_WINDLL_FUNC int SetLogStylesG( long log_styles , funcLogStyle *pfuncLogStyles );
#endif

/* 写日志函数 */
_WINDLL_FUNC int WriteLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int DebugLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int InfoLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WarnLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int ErrorLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int FatalLog( LOG *g , char *c_filename , long c_fileline , char *format , ... );
/*
使用示例
InfoLog( g , __FILE__ , __LINE__ , "xxx处理完成，结果码[%d]" , nret );
*/

/* 写日志函数的可变参数宏 */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITELOG( _g_ , _log_level_ , ... )	WriteLog( _g_ , __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define DEBUGLOG( _g_ , ... )			DebugLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOG( _g_ , ... )			InfoLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOG( _g_ , ... )			WarnLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOG( _g_ , ... )			ErrorLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
#define FATALLOG( _g_ , ... )			FatalLog( _g_ , __FILE__ , __LINE__ , __VA_ARGS__ );
/*
使用示例
INFOLOG( g , "xxx处理完成，结果码[%d]" , nret );
*/
#endif

/* 写十六进制块日志函数 */
_WINDLL_FUNC int WriteHexLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int DebugHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int InfoHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WarnHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int ErrorHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int FatalHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
/* 写十六进制块日志的可变参数宏 */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITEHEXLOG( _g_ , _log_level_ , _buf_ , _buf_size_ , ... )	WriteHexLog( _g_ , __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			DebugHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			InfoHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define WARNHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			WarnHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			ErrorHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define FATALHEXLOG( _g_ , _buf_ , _buf_size_ , ... )			FatalHexLog( _g_ , __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* 写日志函数（基于线程本地存储的缺省日志句柄的函数集合版本） */
_WINDLL_FUNC int WriteLogG( char *c_filename , long c_fileline , int log_level , char *format , ... );
_WINDLL_FUNC int DebugLogG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int InfoLogG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int WarnLogG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int ErrorLogG( char *c_filename , long c_fileline , char *format , ... );
_WINDLL_FUNC int FatalLogG( char *c_filename , long c_fileline , char *format , ... );
/*
使用示例
InfoLogG( __FILE__ , __LINE__ , "xxx处理完成，结果码[%d]" , nret );
*/
/* 写日志函数的可变参数宏（基于线程本地存储的缺省日志句柄的函数集合版本） */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITELOGG( _log_level_ , ... )	WriteLogG( __FILE__ , __LINE__ , _log_level_ , __VA_ARGS__ );
#define DEBUGLOGG( ... )		DebugLogG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOGG( ... )			InfoLogG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOGG( ... )			WarnLogG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOGG( ... )		ErrorLogG( __FILE__ , __LINE__ , __VA_ARGS__ );
#define FATALLOGG( ... )		FatalLogG( __FILE__ , __LINE__ , __VA_ARGS__ );
/*
使用示例
INFOLOGG( "xxx处理完成，结果码[%d]" , nret );
*/
#endif
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* 写十六进制块日志函数（基于线程本地存储的缺省日志句柄的函数集合版本） */
_WINDLL_FUNC int WriteHexLogG( char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int DebugHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int InfoHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int WarnHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int ErrorHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
_WINDLL_FUNC int FatalHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... );
/* 写十六进制块日志的可变参数宏（基于线程本地存储的缺省日志句柄的函数集合版本） */
#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )
#define WRITEHEXLOGG( _log_level_ , _buf_ , _buf_size_ , ... )	WriteHexLogG( __FILE__ , __LINE__ , _log_level_ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define DEBUGHEXLOGG( _buf_ , _buf_size_ , ... )		DebugHexLogG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define INFOHEXLOGG( _buf_ , _buf_size_ , ... )			InfoHexLogG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define WARNHEXLOGG( _buf_ , _buf_size_ , ... )			WarnHexLogG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define ERRORHEXLOGG( _buf_ , _buf_size_ , ... )		ErrorHexLogG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#define FATALHEXLOGG( _buf_ , _buf_size_ , ... )		FatalHexLogG( __FILE__ , __LINE__ , _buf_ , _buf_size_ , __VA_ARGS__ );
#endif
#endif

/********************************************************/
/*                    以下为高级特性                    */
/********************************************************/

/* 自定义检查日志等级回调函数类型 */
typedef int funcFilterLog( LOG *g , void **open_handle , int log_level , char *buf , long len );

/* 自定义标签数量 */
#define LOG_MAXCNT_CUST_LABEL		3
/* 自定义标签最大长度 */
#define LOG_MAXLEN_CUST_LABEL		32

/* 日志选项 */
#define LOG_OPTION_OPEN_AND_CLOSE		1	/* 每次都打开日志、写日志、关闭日志 */
#define LOG_OPTION_CHANGE_TEST			2	/* 侦测文件变动 */
#define LOG_OPTION_OPEN_ONCE			4	/* 日志打开一次不关闭 */
#define LOG_OPTION_SET_OUTPUT_BY_FILENAME	8	/* 自动根据文件名重置输出类型 */
							/* "#stdout" -> LOG_OUTPUT_STDOUT */
							/* "#stderr" -> LOG_OUTPUT_STDERR */
							/* "#syslog" -> LOG_OUTPUT_SYSLOG */
#define LOG_OPTION_FILENAME_APPEND_DOT_LOG	16	/* 日志输出文件名后自动加上".log" */

#define LOG_OPTION_OPEN_DEFAULT			LOG_OPTION_CHANGE_TEST

/* 日志转档模式 */
#define LOG_ROTATEMODE_NONE		0	/* 不转档 */
#define LOG_ROTATEMODE_SIZE		1	/* 按日志文件大小转档，和函数SetLogRotateSize配合使用；转档文件名格式"原日志文件名.序号" */
#define LOG_ROTATEMODE_PER_DAY		2	/* 按每天转档；转档文件名格式"原日志文件名.年年年年月月日日" */
#define LOG_ROTATEMODE_PER_HOUR		3	/* 按小时转档；转档文件名格式"原日志文件名.年年年年月月日日_小时" */

/* 日志转档缺省值 */
#define LOG_ROTATE_SIZE_FILE_COUNT_DEFAULT		99999999
#define LOG_ROTATE_SIZE_PRESSURE_FACTOR_DEFAULT		2

/* 自定义日志转档前后回调函数类型 */
typedef int funcBeforeRotateFile( LOG *g , char *rotate_log_pathfilename );
typedef int funcAfterRotateFile( LOG *g , char *rotate_log_pathfilename );

/* 缓冲区大小缺省值 */
#define LOG_BUFSIZE_DEFAULT		(1024)		/* 缺省行日志缓冲区大小 */
#define LOG_BUFSIZE_MAX			(16*1024)	/* 最大行日志缓冲区大小 */
#define LOG_HEXLOG_BUFSIZE_DEFAULT	(1024*1024)	/* 缺省十六进制块日志缓冲区大小 */
#define LOG_HEXLOG_BUFSIZE_MAX		(16*1024*1024)	/* 最大十六进制块日志缓冲区大小 */

/* 高级句柄环境设置函数 */
_WINDLL_FUNC int SetLogOptions( LOG *g , int log_options );
_WINDLL_FUNC int SetLogFileChangeTest( LOG *g , long interval );
_WINDLL_FUNC int SetLogCustLabel( LOG *g , int index , char *cust_label );
_WINDLL_FUNC int SetLogRotateMode( LOG *g , int rotate_mode );
_WINDLL_FUNC int SetLogRotateSize( LOG *g , long log_rotate_size );
_WINDLL_FUNC int SetLogRotatePressureFactor( LOG *g , long pressure_factor );
_WINDLL_FUNC int SetBeforeRotateFileFunc( LOG *g , funcAfterRotateFile *pfuncAfterRotateFile );
_WINDLL_FUNC int SetAfterRotateFileFunc( LOG *g , funcAfterRotateFile *pfuncAfterRotateFile );
_WINDLL_FUNC int SetFilterLogFunc( LOG *g , funcFilterLog *pfuncFilterLog );
_WINDLL_FUNC int SetLogBufferSize( LOG *g , long log_bufsize , long max_log_bufsize );
_WINDLL_FUNC int SetHexLogBufferSize( LOG *g , long hexlog_bufsize , long max_log_hexbufsize );
_WINDLL_FUNC int SetLogOutputFuncDirectly( LOG *g , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally );
_WINDLL_FUNC int SetLogStyleFuncDirectly( LOG *g , funcLogStyle *pfuncLogStyle );

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
_WINDLL_FUNC int SetLogOptionsG( int log_options );
_WINDLL_FUNC int SetLogFileChangeTestG( long interval );
_WINDLL_FUNC int SetLogCustLabelG( int index , char *cust_label );
_WINDLL_FUNC int SetLogRotateModeG( int rotate_mode );
_WINDLL_FUNC int SetLogRotateSizeG( long log_rotate_size );
_WINDLL_FUNC int SetLogRotatePressureFactorG( long pressure_factor );
_WINDLL_FUNC int SetBeforeRotateFileFuncG( funcAfterRotateFile *pfuncAfterRotateFile );
_WINDLL_FUNC int SetAfterRotateFileFuncG( funcAfterRotateFile *pfuncAfterRotateFile );
_WINDLL_FUNC int SetFilterLogFuncG( funcFilterLog *pfuncFilterLog );
_WINDLL_FUNC int SetLogBufferSizeG( long log_bufsize , long max_log_bufsize );
_WINDLL_FUNC int SetHexLogBufferSizeG( long hexlog_bufsize , long max_log_hexbufsize );
_WINDLL_FUNC int SetLogOutputFuncDirectlyG( funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally );
_WINDLL_FUNC int SetLogStyleFuncDirectlyG( funcLogStyle *pfuncLogStyle );
#endif

#define TEST_LOGLEVEL_NOTENOUGH(_log_level_,_g_)	( (_log_level_) < (_g_)->log_level )
#define TEST_ATTRIBUTE(_param_,_attr_)			( ( _param_ & _attr_ ) == _attr_ )
#define TEST_NOT_ATTRIBUTE(_param_,_attr_)		( ( _param_ & _attr_ ) != _attr_ )
#define TEST_STDSTREAM(_fd_)				( (_fd_) != FD_NULL && ( (_fd_) == STDOUT_HANDLE || (_fd_) == STDERR_HANDLE ) )
#define TEST_FILEFD(_fd_)				( (_fd_) != FD_NULL && (_fd_) != STDOUT_HANDLE && (_fd_) != STDERR_HANDLE )

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX )
/* 得到基于线程本地存储的缺省日志句柄的函数版本 */
_WINDLL_FUNC LOG *GetGlobalLOG();
_WINDLL_FUNC void SetGlobalLOG( LOG *g );
#endif

/********************************************************/
/*                    以下为钩子开发                    */
/********************************************************/

/* 平台宏 */
#if ( defined _WIN32 )
#define LOG_NEWLINE		"\r\n"
#define LOG_NEWLINE_LEN		2
#define SNPRINTF		_snprintf
#define VSNPRINTF		_vsnprintf
#define STDOUT_HANDLE		1
#define STDERR_HANDLE		2
#define FD_NULL			-1
#define WRITE			_write
#define STAT			_stat
#define ACCESS			_access
#define ACCESS_OK		00
#define RENAME			rename
#elif ( defined __unix ) || ( defined __linux__ )
#define LOG_NEWLINE		"\n"
#define LOG_NEWLINE_LEN		1
#define SNPRINTF		snprintf
#define VSNPRINTF		vsnprintf
#define STDOUT_HANDLE		STDOUT_FILENO
#define STDERR_HANDLE		STDERR_FILENO
#define FD_NULL			-1
#define WRITE			write
#define STAT			stat
#define ACCESS			access
#define ACCESS_OK		F_OK
#define RENAME			rename
#endif

/* 代码宏 */
#if ( defined _WIN32 )
#define SYSTEMTIME2TIMEVAL_USEC(_syst_,_tv_) \
		(_tv_).tv_usec = (_syst_).wMilliseconds * 1000 ;
#define SYSTEMTIME2TM(_syst_,_stime_) \
		(_stime_).tm_year = (_syst_).wYear - 1900 ; \
		(_stime_).tm_mon = (_syst_).wMonth - 1 ; \
		(_stime_).tm_mday = (_syst_).wDay ; \
		(_stime_).tm_hour = (_syst_).wHour ; \
		(_stime_).tm_min = (_syst_).wMinute ; \
		(_stime_).tm_sec = (_syst_).wSecond ;
#define LOCALTIME(_tt_,_stime_) \
	{ \
		SYSTEMTIME	stNow ; \
		GetLocalTime( & stNow ); \
		SYSTEMTIME2TM( stNow , (_stime_) ); \
	}
#elif ( defined __unix ) || ( defined __linux__ )
#define LOCALTIME(_tt_,_stime_) \
	localtime_r(&(_tt_),&(_stime_));
#endif

#ifndef STRCMP
#define STRCMP(_a_,_C_,_b_) ( strcmp(_a_,_b_) _C_ 0 )
#define STRNCMP(_a_,_C_,_b_,_n_) ( strncmp(_a_,_b_,_n_) _C_ 0 )
#endif

#ifndef STRICMP
#if ( defined _WIN32 )
#define STRICMP(_a_,_C_,_b_) ( stricmp(_a_,_b_) _C_ 0 )
#define STRNICMP(_a_,_C_,_b_,_n_) ( strnicmp(_a_,_b_,_n_) _C_ 0 )
#elif ( defined __unix ) || ( defined __linux__ )
#define STRICMP(_a_,_C_,_b_) ( strcasecmp(_a_,_b_) _C_ 0 )
#define STRNICMP(_a_,_C_,_b_,_n_) ( strncasecmp(_a_,_b_,_n_) _C_ 0 )
#endif
#endif

#define OFFSET_BUFPTR(_logbuf_,_offset_len_) \
	if( _offset_len_ != -1 ) \
	{ \
		(_logbuf_)->bufptr += _offset_len_ ; \
		(_logbuf_)->buf_remain_len -= _offset_len_ ; \
	}

_WINDLL_FUNC int SetOpenFlag( LOG *g , char open_flag );
_WINDLL_FUNC char IsLogOpened( LOG *g );

_WINDLL_FUNC int GetLogLevel( LOG *g );

_WINDLL_FUNC LOGBUF *GetLogBuffer( LOG *g );
_WINDLL_FUNC LOGBUF *GetHexLogBuffer( LOG *g );

_WINDLL_FUNC long FormatLogBuffer( LOG *g , LOGBUF *logbuf , char *format , ... );
_WINDLL_FUNC long FormatLogBufferV( LOG *g , LOGBUF *logbuf , char *format , va_list valist );
_WINDLL_FUNC long MemcatLogBuffer( LOG *g , LOGBUF *logbuf , char *append , long len );

/* 日志缓冲区结构 */
struct tagLOGBUF
{
	long			max_buf_size ;
	long			buf_size ;
	char			*bufbase ;
	char			*bufptr ;
	long			buf_remain_len ;
} ;

/* 日志句柄结构 */
struct tagLOG
{
	/* 日志文件名 */
	int			output ;
	char			log_pathfilename[ MAXLEN_FILENAME + 1 ];
	funcOpenLog		*pfuncOpenLogFirst ;
	funcOpenLog		*pfuncOpenLog ;
	funcWriteLog		*pfuncWriteLog ;
	funcChangeTest		*pfuncChangeTest ;
	funcCloseLog		*pfuncCloseLog ;
	funcCloseLog		*pfuncCloseLogFinally ;
	void			*open_handle ;
	void			*test_handle ;
	
	int			fd ;
#if ( defined _WIN32 )
	HANDLE			hFile ;
#endif
	
	char			open_flag ;
	
	/* 日志等级 */
	int			log_level ;
	
	funcFilterLog		*pfuncFilterLog ;
	
	/* 行日志风格 */
	long			log_styles ;
	funcLogStyle		*pfuncLogStyle ;
	/* 分析行格式风格，构建格式函数数组，便于写日志时快速填充行日志缓冲区 */
	funcLogStyle		*pfuncLogStyles[ 30 + 1 ] ;
	int			style_func_count ;
	
	/* 日志选项 */
	int			log_options ;
	
	long			file_change_test_interval ;
	long			file_change_test_no ;
	struct STAT		file_change_stat ;
	
	/* 自定义标签 */
	char			cust_label[LOG_MAXCNT_CUST_LABEL][ LOG_MAXLEN_CUST_LABEL + 1 ] ;
	
	/* 日志转档大小 */
	int			rotate_mode ;
	long			log_rotate_size ;
	long			rotate_file_no ;
	long			rotate_file_count ;
	long			pressure_factor ;
	
	long			skip_count ;
	
	funcAfterRotateFile	*pfuncAfterRotateFile ;
	funcBeforeRotateFile	*pfuncBeforeRotateFile ;
	
	/* 行日志缓冲区 */
	LOGBUF			logbuf ;
	
	/* 十六进制日志缓冲区 */
	LOGBUF			hexlogbuf ;
	
	/* 一级缓存 */
	struct timeval		cache1_tv ;
	struct tm		cache1_stime ;
	
	/* 二级缓存 */
	struct timeval		cache2_logstyle_tv ;
	struct tm		cache2_logstyle_stime ;
	char			cache2_logstyle_date_buf[ 10 + 1 ] ;
	long			cache2_logstyle_date_buf_len ;
	char			cache2_logstyle_datetime_buf[ 19 + 1 ] ;
	long			cache2_logstyle_datetime_buf_len ;
	
	unsigned long		cache2_logstyle_pid ;
	char			cache2_logstyle_pid_buf[ 20 + 1 ] ;
	long			cache2_logstyle_pid_buf_len ;
	
	unsigned long		cache2_logstyle_tid ;
	char			cache2_logstyle_tid_buf[ 20 + 1 ] ;
	long			cache2_logstyle_tid_buf_len ;
	
	struct timeval		cache2_rotate_tv ;
	struct tm		cache2_rotate_stime ;
	
	/* 转档文件锁 */
#if ( defined _WIN32 )
	HANDLE			rotate_lock ;
#elif ( defined __unix ) || ( defined __linux__ )
	int			rotate_lock ;
	struct flock		lock ;
#endif
} ;

/********************************************************/
/*                    以下为辅助接口                    */
/********************************************************/

/* 配置辅助函数 */
_WINDLL_FUNC int ConvertLogOutput_atoi( char *output_desc , int *p_log_output );
_WINDLL_FUNC int ConvertLogLevel_atoi( char *log_level_desc , int *p_log_level );
_WINDLL_FUNC int ConvertLogLevel_itoa( int log_level , char **log_level_desc );
_WINDLL_FUNC int ConvertLogStyle_atol( char *line_style_desc , long *p_line_style );
_WINDLL_FUNC int ConvertLogOption_atol( char *log_option_desc , long *p_log_option );
_WINDLL_FUNC int ConvertLogRotateMode_atoi( char *rotate_mode_desc , int *p_rotate_mode );
_WINDLL_FUNC int ConvertBufferSize_atol( char *bufsize_desc , long *p_bufsize );

#ifdef __cplusplus
}
#endif

#endif

