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

#include "LOG.h"

/* 日志等级描述对照表 */ /* log level describe */
static char		sg_aszLogLevelDesc[][6+1] = { "DEBUG " , "INFO  " , "NOTICE" , "WARN  " , "ERROR " , "FATAL " , "NOLOG " } ;

/* 版本标识 */ /* version */
_WINDLL_FUNC int	_LOG_VERSION_1_3_2 = 0 ;

/* 线程本地存储全局对象 */ /* TLS */
#if ( defined _WIN32 )
__declspec( thread ) LOG	*tls_g = NULL ;
#elif ( defined __linux__ ) || ( defined _AIX )
__thread LOG			*tls_g = NULL ;
#endif

/* 临界区 */ /* critical region */
#if ( defined __linux__ ) || ( defined __unix ) || ( defined _AIX )
pthread_mutex_t		g_pthread_mutex = PTHREAD_MUTEX_INITIALIZER ;
#endif

static int CreateMutexSection( LOG *g )
{
#if ( defined _WIN32 )
	char		lock_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	strcpy( lock_pathfilename , "Global\\iLOG3_ROTATELOCK" );
	g->rotate_lock = CreateMutex( NULL , FALSE , lock_pathfilename ) ;
	if( g->rotate_lock == NULL )
		return LOG_RETURN_ERROR_INTERNAL;
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	char		lock_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	mode_t		m ;
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	SNPRINTF( lock_pathfilename , sizeof(lock_pathfilename) , "/tmp/iLOG3.lock" );
	m=umask(0);
	g->rotate_lock = open( lock_pathfilename , O_CREAT|O_APPEND|O_WRONLY , S_IRWXU|S_IRWXG|S_IRWXO ) ;
	umask(m);
	if( g->rotate_lock == FD_NULL )
		return LOG_RETURN_ERROR_INTERNAL;
#endif
	return 0;
}

static int DestroyMutexSection( LOG *g )
{
#if ( defined _WIN32 )
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( g->rotate_lock )
	{
		CloseHandle( g->rotate_lock );
	}
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( g->rotate_lock != -1 )
	{
		close( g->rotate_lock );
	}
#endif
	return 0;
}

static int EnterMutexSection( LOG *g )
{
#if ( defined _WIN32 )
	DWORD	dw ;
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	dw = WaitForSingleObject( g->rotate_lock , INFINITE ) ;
	if( dw != WAIT_OBJECT_0 )
		return LOG_RETURN_ERROR_INTERNAL;
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	int	nret ;
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	memset( & (g->lock) , 0x00 , sizeof(g->lock) );
	g->lock.l_type = F_WRLCK ;
	g->lock.l_whence = SEEK_SET ;
	g->lock.l_start = 0 ;
	g->lock.l_len = 0 ;
	nret = fcntl( g->rotate_lock , F_SETLKW , & (g->lock) ) ;
	if( nret == -1 )
		return LOG_RETURN_ERROR_INTERNAL;
	
	pthread_mutex_lock( & g_pthread_mutex );
#endif
	return 0;
}

static int LeaveMutexSection( LOG *g )
{
#if ( defined _WIN32 )
	BOOL	bret ;
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	bret = ReleaseMutex( g->rotate_lock ) ;
	if( bret != TRUE )
		return LOG_RETURN_ERROR_INTERNAL;
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	int	nret ;
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	pthread_mutex_unlock( & g_pthread_mutex );
	memset( & (g->lock) , 0x00 , sizeof(g->lock) );
	g->lock.l_type = F_UNLCK ;
	g->lock.l_whence = SEEK_SET ;
	g->lock.l_start = 0 ;
	g->lock.l_len = 0 ;
	nret = fcntl( g->rotate_lock , F_SETLK , & (g->lock) ) ;
	if( nret == -1 )
		return LOG_RETURN_ERROR_INTERNAL;
#endif
	return 0;
}

/* 调整缓冲区大小 */ /* adjust buffer size */
static int SetBufferSize( LOG *g , LOGBUF *logbuf , long buf_size , long max_buf_size )
{
	int		nret = 0 ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	if( logbuf == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( max_buf_size != -1 )
	{
		logbuf->max_buf_size = max_buf_size ;
	}
	
	if( buf_size != -1 )
	{
		if( logbuf->bufbase == NULL )
		{
			nret = EnterMutexSection( g ) ;
			if( nret )
				return nret;
			logbuf->bufbase = (char*)malloc( buf_size ) ;
			LeaveMutexSection( g );
			if( logbuf->bufbase == NULL )
				return LOG_RETURN_ERROR_ALLOC;
			
			logbuf->buf_remain_len = buf_size-1 ;
			logbuf->bufptr = logbuf->bufbase ;
			logbuf->buf_size = buf_size ;
			memset( logbuf->bufbase , 0x00 , logbuf->buf_size );
		}
		else
		{
			int	bufptr_offset = logbuf->bufptr - logbuf->bufbase ;
			char	*tmp = NULL ;
			
			nret = EnterMutexSection( g ) ;
			if( nret )
				return nret;
			tmp = (char*)realloc( logbuf->bufbase , buf_size ) ;
			LeaveMutexSection( g );
			if( tmp == NULL )
				return LOG_RETURN_ERROR_ALLOC;
			
			logbuf->buf_remain_len = logbuf->buf_remain_len + ( buf_size - logbuf->buf_size ) ;
			logbuf->bufptr = tmp + bufptr_offset ;
			logbuf->bufbase = tmp ;
			logbuf->buf_size = buf_size ;
			memset( logbuf->bufptr , 0x00 , logbuf->buf_remain_len+1 );
		}
	}
	
	return 0;
}

/* 销毁日志句柄 */ /* destruction of log handle */
void DestroyLogHandle( LOG *g )
{
	if( g )
	{
		if( g->logbuf.bufbase )
		{
			free( g->logbuf.bufbase );
		}
		if( g->hexlogbuf.bufbase )
		{
			free( g->hexlogbuf.bufbase );
		}
		
		if( g->open_flag == 1 )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				if( g->pfuncCloseLogFinally )
				{
					g->pfuncCloseLogFinally( g , & (g->open_handle) );
					g->open_flag = 0 ;
				}
			}
		}
		
		DestroyMutexSection( g );
		
		memset( g , 0x00 , sizeof(LOG) );
		free(g);
	}
	
	return;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
void DestroyLogHandleG()
{
	DestroyLogHandle( tls_g ); tls_g = NULL ;
}
#endif

