#include "LOGSCONF.h"

int test_logsconf( char *program )
{
	LOGS	*gs = NULL ;
	char	buffer[ 64 + 1 ] = "" ;
	long	buflen = sizeof(buffer) - 1 ;
	
#if ( defined _WIN32 )
	if( strstr( program , ".exe" ) )
	{
		strstr( program , ".exe" )[0] = '\0' ;
	}
#endif
	gs = CreateLogsHandleFromConfig( program , ".conf" ) ;
	/*
	if( gs == NULL )
	{
		printf( "从配置文件构建日志句柄集合失败\n" );
		return -1;
	}
	*/
	printf( "创建日志句柄集合成功\n" );
	
	DebugLogs( gs , __FILE__ , __LINE__ , "hello LOGSCONF" );
	InfoLogs( gs , __FILE__ , __LINE__ , "hello LOGSCONF" );
	WarnLogs( gs , __FILE__ , __LINE__ , "hello LOGSCONF" );
	ErrorLogs( gs , __FILE__ , __LINE__ , "hello LOGSCONF" );
	FatalLogs( gs , __FILE__ , __LINE__ , "hello LOGSCONF" );
	
	DebugHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	InfoHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	WarnHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	ErrorHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	FatalHexLogs( gs , __FILE__ , __LINE__ , buffer , buflen , "缓冲区[%ld]" , buflen );
	
	DestroyLogsHandle( gs );
	printf( "销毁日志句柄集合\n" );
	
	return 0;
}

int main( int argc , char *argv[] )
{
	return -test_logsconf( argv[0] );
}
