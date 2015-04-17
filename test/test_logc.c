#include "../src/LOGC.h"

int test_logc()
{
	char	buf[ 4096*10 + 1 + 1 ] ;
	long	buflen ;
	int	i ;
	
	SetLogFile( "%s/log/test_logc.log" , getenv("HOME") );
	SetLogLevel( LOGLEVEL_INFO );
	
	DebugLog( __FILE__ , __LINE__ , "call DebugLog" );
	InfoLog( __FILE__ , __LINE__ , "call InfoLog" );
	WarnLog( __FILE__ , __LINE__ , "call WarnLog" );
	ErrorLog( __FILE__ , __LINE__ , "call ErrorLog" );
	FatalLog( __FILE__ , __LINE__ , "call FatalLog" );
	
	memset( buf , 0x00 , sizeof(buf) );
	DebugHexLog( __FILE__ , __LINE__ , buf , 64 , "call DebugHexLog" );
	InfoHexLog( __FILE__ , __LINE__ , buf , 64 , "call InfoHexLog" );
	WarnHexLog( __FILE__ , __LINE__ , buf , 64 , "call WarnHexLog" );
	ErrorHexLog( __FILE__ , __LINE__ , buf , 64 , "call ErrorHexLog" );
	FatalHexLog( __FILE__ , __LINE__ , buf , 64 , "call FatalHexLog" );
	
	SetLogLevel( LOGLEVEL_DEBUG );
	
	buflen = 4096*10 ;
	DebugHexLog( __FILE__ , __LINE__ , buf , buflen , "call DebugHexLog" );
	InfoLog( __FILE__ , __LINE__ , "call InfoHexLog ok" );
	
	for( i = 0 ; i <= 255 ; i++ )
	{
		buf[i] = i ;
	}
	
	DebugHexLog( __FILE__ , __LINE__ , buf , 256 , "call DebugHexLog" );
	InfoLog( __FILE__ , __LINE__ , "call InfoHexLog ok" );
	
	return 0;
}

int main()
{
	return -test_logc();
}