/* 创建日志句柄 */ /* create log handle */
LOG *CreateLogHandle()
{
	LOG		_g , *g = NULL ;
	int		nret ;
	
#if ( defined _WIN32 )
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#else
	return LOG_RETURN_ERROR_NOTSUPPORT;
#endif
	
	memset( & _g , 0x00 , sizeof(LOG) );
	nret = CreateMutexSection( & _g ) ;
	if( nret )
		return NULL;
	
	nret = EnterMutexSection( & _g ) ;
	if( nret )
		return NULL;
	g = (LOG *)malloc( sizeof(LOG) ) ;
	LeaveMutexSection( & _g );
	if( g == NULL )
		return NULL;
	
	memcpy( g , & _g , sizeof(LOG) );
	
	SetLogLevel( g , LOG_LEVEL_DEFAULT );
	SetLogStyles( g , LOG_STYLES_DEFAULT , LOG_NO_STYLEFUNC );
	SetLogOptions( g , LOG_OPTION_CHANGE_TEST );
	
	g->rotate_mode = LOG_ROTATEMODE_NONE ;
	g->rotate_file_no = 1 ;
	g->rotate_file_count = LOG_ROTATE_SIZE_FILE_COUNT_DEFAULT ;
	g->pressure_factor = LOG_ROTATE_SIZE_PRESSURE_FACTOR_DEFAULT ;
	g->fsync_period = LOG_FSYNC_PERIOD ;
	g->fsync_elapse = g->fsync_period ;
	
	g->logbuf.buf_size = 0 ;
	g->logbuf.bufbase = NULL ;
	g->logbuf.bufptr = NULL ;
	g->logbuf.buf_remain_len = 0 ;
	nret = SetLogBufferSize( g , LOG_BUFSIZE_DEFAULT , LOG_BUFSIZE_MAX ) ;
	if( nret )
	{
		DestroyLogHandle( g );
		return NULL;
	}
	
	g->hexlogbuf.buf_size = 0 ;
	g->hexlogbuf.bufbase = NULL ;
	g->hexlogbuf.bufptr = NULL ;
	g->hexlogbuf.buf_remain_len = 0 ;
	nret = SetHexLogBufferSize( g , LOG_HEXLOG_BUFSIZE_DEFAULT , LOG_HEXLOG_BUFSIZE_MAX ) ;
	if( nret )
	{
		DestroyLogHandle( g );
		return NULL;
	}
	
	return g;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOG *CreateLogHandleG()
{
	tls_g = CreateLogHandle() ;
	return tls_g;
}
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOG *GetLogHandleG()
{
	return tls_g;
}

void GetLogHandlePtrG( LOG **pp_g )
{
	pp_g = & tls_g ;
	return;
}

void SetLogHandleG( LOG *g )
{
	tls_g = g ;
	return;
}
#endif

/* 打开、输出、关闭日志函数 */ /* open , write , close log functions */
#if ( defined _WIN32 )

#define LOG_WINOPENFILE_TRYCOUNT	1000	/* windows上多线程打开同一文件会有互斥现象 */

static int OpenLog_OpenFile( LOG *g , char *log_pathfilename , void **open_handle )
{
	long	l ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( log_pathfilename[0] == '\0' )
		return 0;
	if( g->open_flag == 1 )
		return 0;
	
	for( l = 0 ; l < LOG_WINOPENFILE_TRYCOUNT ; l++ )
	{
		g->hFile = CreateFileA( log_pathfilename , GENERIC_WRITE , FILE_SHARE_READ | FILE_SHARE_WRITE , NULL , OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL ) ;
		if( g->hFile != INVALID_HANDLE_VALUE )
			break;
	}
	if( l >= LOG_WINOPENFILE_TRYCOUNT )
		return LOG_RETURN_ERROR_OPENFILE;
	
	g->open_flag = 1 ;
	return 0;
}

static int WriteLog_WriteFile( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	BOOL	bret ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return -1;
	
	SetFilePointer( g->hFile , 0 , NULL , FILE_END );
	bret = WriteFile( g->hFile , buf , len , writelen , NULL ) ;
	if( bret != TRUE )
		return LOG_RETURN_ERROR_WRITEFILE;
	
	return 0;
}

static int CloseLog_CloseHandle( LOG *g , void **open_handle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return 0;
	
	CloseHandle( g->hFile );
	
	g->open_flag = 0 ;
	return 0;
}

static int OpenLog_RegisterEventSource( LOG *g , char *log_pathfilename , void **open_handle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( log_pathfilename[0] == '\0' )
		return 0;
	if( g->open_flag == 1 )
		return 0;
	
	g->hFile = RegisterEventSource( NULL , g->log_pathfilename ) ;
	if( g->hFile == NULL )
		return LOG_RETURN_ERROR_OPENFILE;
	
	g->open_flag = 1 ;
	return 0;
}

static int WriteLog_ReportEvent( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	unsigned short	event_log_level ;
	char		*ptr = NULL ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return -1;
	
	if( log_level == LOG_LEVEL_DEBUG )
		event_log_level = EVENTLOG_INFORMATION_TYPE ;
	else if( log_level == LOG_LEVEL_INFO )
		event_log_level = EVENTLOG_INFORMATION_TYPE ;
	else if( log_level == LOG_LEVEL_NOTICE )
		event_log_level = EVENTLOG_INFORMATION_TYPE ;
	else if( log_level == LOG_LEVEL_WARN )
		event_log_level = EVENTLOG_WARNING_TYPE ;
	else if( log_level == LOG_LEVEL_ERROR )
		event_log_level = EVENTLOG_ERROR_TYPE ;
	else if( log_level == LOG_LEVEL_FATAL )
		event_log_level = EVENTLOG_ERROR_TYPE ;
	else
		event_log_level = EVENTLOG_ERROR_TYPE ;
	ptr = buf ;
	ReportEvent( g->hFile , event_log_level , 0 , 0 , NULL , 1 , 0 , & ptr , NULL );
	
	return 0;
}

static long CloseLog_DeregisterEventSource( LOG *g , void **open_handle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return 0;
	
	DeregisterEventSource( g->hFile );
	
	g->open_flag = 0 ;
	return 0;
}

#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )

static int OpenLog_open( LOG *g , char *log_pathfilename , void **open_handle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( log_pathfilename[0] == '\0' )
		return 0;
	if( g->open_flag == 1 )
		return 0;
	
	g->fd = open( log_pathfilename , O_CREAT|O_APPEND|O_WRONLY , S_IRWXU|S_IRWXG|S_IRWXO ) ;
	if( g->fd == -1 )
		return LOG_RETURN_ERROR_OPENFILE;
	
	FSTAT( g->fd , & (g->file_change_stat) ) ;
	
	g->open_flag = 1 ;
	return 0;
}

static int CloseLog_close( LOG *g , void **open_handle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return 0;
	
	close( g->fd );
	
	g->open_flag = 0 ;
	return 0;
}

static int OpenLog_openlog( LOG *g , char *log_pathfilename , void **open_handle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( log_pathfilename[0] == '\0' )
		return 0;
	if( g->open_flag == 1 )
		return 0;
	
	openlog( g->log_pathfilename , LOG_PID , LOG_USER );
	
	g->open_flag = 1 ;
	return 0;
}

static int WriteLog_syslog( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	int	syslog_log_level ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return -1;
	
	if( log_level == LOG_LEVEL_DEBUG )
		syslog_log_level = LOG_DEBUG ;
	else if( log_level == LOG_LEVEL_INFO )
		syslog_log_level = LOG_INFO ;
	else if( log_level == LOG_LEVEL_NOTICE )
		syslog_log_level = LOG_NOTICE ;
	else if( log_level == LOG_LEVEL_WARN )
		syslog_log_level = LOG_WARNING ;
	else if( log_level == LOG_LEVEL_ERROR )
		syslog_log_level = LOG_ERR ;
	else if( log_level == LOG_LEVEL_FATAL )
		syslog_log_level = LOG_EMERG ;
	else
		syslog_log_level = LOG_ALERT ;
	syslog( syslog_log_level , buf );
	(*writelen) = len ;
	
	return 0;
}

static int CloseLog_closelog( LOG *g , void **open_handle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return 0;
	
	closelog();
	
	g->open_flag = 0 ;
	return 0;
}

#endif

static int WriteLog_write( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( g->open_flag == 0 )
		return -1;
	
	(*writelen) = WRITE( g->fd , buf , len ) ;
	if( (*writelen) == -1 )
		return LOG_RETURN_ERROR_WRITEFILE;
	
	return 0;
}

static int ChangeTest_interval( LOG *g , void **test_handle )
{
	int		nret ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->file_change_test_no--;
	if( g->file_change_test_no < 1 )
	{
		struct STAT	file_change_stat ;
		nret = STAT( g->log_pathfilename , & file_change_stat ) ;
		if(	(
				nret == -1
				|| file_change_stat.st_size < g->file_change_stat.st_size
#if ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
				|| file_change_stat.st_ino != g->file_change_stat.st_ino
#endif
			) && g->pfuncCloseLogFinally )
		{
			nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
			if( nret )
				return nret;
		}
		g->file_change_test_no = g->file_change_test_interval ;
	}
	
	return 0;
}

int ExpandPathFilename( char *pathfilename , long pathfilename_bufsize )
{
	long		pathfilename_len ;
	
	char		*p1 = NULL , *p2 = NULL ;
	char		env_key[ MAXLEN_FILENAME + 1 ] ;
	long		env_key_len ;
	char		*env_val = NULL ;
	long		env_val_len ;
	
	pathfilename_len = strlen(pathfilename) ;
	
	p1 = strchr( pathfilename , '$' );
	while( p1 )
	{
		/* 展开环境变量 */ /* expand environment variable */
		p2 = strchr( p1 + 1 , '$' ) ;
		if( p2 == NULL )
			return LOG_RETURN_ERROR_PARAMETER;
		
		memset( env_key , 0x00 , sizeof(env_key) );
		env_key_len = p2 - p1 + 1 ;
		strncpy( env_key , p1 + 1 , env_key_len - 2 );
		env_val = getenv( env_key ) ;
		if( env_val == NULL )
			return LOG_RETURN_ERROR_PARAMETER;
		
		env_val_len = strlen(env_val) ;
		if( pathfilename_len + ( env_val_len - env_key_len ) > pathfilename_bufsize-1 )
			return LOG_RETURN_ERROR_PARAMETER;
		
		memmove( p2+1 + ( env_val_len - env_key_len ) , p2+1 , strlen(p2+1) + 1 );
		memcpy( p1 , env_val , env_val_len );
		pathfilename_len += env_val_len - env_key_len ;
		
		p1 = strchr( p1 + ( env_val_len - env_key_len ) , '$' );
	}
	
	return 0;
}

/* 设置日志输出 */ /* set log output */
int SetLogOutput( LOG *g , int output , char *log_pathfilename , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally )
{
	char		pathfilename[ MAXLEN_FILENAME + 1 ] ;
	
	int		nret = 0 ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( log_pathfilename == NULL || log_pathfilename[0] == '\0' )
	{
		memset( pathfilename , 0x00 , sizeof(pathfilename) );
	}
	else
	{
		memset( pathfilename , 0x00 , sizeof(pathfilename) );
		strncpy( pathfilename , log_pathfilename , sizeof(pathfilename)-1 );
		
		if( output != LOG_OUTPUT_CALLBACK )
		{
			nret = ExpandPathFilename( pathfilename , sizeof(pathfilename) ) ;
			if( nret )
				return nret;
		}
	}
	
	if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_SET_OUTPUT_BY_FILENAME ) )
	{
		if( strncmp( pathfilename , "#stdout#" , 8 ) == 0 )
		{
			memmove( pathfilename , pathfilename+8 , strlen(pathfilename+8)+1 );
			output = LOG_OUTPUT_STDOUT ;
		}
		else if( strncmp( pathfilename , "#stderr#" , 8 ) == 0 )
		{
			memmove( pathfilename , pathfilename+8 , strlen(pathfilename+8)+1 );
			output = LOG_OUTPUT_STDERR ;
		}
		else if( strncmp( pathfilename , "#syslog#" , 8 ) == 0 )
		{
			memmove( pathfilename , pathfilename+8 , strlen(pathfilename+8)+1 );
			output = LOG_OUTPUT_SYSLOG ;
		}
	}
	
	if( output == LOG_OUTPUT_FILE && TEST_ATTRIBUTE( g->log_options , LOG_OPTION_FILENAME_APPEND_DOT_LOG ) )
	{
		if( strlen(pathfilename) + 4 > sizeof(pathfilename) - 1 )
			return LOG_RETURN_ERROR_PARAMETER;
		
		strcat( pathfilename , ".log" );
	}
	
	if( g->open_flag == 1 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
		{
			if( g->pfuncCloseLogFinally )
			{
				nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
			}
		}
	}
	
	if( TEST_NO_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE )
		&& TEST_NO_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST )
		&& TEST_NO_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
	{
		g->log_options |= LOG_OPTION_OPEN_DEFAULT ;
	}
	
	memset( g->log_pathfilename , 0x00 , sizeof(g->log_pathfilename) );
	strncpy( g->log_pathfilename , pathfilename , sizeof(g->log_pathfilename)-1 );
	
	if( output != LOG_OUTPUT_INVALID )
	{
		g->output = output ;
		if( g->output == LOG_OUTPUT_STDOUT )
		{
			g->fd = STDOUT_HANDLE ;
			g->pfuncOpenLogFirst = NULL ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_write ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = NULL ;
			
			g->log_options &= ~LOG_OPTION_OPEN_AND_CLOSE ;
			g->log_options &= ~LOG_OPTION_CHANGE_TEST ;
			g->log_options |= LOG_OPTION_OPEN_ONCE ;
			
			g->open_flag = 1 ;
		}
		else if( output == LOG_OUTPUT_STDERR )
		{
			g->fd = STDERR_HANDLE ;
			g->pfuncOpenLogFirst = NULL ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_write ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = NULL ;
			
			g->log_options &= ~LOG_OPTION_OPEN_AND_CLOSE ;
			g->log_options &= ~LOG_OPTION_CHANGE_TEST ;
			g->log_options |= LOG_OPTION_OPEN_ONCE ;
			
			g->open_flag = 1 ;
		}
		else if( output == LOG_OUTPUT_SYSLOG )
		{
#if ( defined _WIN32 )
			g->pfuncOpenLogFirst = & OpenLog_RegisterEventSource ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_ReportEvent ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = & CloseLog_DeregisterEventSource ;
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
			g->pfuncOpenLogFirst = & OpenLog_openlog ;
			g->pfuncOpenLog = NULL ;
			g->pfuncWriteLog = & WriteLog_syslog ;
			g->pfuncChangeTest = NULL ;
			g->pfuncCloseLog = NULL ;
			g->pfuncCloseLogFinally = & CloseLog_closelog ;
#endif
			
			g->log_options &= ~LOG_OPTION_OPEN_AND_CLOSE ;
			g->log_options &= ~LOG_OPTION_CHANGE_TEST ;
			g->log_options |= LOG_OPTION_OPEN_ONCE ;
			
			g->open_flag = 0 ;
		}
		else if( output == LOG_OUTPUT_FILE )
		{
#if ( defined _WIN32 )
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				g->pfuncOpenLogFirst = & OpenLog_OpenFile ;
				g->pfuncOpenLog = NULL ;
				g->pfuncWriteLog = & WriteLog_WriteFile ;
				g->pfuncChangeTest = & ChangeTest_interval ;
				g->pfuncCloseLog = NULL ;
				g->pfuncCloseLogFinally = & CloseLog_CloseHandle ;
			}
			else
			{
				g->pfuncOpenLogFirst = NULL ;
				g->pfuncOpenLog = & OpenLog_OpenFile ;
				g->pfuncWriteLog = & WriteLog_WriteFile ;
				g->pfuncChangeTest = NULL ;
				g->pfuncCloseLog = & CloseLog_CloseHandle ;
				g->pfuncCloseLogFinally = NULL ;
			}
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				g->pfuncOpenLogFirst = & OpenLog_open ;
				g->pfuncOpenLog = NULL ;
				g->pfuncWriteLog = & WriteLog_write ;
				g->pfuncChangeTest = & ChangeTest_interval ;
				g->pfuncCloseLog = NULL ;
				g->pfuncCloseLogFinally = & CloseLog_close ;
			}
			else
			{
				g->pfuncOpenLogFirst = NULL ;
				g->pfuncOpenLog = & OpenLog_open ;
				g->pfuncWriteLog = & WriteLog_write ;
				g->pfuncChangeTest = NULL ;
				g->pfuncCloseLog = & CloseLog_close ;
				g->pfuncCloseLogFinally = NULL ;
			}
