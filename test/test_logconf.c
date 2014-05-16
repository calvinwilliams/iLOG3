#include "LOGCONF.h"

int test_logconf()
{
	LOG	*g = NULL ;
	char	buffer[ 64 + 1 ] = "" ;
	long	buflen = sizeof(buffer) - 1 ;
	
	g = CreateLogHandleFromConfig( "$HOME$/etc/test_logconf.conf" , NULL ) ;
	if( g == NULL )
	{
		printf( "从配置文件构建日志句柄失败\n" );
		return -1;
	}
	
	/*
	SetLogOutput( g , LOG_OUTPUT_NOSET , "$HOME$/log/test_logconf_1.log" , LOG_NO_OUTPUTFUNC );
	*/
	
	printf( "创建日志句柄成功\n" );
	
	DebugLog( g , __FILE__ , __LINE__ , "hello LOGCONF" );
	InfoLog( g , __FILE__ , __LINE__ , "hello LOGCONF" );
	WarnLog( g , __FILE__ , __LINE__ , "hello LOGCONF" );
	ErrorLog( g , __FILE__ , __LINE__ , "hello LOGCONF" );
	FatalLog( g , __FILE__ , __LINE__ , "hello LOGCONF" );
	
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
	return -test_logconf();
}
