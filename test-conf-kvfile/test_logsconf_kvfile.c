#include "LOGSCONF_KVFILE.h"

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
	
	gs = CreateLogsHandleFromKvFileG( program , ".conf" ) ;
	if( gs == NULL )
	{
		printf( "从配置文件构建日志句柄集合失败\n" );
		return -1;
	}
	else
	{
		printf( "创建日志句柄集合成功\n" );
	}
	
	DEBUGLOGSG( "debug LOGSCONF" );
	INFOLOGSG( "info LOGSCONF" );
	WARNLOGSG( "warn LOGSCONF" );
	ERRORLOGSG( "error LOGSCONF" );
	FATALLOGSG( "fatal LOGSCONF" );
	
	DEBUGHEXLOGSG( buffer , buflen , "缓冲区[%ld]" , buflen );
	INFOHEXLOGSG( buffer , buflen , "缓冲区[%ld]" , buflen );
	WARNHEXLOGSG( buffer , buflen , "缓冲区[%ld]" , buflen );
	ERRORHEXLOGSG( buffer , buflen , "缓冲区[%ld]" , buflen );
	FATALHEXLOGSG( buffer , buflen , "缓冲区[%ld]" , buflen );
	
	DestroyLogsHandleG( gs );
	printf( "销毁日志句柄集合\n" );
	
	return 0;
}

int main( int argc , char *argv[] )
{
	return -test_logsconf( argv[0] );
}