#endif
			g->open_flag = 0 ;
		}
		else if( output == LOG_OUTPUT_CALLBACK )
		{
			SetLogOutputFuncDirectly( g , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally );
			g->open_flag = 0 ;
		}
	}
	
	if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
	{
		if( g->pfuncOpenLogFirst )
		{
			nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
			if( nret )
				return nret;
		}
	}
	
	return 0;
}

int SetLogOutput2V( LOG *g , int output , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally , char *log_pathfilename_format , va_list valist )
{
	char		log_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	memset( log_pathfilename , 0x00 , sizeof(log_pathfilename) );
	VSNPRINTF( log_pathfilename , sizeof(log_pathfilename)-1 , log_pathfilename_format , valist );
	
	return SetLogOutput( g , output , log_pathfilename , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally );
}

int SetLogOutput2( LOG *g , int output , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally , char *log_pathfilename_format , ... )
{
	va_list		valist ;
	int		nret = 0 ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	va_start( valist , log_pathfilename_format );
	nret = SetLogOutput2V( g , output , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally , log_pathfilename_format , valist ) ;
	va_end( valist );
	return nret;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogOutputG( int output , char *log_pathfilename , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally )
{
	return SetLogOutput( tls_g , output , log_pathfilename , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally );
}

int SetLogOutput2G( int output , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally , char *log_pathfilename_format , ... )
{
	va_list		valist ;
	int		nret = 0 ;
	
	va_start( valist , log_pathfilename_format );
	nret = SetLogOutput2V( tls_g , output , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally , log_pathfilename_format , valist ) ;
	va_end( valist );
	return nret;
}
#endif

int ReOpenLogOutput( LOG *g )
{
	int		nret = 0 ;
	
	if( g->open_flag == 1 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
		{
			if( g->pfuncCloseLogFinally )
			{
				nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
			}
		}
		
		g->open_flag = 0 ;
	}
	
	if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
	{
		if( g->pfuncOpenLogFirst )
		{
			nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
			if( nret )
				return nret;
		}
	}
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int ReOpenLogOutputG()
{
	return ReOpenLogOutput( tls_g );
}
#endif

/* 设置日志等级 */ /* set log level */
int SetLogLevel( LOG *g , int log_level )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->log_level = log_level ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogLevelG( int log_level )
{
	return SetLogLevel( tls_g , log_level );
}
#endif

/* 行格式函数集合 */ /* log style functions */
static int LogStyle_SEPARATOR( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , " | " , 3 );
	return 0;
}

static int LogStyle_SEPARATOR2( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , "| " , 2 );
	return 0;
}

static int LogStyle_DATE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_tv.tv_sec != g->cache2_logstyle_tv.tv_sec )
	{
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
		FormatLogBuffer( g , logbuf , "%04d-%02d-%02d" , g->cache1_stime.tm_year+1900 , g->cache1_stime.tm_mon+1 , g->cache1_stime.tm_mday );
		g->cache2_logstyle_tv.tv_sec = g->cache1_tv.tv_sec ;
		g->cache2_logstyle_tv.tv_usec = g->cache1_tv.tv_usec ;
		g->cache2_logstyle_date_buf_len = 10 ;
		memcpy( g->cache2_logstyle_date_buf , logbuf->bufptr - g->cache2_logstyle_date_buf_len , g->cache2_logstyle_date_buf_len );
		g->cache2_logstyle_date_buf[g->cache2_logstyle_date_buf_len] = '\0' ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_date_buf , g->cache2_logstyle_date_buf_len );
	}
	return 0;
}

static int LogStyle_DATETIME( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_tv.tv_sec != g->cache2_logstyle_tv.tv_sec )
	{
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
		FormatLogBuffer( g , logbuf , "%04d-%02d-%02d %02d:%02d:%02d" , g->cache1_stime.tm_year+1900 , g->cache1_stime.tm_mon+1 , g->cache1_stime.tm_mday , g->cache1_stime.tm_hour , g->cache1_stime.tm_min , g->cache1_stime.tm_sec ) ;
		g->cache2_logstyle_tv.tv_sec = g->cache1_tv.tv_sec ;
		g->cache2_logstyle_datetime_buf_len = 19 ;
		memcpy( g->cache2_logstyle_datetime_buf , logbuf->bufptr - g->cache2_logstyle_datetime_buf_len , g->cache2_logstyle_datetime_buf_len ); g->cache2_logstyle_datetime_buf[g->cache2_logstyle_datetime_buf_len] = '\0' ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_datetime_buf , g->cache2_logstyle_datetime_buf_len ) ;
	}
	return 0;
}

static int LogStyle_DATETIMEMS( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
#if ( defined _WIN32 )
	SYSTEMTIME	stNow ;
	GetLocalTime( & stNow );
	SYSTEMTIME2TIMEVAL_USEC( stNow , g->cache1_tv );
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	gettimeofday( & (g->cache1_tv) , NULL );
#endif	
	LogStyle_DATETIME( g , logbuf , c_filename , c_fileline , log_level , format , valist );
	FormatLogBuffer( g , logbuf , ".%06ld" , g->cache1_tv.tv_usec ) ;
	return 0;
}

