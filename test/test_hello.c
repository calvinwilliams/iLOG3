#include <stdio.h>
#include <errno.h>

#include "LOG.h"

#define LOG_STYLES_HELLO	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_hello()
{
	LOG		*g = NULL ;
	
	char		buffer[ 64 + 1 ] = "" ;
	long		buflen = sizeof(buffer) - 1 ;
	
	g = CreateLogHandle() ;
	if( g == NULL )
	{
		printf( "创建日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	
	printf( "创建日志句柄成功\n" );
	
	SetLogOutput( g , LOG_OUTPUT_FILE , "test_hello.log" , LOG_NO_OUTPUTFUNC );
	SetLogLevel( g , LOG_LEVEL_INFO );
	SetLogStyles( g , LOG_STYLES_HELLO , LOG_NO_STYLEFUNC );
	
	DebugLog( g , __FILE__ , __LINE__ , "hello DEBUG" );
	InfoLog( g , __FILE__ , __LINE__ , "hello INFO" );
	WarnLog( g , __FILE__ , __LINE__ , "hello WARN" );
	ErrorLog( g , __FILE__ , __LINE__ , "hello ERROR" );
	FatalLog( g , __FILE__ , __LINE__ , "hello FATAL" );
	
	DebugHexLog( g , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	InfoHexLog( g , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	WarnHexLog( g , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	ErrorHexLog( g , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	FatalHexLog( g , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	
	DestroyLogHandle( g );
	printf( "销毁日志句柄\n" );
	
	return 0;
}

int main()
{
	return -test_hello();
}
