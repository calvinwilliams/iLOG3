#include "LOGCONF_KVFILE.h"

int test_logconf()
{
	LOG	*g = NULL ;
	char	buffer[ 64 + 1 ] = "" ;
	long	buflen = sizeof(buffer) - 1 ;
	
	g = CreateLogHandleFromKvFileG( "test_logconf_kvfile.conf" , NULL ) ;
	if( g == NULL )
	{
		printf( "从配置文件构建日志句柄失败\n" );
		return -1;
	}
	else
	{
		printf( "创建日志句柄成功\n" );
	}
	
	DEBUGLOGG( "debug LOGCONF" )
	INFOLOGG( "info LOGCONF" )
	WARNLOGG( "warn LOGCONF" )
	ERRORLOGG( "error LOGCONF" )
	FATALLOGG( "fatal LOGCONF" )
	
	DEBUGHEXLOGG( buffer , buflen , "缓冲区[%ld]" , buflen )
	INFOHEXLOGG( buffer , buflen , "缓冲区[%ld]" , buflen )
	WARNHEXLOGG( buffer , buflen , "缓冲区[%ld]" , buflen )
	ERRORHEXLOGG( buffer , buflen , "缓冲区[%ld]" , buflen )
	FATALHEXLOGG( buffer , buflen , "缓冲区[%ld]" , buflen )
	
	DestroyLogHandleG();
	printf( "销毁日志句柄\n" );
	
	return 0;
}

int main()
{
	return -test_logconf();
}