static int LogStyle_LOGLEVEL( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%-5s" , sg_aszLogLevelDesc[log_level] );
	return 0;
}

static int LogStyle_CUSTLABEL1( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[1-1] );
	return 0;
}

static int LogStyle_CUSTLABEL2( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[2-1] );
	return 0;
}

static int LogStyle_CUSTLABEL3( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[3-1] );
	return 0;
}

static int LogStyle_CUSTLABEL4( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[4-1] );
	return 0;
}

static int LogStyle_CUSTLABEL5( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBuffer( g , logbuf , "%s" , g->cust_label[5-1] );
	return 0;
}

static int LogStyle_SPACE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , " " , 1 );
	return 0;
}

static int LogStyle_PID( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	unsigned long	pid ;
#if ( defined _WIN32 )
	HWND		hd ;
	DWORD		dw ;
	hd = GetForegroundWindow() ;
	GetWindowThreadProcessId( hd , & dw );
	pid = (long)dw ;
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	pid = (long)getpid() ;
#endif
	if( pid != g->cache2_logstyle_pid )
	{
		g->cache2_logstyle_pid_buf_len = FormatLogBuffer( g , logbuf , "%u:" , pid ) ;
		
		memcpy( g->cache2_logstyle_pid_buf , logbuf->bufptr - g->cache2_logstyle_pid_buf_len , g->cache2_logstyle_pid_buf_len ); g->cache2_logstyle_pid_buf[g->cache2_logstyle_pid_buf_len] = '\0' ;
		
		g->cache2_logstyle_pid = pid ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_pid_buf , g->cache2_logstyle_pid_buf_len ) ;
	}
	return 0;
}

static int LogStyle_TID( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	unsigned long	tid ;
#if ( defined _WIN32 )
	tid = (unsigned long)GetCurrentThreadId() ;
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#if ( defined _PTHREAD_H )
	tid = (unsigned long)pthread_self() ;
#else
#endif
#endif
	if( tid != g->cache2_logstyle_tid )
	{
		g->cache2_logstyle_tid_buf_len = FormatLogBuffer( g , logbuf , "%u:" , tid ) ;
		
		memcpy( g->cache2_logstyle_tid_buf , logbuf->bufptr - g->cache2_logstyle_tid_buf_len , g->cache2_logstyle_tid_buf_len ); g->cache2_logstyle_tid_buf[g->cache2_logstyle_tid_buf_len] = '\0' ;
		
		g->cache2_logstyle_tid = tid ;
	}
	else
	{
		MemcatLogBuffer( g , logbuf , g->cache2_logstyle_tid_buf , g->cache2_logstyle_tid_buf_len ) ;
	}
	return 0;
}

static int LogStyle_SOURCE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
#if ( defined _WIN32 )
	char		*pfilename = NULL ;
	pfilename = strrchr( c_filename , '\\' ) ;
	if( pfilename )
	{
		pfilename++;
	}
	else
	{
		pfilename = strrchr( c_filename , '/' ) ;
		if( pfilename )
			pfilename++;
		else
			pfilename = c_filename ;
	}
	FormatLogBuffer( g , logbuf , "%s:%ld" , pfilename , c_fileline ) ;
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	FormatLogBuffer( g , logbuf , "%s:%ld" , c_filename , c_fileline ) ;
#endif
	return 0;
}

static int LogStyle_FORMAT( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	FormatLogBufferV( g , logbuf , format , valist );
	return 0;
}

static int LogStyle_NEWLINE( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	MemcatLogBuffer( g , logbuf , LOG_NEWLINE , LOG_NEWLINE_LEN );
	return 0;
}

/* 填充行日志段 */ /* fill log segments */
static int LogStyle_FuncArray( LOG *g , LOGBUF *logbuf , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	int		format_func_index ;
	funcLogStyle	**ppfuncLogStyle = NULL ;
	int		nret ;
	
	/* 遍历格式函数数组 */ /* travel all log style functions */
	for( format_func_index = 0 , ppfuncLogStyle = g->pfuncLogStyles ; format_func_index < g->style_func_count ; format_func_index++ , ppfuncLogStyle++ )
	{
		nret = (*ppfuncLogStyle)( g , logbuf , c_filename , c_fileline , log_level , format , valist ) ;
		if( nret )
			return nret;
	}
	
	return 0;
}

/* 设置行日志风格 */ /* set log styles */
int SetLogStylesEx( LOG *g , long log_styles , funcLogStyle *pfuncLogStyle , funcLogStyle *pfuncLogStylePrefix )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->log_styles = log_styles ;
	if( g->log_styles == LOG_STYLE_CALLBACK )
	{
		SetLogStyleFuncDirectly( g , pfuncLogStyle );
		return 0;
	}
	
	/* 构造行风格函数数组 */
	g->style_func_count = 0 ;
	
	if( pfuncLogStylePrefix )
	{
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = pfuncLogStylePrefix ;
	}
	
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_DATETIMEMS ) )
	{
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_DATETIMEMS ;
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SEPARATOR ;
	}
	else if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_DATETIME ) )
	{
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_DATETIME ;
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SEPARATOR ;
	}
	else if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_DATE ) )
	{
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_DATE ;
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SEPARATOR ;
	}
	
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_LOGLEVEL ) )
	{
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_LOGLEVEL ;
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SEPARATOR ;
	}
	
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL1 )
		|| TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL2 )
		|| TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL3 ) )
	{
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL1 ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL1 ;
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SPACE ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL2 ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL2 ;
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SPACE ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL3 ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL3 ;
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SPACE ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL4 ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL4 ;
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SPACE ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_CUSTLABEL5 ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_CUSTLABEL5 ;
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SPACE ;
		}
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SEPARATOR2 ;
	}
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_PID )
		|| TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_TID )
		|| TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_SOURCE ) )
	{
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_PID ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_PID ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_TID ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_TID ;
		}
		if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_SOURCE ) )
		{
			if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
				return LOG_RETURN_ERROR_TOO_MANY_STYLES;
			g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SOURCE ;
		}
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_SEPARATOR ;
	}
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_FORMAT ) )
	{
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_FORMAT ;
	}
	if( TEST_ATTRIBUTE( g->log_styles , LOG_STYLE_NEWLINE ) )
	{
		if( g->style_func_count >= LOG_CUST_LABELS_COUNT )
			return LOG_RETURN_ERROR_TOO_MANY_STYLES;
		g->pfuncLogStyles[ g->style_func_count++ ] = LogStyle_NEWLINE ;
	}
	
	g->pfuncLogStyle = & LogStyle_FuncArray ;
	
	return 0;
}

