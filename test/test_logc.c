#include "../src/LOGC.h"

int test_logc()
{
	char	buf[ 64 + 1 ] ;
	long	buflen ;
	
	SetLogFile( "%s/log/test_logc.log" , getenv("HOME") );
	SetLogLevel( LOGLEVEL_INFO );
	
	DebugLog( __FILE__ , __LINE__ , "call DebugLog" );
	InfoLog( __FILE__ , __LINE__ , "call InfoLog" );
	WarnLog( __FILE__ , __LINE__ , "call WarnLog" );
	ErrorLog( __FILE__ , __LINE__ , "call ErrorLog" );
	FatalLog( __FILE__ , __LINE__ , "call FatalLog" );
	
	memset( buf , 0x00 , sizeof(buf) );
	buflen = sizeof(buf) - 1 ;
	DebugHexLog( __FILE__ , __LINE__ , buf , buflen , "call DebugHexLog" );
	InfoHexLog( __FILE__ , __LINE__ , buf , buflen , "call InfoHexLog" );
	WarnHexLog( __FILE__ , __LINE__ , buf , buflen , "call WarnHexLog" );
	ErrorHexLog( __FILE__ , __LINE__ , buf , buflen , "call ErrorHexLog" );
	FatalHexLog( __FILE__ , __LINE__ , buf , buflen , "call FatalHexLog" );
	
	return 0;
}

int main()
{
	return -test_logc();
}
