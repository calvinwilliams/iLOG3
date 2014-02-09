#include <stdio.h>
#include <errno.h>

#include "LOGS.h"

#define LOG_STYLES_HELLO	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_logs()
{
	LOGS		*gs = NULL ;
	LOG		*g = NULL ;
	
	long		index ;
	char		*g_id = NULL ;
	
	char		buffer[ 64 + 1 ] = "" ;
	long		buflen = sizeof(buffer) - 1 ;
	
	int		nret ;
	
	gs = CreateLogsHandle() ;
	if( gs == NULL )
	{
		printf( "创建日志句柄集合失败errno[%d]\n" , errno );
		return -1;
	}
	
	g = CreateLogHandle() ;
	if( g == NULL )
	{
		printf( "创建日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	
	SetLogOutput( g , LOG_OUTPUT_FILE , "test_logs.log" , LOG_NO_OUTPUTFUNC );
	SetLogLevel( g , LOG_LEVEL_INFO );
	SetLogStyles( g , LOG_STYLES_HELLO , LOG_NO_STYLEFUNC );
	
	AddLogToLogs( gs , "FILE" , g );
	
	AddLogToLogs( gs , "STDERR" , CreateLogHandle() );
	
	g = GetLogFromLogs( gs , "STDERR" ) ;
	if( g == NULL )
	{
		printf( "得到日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	
	SetLogOutput( g , LOG_OUTPUT_STDERR , "" , LOG_NO_OUTPUTFUNC );
	SetLogLevel( g , LOG_LEVEL_ERROR );
	SetLogStyles( g , LOG_STYLES_HELLO , LOG_NO_STYLEFUNC );
	
	printf( "创建日志句柄集合成功\n" );
	
	for( index = LOG_TRAVELLOG_INDEX_INIT ; ; index++ )
	{
		nret = TravelLogFromLogs( gs , & index , & g_id , & g ) ;
		if( nret )
			break;
		
		printf( "LOG id[%s]\n" , g_id );
	}
	
	printf( "遍历日志句柄集合完成\n" );
	
	DebugLogs( gs , __FILE__ , __LINE__ , "hello iLOG3" );
	InfoLogs( gs , __FILE__ , __LINE__ , "hello iLOG3" );
	WarnLogs( gs , __FILE__ , __LINE__ , "hello iLOG3" );
	ErrorLogs( gs , __FILE__ , __LINE__ , "hello iLOG3" );
	FatalLogs( gs , __FILE__ , __LINE__ , "hello iLOG3" );
	
	DebugHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	InfoHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	WarnHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	ErrorHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	FatalHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	
	DestroyLogHandle( RemoveOutLogFromLogs( gs , "FILE" ) );
	printf( "销毁日志句柄\n" );
	
	DestroyLogsHandle( gs );
	printf( "销毁日志句柄集合\n" );
	
	return 0;
}

int main()
{
	return -test_logs();
}