int SetLogStyles( LOG *g , long log_styles , funcLogStyle *pfuncLogStyle )
{
	return SetLogStylesEx( g , log_styles , pfuncLogStyle , NULL );
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogStylesG( long log_styles , funcLogStyle *pfuncLogStyles )
{
	return SetLogStyles( tls_g , log_styles , pfuncLogStyles );
}

int SetLogStylesExG( long log_styles , funcLogStyle *pfuncLogStyles , funcLogStyle *pfuncLogStylePrefix )
{
	return SetLogStylesEx( tls_g , log_styles , pfuncLogStyles , pfuncLogStylePrefix );
}
#endif

/* 转档日志文件 */ /* rotate log file */
static int RotateLogFileSize( LOG *g , long step )
{
	char		rotate_log_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	char		rotate_log_pathfilename_access[ MAXLEN_FILENAME + 1 ] ;
	int		nret ;
	
	if( g->open_flag == 1 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
		{
			if( g->pfuncCloseLogFinally )
			{
				nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
	}
	
	nret = EnterMutexSection( g ) ;
	if( nret )
		return nret;
	
	nret = STAT( g->log_pathfilename , & (g->file_change_stat) ) ;
	if( nret == -1 )
	{
		LeaveMutexSection( g );
		return LOG_RETURN_ERROR_INTERNAL;
	}
	
	if( g->file_change_stat.st_size >= g->log_rotate_size )
	{
		for( g->rotate_file_no = 1 ; g->rotate_file_no <= g->rotate_file_count ; g->rotate_file_no++ )
		{
			SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%ld" , g->log_pathfilename , g->rotate_file_no );
			strcpy( rotate_log_pathfilename_access , rotate_log_pathfilename );
			if( g->pfuncBeforeRotateFile )
				g->pfuncBeforeRotateFile( g , rotate_log_pathfilename_access );
			nret = ACCESS( rotate_log_pathfilename_access , ACCESS_OK ) ;
			if( nret == -1 )
				break;
		}
		if( g->rotate_file_no > g->rotate_file_count )
		{
			g->rotate_file_no = g->rotate_file_count ;
			SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%ld" , g->log_pathfilename , g->rotate_file_count );
		}
		
		nret = RENAME( g->log_pathfilename , rotate_log_pathfilename ) ;
		if( nret )
		{
			UNLINK( rotate_log_pathfilename );
			RENAME( g->log_pathfilename , rotate_log_pathfilename ) ;
		}
		
		if( g->pfuncAfterRotateFile )
			g->pfuncAfterRotateFile( g , rotate_log_pathfilename );
		g->skip_count = 1 ;
	}
	else
	{
		if( step == 0 )
			step = 1 ;
		if( g->pressure_factor == 0 )
			g->skip_count = 1 ;
		else
			g->skip_count = ( g->log_rotate_size - g->file_change_stat.st_size ) / step / g->pressure_factor + 1 ;
	}
	
	LeaveMutexSection( g );
	
	return 0;
}

static int RotateLogFilePerDate( LOG *g )
{
	char		rotate_log_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	int		nret ;
	
	if( g->open_flag == 1 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
		{
			if( g->pfuncCloseLogFinally )
			{
				nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
				if( nret )
					return nret;
				g->open_flag = 0 ;
			}
		}
	}
	
	nret = EnterMutexSection( g ) ;
	if( nret )
		return nret;
	
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_stime.tm_mday == 0 )
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
	
	if( g->cache2_rotate_tv.tv_sec == 0 || g->cache2_rotate_stime.tm_mday == 0 )
	{
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	else if
	(
		g->cache1_tv.tv_sec == g->cache2_rotate_tv.tv_sec
		||
		(
			g->cache1_stime.tm_year == g->cache2_rotate_stime.tm_year
			&&
			g->cache1_stime.tm_mon == g->cache2_rotate_stime.tm_mon
			&&
			g->cache1_stime.tm_mday == g->cache2_rotate_stime.tm_mday
		)
	)
	{
	}
	else
	{
		SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%04d%02d%02d" , g->log_pathfilename , g->cache2_rotate_stime.tm_year+1900 , g->cache2_rotate_stime.tm_mon+1 , g->cache2_rotate_stime.tm_mday );
		RENAME( g->log_pathfilename , rotate_log_pathfilename );
		if( g->pfuncAfterRotateFile )
			g->pfuncAfterRotateFile( g , rotate_log_pathfilename );
		
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	
	LeaveMutexSection( g );
	
	return 0;
}

static int RotateLogFilePerHour( LOG *g )
{
	char		rotate_log_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	int		nret ;
	
	if( g->open_flag == 1 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
		{
			if( g->pfuncCloseLogFinally )
			{
				nret = g->pfuncCloseLogFinally( g , & (g->open_handle) ) ;
				if( nret )
					return nret;
				g->open_flag = 0 ;
			}
		}
	}
	
	nret = EnterMutexSection( g ) ;
	if( nret )
		return nret;
	
	if( g->cache1_tv.tv_sec == 0 )
		g->cache1_tv.tv_sec = time( NULL ) ;
	if( g->cache1_stime.tm_mday == 0 )
		LOCALTIME( g->cache1_tv.tv_sec , g->cache1_stime )
	
	if( g->cache2_rotate_tv.tv_sec == 0 || g->cache2_rotate_stime.tm_mday == 0 )
	{
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	else if
	(
		g->cache2_rotate_tv.tv_sec == g->cache1_tv.tv_sec
		||
		(
			g->cache2_rotate_stime.tm_year == g->cache1_stime.tm_year
			&&
			g->cache2_rotate_stime.tm_mon == g->cache1_stime.tm_mon
			&&
			g->cache2_rotate_stime.tm_mday == g->cache1_stime.tm_mday
			&&
			g->cache2_rotate_stime.tm_hour == g->cache1_stime.tm_hour
		)
	)
	{
	}
	else
	{
		SNPRINTF( rotate_log_pathfilename , sizeof(rotate_log_pathfilename) , "%s.%04d%02d%02d_%02d" , g->log_pathfilename , g->cache2_rotate_stime.tm_year+1900 , g->cache2_rotate_stime.tm_mon+1 , g->cache2_rotate_stime.tm_mday , g->cache2_rotate_stime.tm_hour );
		RENAME( g->log_pathfilename , rotate_log_pathfilename );
		if( g->pfuncAfterRotateFile )
			g->pfuncAfterRotateFile( g , rotate_log_pathfilename );
		
		memcpy( & (g->cache2_rotate_tv) , & (g->cache1_tv) , sizeof(struct timeval) );
		memcpy( & (g->cache2_rotate_stime) , & (g->cache1_stime) , sizeof(struct tm) );
	}
	
	LeaveMutexSection( g );
	
	return 0;
}

/* 写日志基函数 */ /* output log */
int WriteLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , va_list valist )
{
	long		writelen ;
	int		nret ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( format == NULL )
		return 0;
	
	/* 初始化行日志缓冲区 */ /* initialize log buffer  */
	CleanLogBuffer( g , GetLogBuffer(g) );
	
	/* 填充行日志缓冲区 */ /* fill log buffer */
	if( g->pfuncLogStyle )
	{
		nret = g->pfuncLogStyle( g , & (g->logbuf) , c_filename , c_fileline , log_level , format , valist ) ;
		if( nret )
			return nret;
	}
	
	/* 自定义过滤日志 */ /* filter log */
	if( g->pfuncFilterLog )
	{
		nret = g->pfuncFilterLog( g , & (g->open_handle) , log_level , g->logbuf.bufbase , g->logbuf.buf_size-1-1 - g->logbuf.buf_remain_len ) ;
		if( nret )
			return nret;
	}
	
	/* 打开文件 */ /* open log */
	if( g->open_flag == 0 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLogFirst )
			{
				nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
		else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLog )
			{
				nret = g->pfuncOpenLog( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
	}
	
	/* 导出日志缓冲区 */ /* output log */
	if( g->pfuncWriteLog )
	{
		nret = g->pfuncWriteLog( g , & (g->open_handle) , log_level , g->logbuf.bufbase , g->logbuf.buf_size-1-1 - g->logbuf.buf_remain_len , & writelen ) ;
		if( nret )
			return nret;
	}
	
#if ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	if( g->output == LOG_OUTPUT_FILE )
	{
		g->fsync_elapse--;
		if( g->fsync_elapse < 1 )
		{
			fsync( g->fd );
			g->fsync_elapse = g->fsync_period ;
		}
	}
#endif
	
	/* 关闭日志 */ /* close log */
	if( g->open_flag == 1 )
	{
		if( g->output == LOG_OUTPUT_FILE || g->output == LOG_OUTPUT_CALLBACK )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				/* 测试日志文件 */
				if( g->pfuncChangeTest )
				{
					nret = g->pfuncChangeTest( g , & (g->test_handle) ) ;
					if( nret )
						return nret;
				}
			}
			else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
			{
				/* 关闭日志文件 */
				if( g->pfuncCloseLog )
				{
					nret = g->pfuncCloseLog( g , & (g->open_handle) ) ;
					if( nret )
						return nret;
				}
			}
		}
	}
	
	/* 如果输出到文件 */
	if( g->output == LOG_OUTPUT_FILE )
	{
		/* 日志转档侦测 */ /* rotate log file */
		if( g->rotate_mode == LOG_ROTATEMODE_NONE )
		{
		}
		else if( g->rotate_mode == LOG_ROTATEMODE_SIZE && g->log_rotate_size > 0 )
		{
			g->skip_count--;
			if( g->skip_count < 1 )
			{
				RotateLogFileSize( g , writelen );
			}
		}
		else if( g->rotate_mode == LOG_ROTATEMODE_PER_DAY )
		{
			RotateLogFilePerDate( g );
		}
		else if( g->rotate_mode == LOG_ROTATEMODE_PER_HOUR )
		{
			RotateLogFilePerHour( g );
		}
	}
	
	/* 清空一级缓存 */ /* clean level 1 cache */
	g->cache1_tv.tv_sec = 0 ;
	g->cache1_stime.tm_mday = 0 ;
	
	return 0;
}

/* 代码宏 */ /* code macros */
#define WRITELOGBASE(_g_,_log_level_) \
	va_list		valist; \
	int		nret ; \
	if( (_g_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	if( (_g_)->output == LOG_OUTPUT_FILE && (_g_)->log_pathfilename[0] == '\0' ) \
		return 0; \
	if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (_g_) ) ) \
		return 0; \
	va_start( valist , format ); \
	nret = WriteLogBase( (_g_) , c_filename , c_fileline , _log_level_ , format , valist ) ; \
	va_end( valist ); \
	if( nret < 0 ) \
		return nret;

/* 带日志等级的写日志 */ /* write log */
int WriteLevelLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGBASE( g , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteLevelLogG( char *c_filename , long c_fileline , int log_level , char *format , ... )
{
	WRITELOGBASE( tls_g , log_level )
	return 0;
}
#endif

/* 写调试日志 */ /* write debug log */
int WriteDebugLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteDebugLogG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( tls_g , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* 写普通信息日志 */ /* write info log */
int WriteInfoLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteInfoLogG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( tls_g , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* 写通知信息日志 */ /* write notice log */
int WriteNoticeLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_NOTICE )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteNoticeLogG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( tls_g , LOG_LEVEL_NOTICE )
	return 0;
}
#endif

/* 写警告日志 */ /* write warn log */
int WriteWarnLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteWarnLogG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( tls_g , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* 写错误日志 */ /* write error log */
int WriteErrorLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteErrorLogG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( tls_g , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* 写致命错误日志 */ /* write fatal log */
int WriteFatalLog( LOG *g , char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( g , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteFatalLogG( char *c_filename , long c_fileline , char *format , ... )
{
	WRITELOGBASE( tls_g , LOG_LEVEL_FATAL )
	return 0;
}
#endif

/* 写十六进制块日志基函数 */ /* output log */
int WriteHexLogBase( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , va_list valist )
{
	int		row_offset , col_offset ;
	/*
	long		len ;
	*/
	long		writelen ;
	int		nret ;
	
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	/* 初始化十六进制块日志缓冲区 */ /* initialize log buffer  */
	CleanLogBuffer( g , GetHexLogBuffer(g) );
	
	/* 填充行日志缓冲区 */ /* fill log buffer */
	if( format )
	{
		if( g->pfuncLogStyle )
		{
			nret = g->pfuncLogStyle( g , & (g->hexlogbuf) , c_filename , c_fileline , log_level , format , valist ) ;
			if( nret )
				return nret;
		}
	}
	
	/* 填充十六进制块日志缓冲区 */ /* fill hex log buffer */
	if( buffer && buflen > 0 )
	{
		/*
		len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , "             0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F    0123456789ABCDEF" ) ;
		OFFSET_BUFPTR( & (g->hexlogbuf) , len )
		len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , LOG_NEWLINE ) ;
		OFFSET_BUFPTR( & (g->hexlogbuf) , len )
		*/
		FormatLogBuffer( g , & (g->hexlogbuf) , "             0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F    0123456789ABCDEF" LOG_NEWLINE );
		
		row_offset = 0 ;
		col_offset = 0 ;
		while(1)
		{
			/*
			len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , "0x%08X   " , row_offset * 16 ) ;
			OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
			*/
			FormatLogBuffer( g , & (g->hexlogbuf) , "0x%08X   " , row_offset * 16 );
			for( col_offset = 0 ; col_offset < 16 ; col_offset++ )
			{
				if( row_offset * 16 + col_offset < buflen )
				{
					/*
					len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , "%02X " , *((unsigned char *)buffer+row_offset*16+col_offset)) ;
					OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
					*/
					FormatLogBuffer( g , & (g->hexlogbuf) , "%02X " , *((unsigned char *)buffer+row_offset*16+col_offset) );
				}
				else
				{
					/*
					len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , "   " ) ;
					OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
					*/
					FormatLogBuffer( g , & (g->hexlogbuf) , "   " );
				}
			}
			/*
			len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , "  " ) ;
			OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
			*/
			FormatLogBuffer( g , & (g->hexlogbuf) , "  " );
			for( col_offset = 0 ; col_offset < 16 ; col_offset++ )
			{
				if( row_offset * 16 + col_offset < buflen )
				{
					if( isprint( (int)*((unsigned char*)buffer+row_offset*16+col_offset) ) )
					{
						/*
						len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , "%c" , *((unsigned char *)buffer+row_offset*16+col_offset) ) ;
						OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
						*/
						FormatLogBuffer( g , & (g->hexlogbuf) , "%c" , *((unsigned char *)buffer+row_offset*16+col_offset) );
					}
					else
					{
						/*
						len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , "." ) ;
						OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
						*/
						FormatLogBuffer( g , & (g->hexlogbuf) , "." );
					}
				}
				else
				{
					/*
					len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , " " ) ;
					OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
					*/
					FormatLogBuffer( g , & (g->hexlogbuf) , " " );
				}
			}
			/*
			len = SNPRINTF( g->hexlogbuf.bufptr , g->hexlogbuf.buf_remain_len , LOG_NEWLINE ) ;
			OFFSET_BUFPTR_IN_LOOP( & (g->hexlogbuf) , len )
			*/
			FormatLogBuffer( g , & (g->hexlogbuf) , LOG_NEWLINE );
			if( row_offset * 16 + col_offset >= buflen )
				break;
			row_offset++;
		}
	}
	
	if( g->hexlogbuf.bufptr == g->hexlogbuf.bufbase )
		return 0;
	
	if( STRNCMP( g->hexlogbuf.bufptr-(sizeof(LOG_NEWLINE)-1) , != , LOG_NEWLINE , sizeof(LOG_NEWLINE)-1 ) )
	{
		/*
		memcpy( g->hexlogbuf.bufptr-(sizeof(LOG_NEWLINE)-1) , LOG_NEWLINE , sizeof(LOG_NEWLINE)-1 );
		*/
		FormatLogBuffer( g , & (g->hexlogbuf) , LOG_NEWLINE );
	}
	
	/* 自定义过滤日志 */ /* filter log */
	if( g->pfuncFilterLog )
	{
		nret = g->pfuncFilterLog( g , & (g->open_handle) , log_level , g->hexlogbuf.bufbase , g->hexlogbuf.buf_size-1-1 - g->hexlogbuf.buf_remain_len ) ;
		if( nret )
			return nret;
	}
	
	/* 打开文件 */ /* open log */
	if( g->open_flag == 0 )
	{
		if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) || TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLogFirst )
			{
				nret = g->pfuncOpenLogFirst( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
		else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
		{
			/* 打开日志文件 */
			if( g->pfuncOpenLog )
			{
				nret = g->pfuncOpenLog( g , g->log_pathfilename , & (g->open_handle) ) ;
				if( nret )
					return nret;
			}
		}
	}
	
	/* 导出日志缓冲区 */ /* output file */
	if( g->pfuncWriteLog )
	{
		nret = g->pfuncWriteLog( g , & (g->open_handle) , log_level , g->hexlogbuf.bufbase , g->hexlogbuf.buf_size-1-1 - g->hexlogbuf.buf_remain_len , & writelen ) ;
		if( nret )
			return nret;
	}
	
#if ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
	if( g->output == LOG_OUTPUT_FILE )
	{
		g->fsync_elapse--;
		if( g->fsync_elapse < 1 )
		{
			fsync( g->fd );
			g->fsync_elapse = g->fsync_period ;
		}
	}
#endif
	
	/* 关闭日志 */ /* close file */
	if( g->open_flag == 1 )
	{
		if( g->output == LOG_OUTPUT_FILE || g->output == LOG_OUTPUT_CALLBACK )
		{
			if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_CHANGE_TEST ) )
			{
				/* 测试日志文件 */
				if( g->pfuncChangeTest )
				{
					nret = g->pfuncChangeTest( g , & (g->test_handle) ) ;
					if( nret )
						return nret;
				}
			}
			/*
			else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_ONCE ) )
			{
			}
			*/
			else if( TEST_ATTRIBUTE( g->log_options , LOG_OPTION_OPEN_AND_CLOSE ) )
			{
				/* 关闭日志文件 */
				if( g->pfuncCloseLog )
				{
					nret = g->pfuncCloseLog( g , & (g->open_handle) ) ;
					if( nret )
						return nret;
				}
				
			}
		}
	}
	
	/* 如果输出到文件 */
	if( g->output == LOG_OUTPUT_FILE )
	{
		/* 日志转档侦测 */ /* rotate log file */
		if( g->rotate_mode == LOG_ROTATEMODE_NONE )
		{
		}
		else if( g->rotate_mode == LOG_ROTATEMODE_SIZE && g->log_rotate_size > 0 )
		{
			g->skip_count--;
			if( g->skip_count < 1 )
			{
				RotateLogFileSize( g , writelen );
			}
		}
		else if( g->rotate_mode == LOG_ROTATEMODE_PER_DAY )
		{
			RotateLogFilePerDate( g );
		}
		else if( g->rotate_mode == LOG_ROTATEMODE_PER_HOUR )
		{
			RotateLogFilePerHour( g );
		}
	}
	
	/* 清空一级缓存 */ /* clean level 1 cache */
	g->cache1_tv.tv_sec = 0 ;
	g->cache1_stime.tm_mday = 0 ;
	
	return 0;
}

/* 代码宏 */ /* code macro */
#define WRITEHEXLOGBASE(_g_,_log_level_) \
	va_list		valist; \
	int		nret ; \
	if( (_g_) == NULL ) \
		return LOG_RETURN_ERROR_PARAMETER; \
	if( (_g_)->output == LOG_OUTPUT_FILE && (_g_)->log_pathfilename[0] == '\0' ) \
		return 0; \
	if( TEST_LOGLEVEL_NOTENOUGH( _log_level_ , (_g_) ) ) \
		return 0; \
	va_start( valist , format ); \
	nret = WriteHexLogBase( (_g_) , c_filename , c_fileline , _log_level_ , buffer , buflen , format , valist ) ; \
	va_end( valist ); \
	if( nret ) \
		return nret;

/* 带日志等级的写十六进制块日志 */ /* write hex log */
int WriteHexLog( LOG *g , char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , log_level )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteHexLogG( char *c_filename , long c_fileline , int log_level , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( tls_g , log_level )
	return 0;
}
#endif

/* 写十六进制块调试日志 */ /* write debug hex log */
int WriteDebugHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_DEBUG )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteDebugHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( tls_g , LOG_LEVEL_DEBUG )
	return 0;
}
#endif

/* 写十六进制块普通信息日志 */ /* write info hex log */
int WriteInfoHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_INFO )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteInfoHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( tls_g , LOG_LEVEL_INFO )
	return 0;
}
#endif

/* 写十六进制块通知信息日志 */ /* write notice hex log */
int WriteNoticeHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_NOTICE )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteNoticeHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( tls_g , LOG_LEVEL_NOTICE )
	return 0;
}
#endif

