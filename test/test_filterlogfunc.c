#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

funcFilterLog FilterLog ;
int FilterLog( LOG *g , void **open_handle , int log_level , char *buf , long len )
{
	if( log_level == LOG_LEVEL_ERROR )
		return 0;
	
	if( strstr( buf , "FATAL" ) )
		return 0;
	
	return -1;
}

#define LOG_STYLES_FILTERLOG	( LOG_STYLE_DATETIME | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_filterlog()
{
	if( CreateLogHandleG() == NULL )
	{
		printf( "创建日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	else
	{
		printf( "创建日志句柄成功\n" );
	}
	
	SetLogOutputG( LOG_OUTPUT_FILE , "test_filterlog.log" , LOG_NO_OUTPUTFUNC );
	SetLogLevelG( LOG_LEVEL_INFO );
	SetLogStylesG( LOG_STYLES_FILTERLOG , LOG_NO_STYLEFUNC );
	SetFilterLogFuncG( & FilterLog );
	
	DebugLogG( __FILE__ , __LINE__ , "hello DEBUG" );
	InfoLogG( __FILE__ , __LINE__ , "hello INFO" );
	WarnLogG( __FILE__ , __LINE__ , "hello WARN" );
	ErrorLogG( __FILE__ , __LINE__ , "hello ERROR" );
	FatalLogG( __FILE__ , __LINE__ , "hello FATAL" );
	
	DestroyLogHandleG();
	printf( "销毁句柄环境\n" );
	
	return 0;
}

int main()
{
	return -test_filterlog();
}
