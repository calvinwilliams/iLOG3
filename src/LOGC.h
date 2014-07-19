#ifndef _H_LOGC_
#define _H_LOGC_

/*
 * iLOG3Lite - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 * LastVersion	: v1.0.9
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

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

#ifdef __cplusplus
extern "C" {
#endif

/* 公共宏 */
#ifndef MAXLEN_FILENAME
#define MAXLEN_FILENAME			256
#endif

#ifndef STRCMP
#define STRCMP(_a_,_C_,_b_) ( strcmp(_a_,_b_) _C_ 0 )
#define STRNCMP(_a_,_C_,_b_,_n_) ( strncmp(_a_,_b_,_n_) _C_ 0 )
#endif

#ifndef MEMCMP
#define MEMCMP(_a_,_C_,_b_,_n_) ( memcmp(_a_,_b_,_n_) _C_ 0 )
#endif

/* 跨平台宏 */
#if ( defined __linux__ ) || ( defined __unix )
#define TLS		__thread
#define VSNPRINTF	vsnprintf
#define SNPRINTF	snprintf
#define OPEN		open
#define READ		read
#define WRITE		write
#define CLOSE		close
#define PROCESSID	(unsigned long)getpid()
#define THREADID	(unsigned long)pthread_self()
#define NEWLINE		"\n"
#elif ( defined _WIN32 )
#define TLS		__declspec( thread )
#define VSNPRINTF	_vsnprintf
#define SNPRINTF	_snprintf
#define OPEN		_open
#define READ		_read
#define WRITE		_write
#define CLOSE		_close
#define PROCESSID	(unsigned long)GetCurrentProcessId()
#define THREADID	(unsigned long)GetCurrentThreadId()
#define NEWLINE		"\r\n"
#endif

/* 简单日志函数 */
#ifndef LOGLEVEL_DEBUG
#define LOGLEVEL_DEBUG		0
#define LOGLEVEL_INFO		1
#define LOGLEVEL_WARN		2
#define LOGLEVEL_ERROR		3
#define LOGLEVEL_FATAL		4
#endif

void SetLogFile( char *format , ... );
void SetLogLevel( int log_level );

int FatalLog( char *c_filename , long c_fileline , char *format , ... );
int ErrorLog( char *c_filename , long c_fileline , char *format , ... );
int WarnLog( char *c_filename , long c_fileline , char *format , ... );
int InfoLog( char *c_filename , long c_fileline , char *format , ... );
int DebugLog( char *c_filename , long c_fileline , char *format , ... );

int FatalHexLog( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int ErrorHexLog( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int WarnHexLog( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int InfoHexLog( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );
int DebugHexLog( char *c_filename , long c_fileline , char *buf , long buflen , char *format , ... );

#if ( defined __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901 )

#define FATALLOG(...)			FatalLog( __FILE__ , __LINE__ , __VA_ARGS__ );
#define ERRORLOG(...)			ErrorLog( __FILE__ , __LINE__ , __VA_ARGS__ );
#define WARNLOG(...)			WarnLog( __FILE__ , __LINE__ , __VA_ARGS__ );
#define INFOLOG(...)			InfoLog( __FILE__ , __LINE__ , __VA_ARGS__ );
#define DEBUGLOG(...)			DebugLog( __FILE__ , __LINE__ , __VA_ARGS__ );

#define FATALHEXLOG(_buf_,_buflen_,...)	FatalHexLog( __FILE__ , __LINE__ , buf , buflen , __VA_ARGS__ );
#define ERRORHEXLOG(_buf_,_buflen_,...)	ErrorHexLog( __FILE__ , __LINE__ , buf , buflen , __VA_ARGS__ );
#define WARNHEXLOG(_buf_,_buflen_,...)	WarnHexLog( __FILE__ , __LINE__ , buf , buflen , __VA_ARGS__ );
#define INFOHEXLOG(_buf_,_buflen_,...)	InfoHexLog( __FILE__ , __LINE__ , buf , buflen , __VA_ARGS__ );
#define DEBUGHEXLOG(_buf_,_buflen_,...)	DebugHexLog( __FILE__ , __LINE__ , buf , buflen , __VA_ARGS__ );

#endif

#ifdef __cplusplus
}
#endif

#endif