/* 写十六进制块警告日志 */ /* write warn hex log */
int WriteWarnHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_WARN )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteWarnHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( tls_g , LOG_LEVEL_WARN )
	return 0;
}
#endif

/* 写十六进制块错误日志 */ /* write error hex log */
int WriteErrorHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_ERROR )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteErrorHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( tls_g , LOG_LEVEL_ERROR )
	return 0;
}
#endif

/* 写十六进制块致命错误日志 */ /* write fatal hex log */
int WriteFatalHexLog( LOG *g , char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( g , LOG_LEVEL_FATAL )
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int WriteFatalHexLogG( char *c_filename , long c_fileline , char *buffer , long buflen , char *format , ... )
{
	WRITEHEXLOGBASE( tls_g , LOG_LEVEL_FATAL )
	return 0;
}
#endif

/* 文件变动测试间隔 */
#define LOG_FILECHANGETEST_INTERVAL_ON_OPEN_ONCE_DEFAULT	10

/* 设置日志选项 */ /* set log options */
int SetLogOptions( LOG *g , int log_options )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->log_options = log_options ;
	
	SetLogFileChangeTest( g , LOG_FILECHANGETEST_INTERVAL_ON_OPEN_ONCE_DEFAULT );
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogOptionsG( int log_options )
{
	return SetLogOptions( tls_g , log_options );
}
#endif

/* 设置文件改变测试间隔 */
int SetLogFileChangeTest( LOG *g , long interval )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->file_change_test_interval = interval ;
	g->file_change_test_no = g->file_change_test_interval ;
	/* memset( & (g->file_change_stat) , 0x00 , sizeof(g->file_change_stat) ); */
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogFileChangeTestG( long interval )
{
	return SetLogFileChangeTest( tls_g , interval );
}
#endif

/* 刷存储IO周期 */
int SetLogFsyncPeriod( LOG *g , long period )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->fsync_period = period ;
	g->fsync_elapse = g->fsync_period ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogFsyncPeriodG( long period )
{
	return SetLogFsyncPeriod( tls_g , period );
}
#endif

/* 设置日志自定义标签 */ /* set log custom labels */
int SetLogCustLabel( LOG *g , int index , char *cust_label )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( 1 <= index && index <= LOG_MAXCNT_CUST_LABEL )
	{
		memset( g->cust_label[index-1] , 0x00 , sizeof(g->cust_label[index-1]) );
		if( cust_label && cust_label[0] != '\0' )
			strncpy( g->cust_label[index-1] , cust_label , sizeof(g->cust_label[index-1])-1 );
		return 0;
	}
	else
	{
		return LOG_RETURN_ERROR_PARAMETER;
	}
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogCustLabelG( int index , char *cust_label )
{
	return SetLogCustLabel( tls_g , index , cust_label );
}
#endif

/* 设置日志转档模式 */ /* set log rotate mode */
int SetLogRotateMode( LOG *g , int rotate_mode )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( rotate_mode == LOG_ROTATEMODE_SIZE
		|| rotate_mode == LOG_ROTATEMODE_PER_DAY
		|| rotate_mode == LOG_ROTATEMODE_PER_HOUR )
	{
		g->rotate_mode = rotate_mode ;
		return 0;
	}
	else
	{
		return LOG_RETURN_ERROR_PARAMETER;
	}
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogRotateModeG( int rotate_mode )
{
	return SetLogRotateMode( tls_g , rotate_mode );
}
#endif

/* 设置日志转档大小 */ /* set rotate size */
int SetLogRotateSize( LOG *g , long log_rotate_size )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( log_rotate_size <= 0 )
		return LOG_RETURN_ERROR_PARAMETER;
	g->log_rotate_size = log_rotate_size ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogRotateSizeG( long log_rotate_size )
{
	return SetLogRotateSize( tls_g , log_rotate_size );
}
#endif

/* 设置日志转档紧迫系数 */ /* set rotate pressure fator */
int SetLogRotatePressureFactor( LOG *g , long pressure_factor )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( pressure_factor < 0 )
		return LOG_RETURN_ERROR_PARAMETER;
	g->pressure_factor = pressure_factor ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogRotatePressureFactorG( long pressure_factor )
{
	return SetLogRotatePressureFactor( tls_g , pressure_factor );
}
#endif

/* 设置日志转档最大后缀序号 */
int SetLogRotateFileCount( LOG *g , long rotate_file_count )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	if( rotate_file_count <= 0 )
		return LOG_RETURN_ERROR_PARAMETER;
	g->rotate_file_count = rotate_file_count ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogRotateFileCountG( long rotate_file_count )
{
	return SetLogRotateFileCount( tls_g , rotate_file_count );
}
#endif

/* 设置自定义日志转档前回调函数 */ /* set custom callback function before rotate log */
int SetBeforeRotateFileFunc( LOG *g , funcBeforeRotateFile *pfuncBeforeRotateFile )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->pfuncBeforeRotateFile = pfuncBeforeRotateFile ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetBeforeRotateFileFuncG( funcBeforeRotateFile *pfuncBeforeRotateFile )
{
	return SetBeforeRotateFileFunc( tls_g , pfuncBeforeRotateFile );
}
#endif

/* 设置自定义日志转档后回调函数 */ /* set custom callback function after rotate log */
int SetAfterRotateFileFunc( LOG *g , funcAfterRotateFile *pfuncAfterRotateFile )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->pfuncAfterRotateFile = pfuncAfterRotateFile ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetAfterRotateFileFuncG( funcAfterRotateFile *pfuncAfterRotateFile )
{
	return SetAfterRotateFileFunc( tls_g , pfuncAfterRotateFile );
}
#endif

/* 设置自定义检查日志等级回调函数类型 */ /* set custom filter callback function */
int SetFilterLogFunc( LOG *g , funcFilterLog *pfuncFilterLog )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->pfuncFilterLog = pfuncFilterLog ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetFilterLogFuncG( funcFilterLog *pfuncFilterLog )
{
	return SetFilterLogFunc( tls_g , pfuncFilterLog );
}
#endif

/* 设置行日志缓冲区大小 */ /* set log buffer size */
int SetLogBufferSize( LOG *g , long log_bufsize , long max_log_bufsize )
{
	return SetBufferSize( g , & (g->logbuf) , log_bufsize , max_log_bufsize );
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogBufferSizeG( long log_bufsize , long max_log_bufsize )
{
	return SetLogBufferSize( tls_g , log_bufsize , max_log_bufsize );
}
#endif

/* 设置十六进制日志缓冲区大小 */ /* set hex log buffer size */
int SetHexLogBufferSize( LOG *g , long hexlog_bufsize , long max_hexlog_bufsize )
{
	return SetBufferSize( g , & (g->hexlogbuf) , hexlog_bufsize , max_hexlog_bufsize );
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetHexLogBufferSizeG( long hexlog_bufsize , long max_hexlog_bufsize )
{
	return SetHexLogBufferSize( tls_g , hexlog_bufsize , max_hexlog_bufsize );
}
#endif

/* 直接设置日志输出回调函数 */
int SetLogOutputFuncDirectly( LOG *g , funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->pfuncOpenLogFirst = pfuncOpenLogFirst ;
	g->pfuncOpenLog = pfuncOpenLog ;
	g->pfuncWriteLog = pfuncWriteLog ;
	g->pfuncChangeTest = pfuncChangeTest ;
	g->pfuncCloseLog = pfuncCloseLog ;
	g->pfuncCloseLogFinally = pfuncCloseLogFinally ;
	
	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogOutputFuncDirectlyG( funcOpenLog *pfuncOpenLogFirst , funcOpenLog *pfuncOpenLog , funcWriteLog *pfuncWriteLog , funcChangeTest *pfuncChangeTest , funcCloseLog *pfuncCloseLog , funcCloseLog *pfuncCloseLogFinally )
{
	return SetLogOutputFuncDirectly( tls_g , pfuncOpenLogFirst , pfuncOpenLog , pfuncWriteLog , pfuncChangeTest , pfuncCloseLog , pfuncCloseLogFinally );
}
#endif

/* 直接设置行日志风格回调函数 */
int SetLogStyleFuncDirectly( LOG *g , funcLogStyle *pfuncLogStyle )
{
	if( g == NULL )
		return LOG_RETURN_ERROR_PARAMETER;
	
	g->pfuncLogStyle = pfuncLogStyle ;

	return 0;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
int SetLogStyleFuncDirectlyG( funcLogStyle *pfuncLogStyle )
{
	return SetLogStyleFuncDirectly( tls_g , pfuncLogStyle );
}
#endif

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
/* 得到基于线程本地存储的缺省日志句柄的函数版本 */
LOG *GetGlobalLOG()
{
	return tls_g;
}
void SetGlobalLOG( LOG *g )
{
	tls_g = g ;
	return;
}
#endif

char *GetLogPathfilename( LOG *g )
{
	return g->log_pathfilename;
}

int SetOpenFlag( LOG *g , char open_flag )
{
	g->open_flag = open_flag ;
	return 0;
}

char IsLogOpened( LOG *g )
{
	return g->open_flag;
}

void *GetLogOpenHandle( LOG *g )
{
	return g->open_handle;
}

void SetLogOpenHandle( LOG *g , void *open_handle )
{
	g->open_handle = open_handle ;
	return;
}

int GetLogLevel( LOG *g )
{
	return g->log_level;
}

LOGBUF *GetLogBuffer( LOG *g )
{
	return & (g->logbuf);
}

LOGBUF *GetHexLogBuffer( LOG *g )
{
	return & (g->hexlogbuf);
}

void CleanLogBuffer( LOG *g , LOGBUF *logbuf )
{
	logbuf->buf_remain_len = logbuf->buf_size - 1 - 1 ;
	logbuf->bufptr = logbuf->bufbase ;
	return;
}

long FormatLogBuffer( LOG *g , LOGBUF *logbuf , char *format , ... )
{
	va_list		valist ;
	va_list		valist_copy ;
	long		len ;
	
	va_start( valist , format );
	va_copy( valist_copy , valist );
_REDO :
	va_copy( valist , valist_copy );
	len = VSNPRINTF( logbuf->bufptr , logbuf->buf_remain_len , format , valist ) ;
	va_end( valist );
#if ( defined _WIN32 )
	if( len == -1 )
#elif ( defined __linux__ ) || ( defined _AIX )
	if( len >= logbuf->buf_remain_len )
#endif
	{
		long		new_buf_size ;
		int		nret ;
		if( logbuf->buf_size == logbuf->max_buf_size )
		{
			logbuf->bufptr[0] = '\0' ;
			va_end( valist_copy );
			return LOG_RETURN_ERROR_ALLOC_MAX;
		}
		new_buf_size = logbuf->buf_size * 2 ;
		if( new_buf_size > logbuf->max_buf_size )
			new_buf_size = logbuf->max_buf_size ;
		nret = SetBufferSize( g , logbuf , new_buf_size , -1 );
		if( nret )
		{
			va_end( valist_copy );
			return nret;
		}
		goto _REDO;
	}
	OFFSET_BUFPTR( logbuf , len )
	va_end( valist_copy );
	return len;
}

long FormatLogBufferV( LOG *g , LOGBUF *logbuf , char *format , va_list valist )
{
	va_list		valist_copy ;
	long		len ;
	va_copy( valist_copy , valist );
_REDO :
	va_copy( valist , valist_copy );
	len = VSNPRINTF( logbuf->bufptr , logbuf->buf_remain_len , format , valist ) ;
#if ( defined _WIN32 )
	if( len == -1 )
#elif ( defined __linux__ ) || ( defined _AIX )
	if( len >= logbuf->buf_remain_len )
#endif
	{
		long		new_buf_size ;
		int		nret ;
		if( logbuf->buf_size == logbuf->max_buf_size )
		{
			logbuf->bufptr[0] = '\0' ;
			va_end( valist_copy );
			return LOG_RETURN_ERROR_ALLOC_MAX;
		}
		new_buf_size = logbuf->buf_size * 2 ;
		if( new_buf_size > logbuf->max_buf_size )
			new_buf_size = logbuf->max_buf_size ;
		nret = SetBufferSize( g , logbuf , new_buf_size , -1 );
		if( nret )
		{
			va_end( valist_copy );
			return nret;
		}
		goto _REDO;
	}
	OFFSET_BUFPTR( logbuf , len )
	va_end( valist_copy );
	return len;
}

long MemcatLogBuffer( LOG *g , LOGBUF *logbuf , char *append , long len )
{
_REDO :
	if( len > logbuf->buf_remain_len )
	{
		long		new_buf_size ;
		int		nret ;
		if( logbuf->buf_size == logbuf->max_buf_size )
			return LOG_RETURN_ERROR_ALLOC_MAX;
		new_buf_size = logbuf->buf_size * 2 ;
		if( new_buf_size > logbuf->max_buf_size )
			new_buf_size = logbuf->max_buf_size ;
		nret = SetBufferSize( g , logbuf , new_buf_size , -1 );
		if( nret )
			return nret;
		goto _REDO;
	}
	memcpy( logbuf->bufptr , append , len );
	OFFSET_BUFPTR( logbuf , len )
	logbuf->bufptr[0] = '\0' ;
	return len;
}

int ConvertLogOutput_atoi( char *output_desc )
{
	if( strcmp( output_desc , "STDOUT" ) == 0 )
		return LOG_OUTPUT_STDOUT;
	else if( strcmp( output_desc , "STDERR" ) == 0 )
		return LOG_OUTPUT_STDERR;
	else if( strcmp( output_desc , "SYSLOG" ) == 0 )
		return LOG_OUTPUT_SYSLOG;
	else if( strcmp( output_desc , "FILE" ) == 0 )
		return LOG_OUTPUT_FILE;
	else if( strcmp( output_desc , "CALLBACK" ) == 0 )
		return LOG_OUTPUT_CALLBACK;
	else
		return LOG_OUTPUT_INVALID;
}

int ConvertLogLevel_atoi( char *log_level_desc )
{
	if( strcmp( log_level_desc , "DEBUG" ) == 0 )
		return LOG_LEVEL_DEBUG;
	else if( strcmp( log_level_desc , "INFO" ) == 0 )
		return LOG_LEVEL_INFO;
	else if( strcmp( log_level_desc , "NOTICE" ) == 0 )
		return LOG_LEVEL_NOTICE;
	else if( strcmp( log_level_desc , "WARN" ) == 0 )
		return LOG_LEVEL_WARN;
	else if( strcmp( log_level_desc , "ERROR" ) == 0 )
		return LOG_LEVEL_ERROR;
	else if( strcmp( log_level_desc , "FATAL" ) == 0 )
		return LOG_LEVEL_FATAL;
	else if( strcmp( log_level_desc , "NOLOG" ) == 0 )
		return LOG_LEVEL_NOLOG;
	else
		return LOG_LEVEL_INVALID;
}

char *ConvertLogLevel_itoa( int log_level )
{
	if( LOG_LEVEL_DEBUG <= log_level && log_level <= LOG_LEVEL_NOLOG )
		return sg_aszLogLevelDesc[log_level] ;
	else
		return NULL;
}

long ConvertLogStyle_atol( char *line_style_desc )
{
	if( strcmp( line_style_desc , "DATE" ) == 0 )
		return LOG_STYLE_DATE;
	else if( strcmp( line_style_desc , "DATETIME" ) == 0 )
		return LOG_STYLE_DATETIME;
	else if( strcmp( line_style_desc , "DATETIMEMS" ) == 0 )
		return LOG_STYLE_DATETIMEMS;
	else if( strcmp( line_style_desc , "LOGLEVEL" ) == 0 )
		return LOG_STYLE_LOGLEVEL;
	else if( strcmp( line_style_desc , "PID" ) == 0 )
		return LOG_STYLE_PID;
	else if( strcmp( line_style_desc , "TID" ) == 0 )
		return LOG_STYLE_TID;
	else if( strcmp( line_style_desc , "SOURCE" ) == 0 )
		return LOG_STYLE_SOURCE;
	else if( strcmp( line_style_desc , "FORMAT" ) == 0 )
		return LOG_STYLE_FORMAT;
	else if( strcmp( line_style_desc , "NEWLINE" ) == 0 )
		return LOG_STYLE_NEWLINE;
	else if( strcmp( line_style_desc , "CUSTLABEL1" ) == 0 )
		return LOG_STYLE_CUSTLABEL1;
	else if( strcmp( line_style_desc , "CUSTLABEL2" ) == 0 )
		return LOG_STYLE_CUSTLABEL2;
	else if( strcmp( line_style_desc , "CUSTLABEL3" ) == 0 )
		return LOG_STYLE_CUSTLABEL3;
	else if( strcmp( line_style_desc , "CUSTLABEL4" ) == 0 )
		return LOG_STYLE_CUSTLABEL4;
	else if( strcmp( line_style_desc , "CUSTLABEL5" ) == 0 )
		return LOG_STYLE_CUSTLABEL5;
	else
		return LOG_STYLE_INVALID;
}

long ConvertLogOption_atol( char *log_option_desc )
{
	if( strcmp( log_option_desc , "OPEN_AND_CLOSE" ) == 0 )
		return LOG_OPTION_OPEN_AND_CLOSE;
	else if( strcmp( log_option_desc , "CHANGE_TEST" ) == 0 )
		return LOG_OPTION_CHANGE_TEST;
	else if( strcmp( log_option_desc , "OPEN_ONCE" ) == 0 )
		return LOG_OPTION_OPEN_ONCE;
	else if( strcmp( log_option_desc , "SET_OUTPUT_BY_FILENAME" ) == 0 )
		return LOG_OPTION_SET_OUTPUT_BY_FILENAME;
	else if( strcmp( log_option_desc , "FILENAME_APPEND_DOT_LOG" ) == 0 )
		return LOG_OPTION_FILENAME_APPEND_DOT_LOG;
	else
		return LOG_OPTION_INVALID;
}

int ConvertLogRotateMode_atoi( char *rotate_mode_desc )
{
	if( strcmp( rotate_mode_desc , "NONE" ) == 0 )
		return LOG_ROTATEMODE_NONE ;
	else if( strcmp( rotate_mode_desc , "SIZE" ) == 0 )
		return LOG_ROTATEMODE_SIZE ;
	else if( strcmp( rotate_mode_desc , "PER_DAY" ) == 0 )
		return LOG_ROTATEMODE_PER_DAY ;
	else if( strcmp( rotate_mode_desc , "PER_HOUR" ) == 0 )
		return LOG_ROTATEMODE_PER_HOUR ;
	else
		return LOG_ROTATEMODE_INVALID;
}

long ConvertBufferSize_atol( char *bufsize_desc )
{
	long	bufsize_desc_len ;
	
	bufsize_desc_len = strlen(bufsize_desc) ;
	
	if( bufsize_desc_len > 2 && strcmp( bufsize_desc + bufsize_desc_len - 2 , "GB" ) == 0 )
		return atol(bufsize_desc) * 1024 * 1024 * 1024 ;
	else if( bufsize_desc_len > 2 && strcmp( bufsize_desc + bufsize_desc_len - 2 , "MB" ) == 0 )
		return atol(bufsize_desc) * 1024 * 1024 ;
	else if( bufsize_desc_len > 2 && strcmp( bufsize_desc + bufsize_desc_len - 2 , "KB" ) == 0 )
		return atol(bufsize_desc) * 1024 ;
	else if( bufsize_desc_len > 1 && strcmp( bufsize_desc + bufsize_desc_len - 1 , "B" ) == 0 )
		return atol(bufsize_desc) ;
	else
		return atol(bufsize_desc) ;
}
