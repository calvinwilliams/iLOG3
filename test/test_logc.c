#include "../src/LOGC.h"

int test_logc()
{
	char	buf[ 4096*10 + 1 + 1 ] ;
	long	buflen ;
	int	i ;
	
	SetLogcFile( "%s/log/test_logc.log" , getenv("HOME") );
	SetLogcLevel( LOGCLEVEL_INFO );
	
	WriteDebugLogc( __FILE__ , __LINE__ , "call DebugLog" );
	WriteInfoLogc( __FILE__ , __LINE__ , "call InfoLog" );
	WriteWarnLogc( __FILE__ , __LINE__ , "call WarnLog" );
	WriteErrorLogc( __FILE__ , __LINE__ , "call ErrorLog" );
	WriteFatalLogc( __FILE__ , __LINE__ , "call FatalLog" );
	
	memset( buf , 0x00 , sizeof(buf) );
	WriteDebugHexLogc( __FILE__ , __LINE__ , buf , 64 , "call DebugHexLog" );
	WriteInfoHexLogc( __FILE__ , __LINE__ , buf , 64 , "call InfoHexLog" );
	WriteWarnHexLogc( __FILE__ , __LINE__ , buf , 64 , "call WarnHexLog" );
	WriteErrorHexLogc( __FILE__ , __LINE__ , buf , 64 , "call ErrorHexLog" );
	WriteFatalHexLogc( __FILE__ , __LINE__ , buf , 64 , "call FatalHexLog" );
	
	SetLogcLevel( LOGCLEVEL_DEBUG );
	
	buflen = 4096*10 ;
	WriteDebugHexLogc( __FILE__ , __LINE__ , buf , buflen , "call DebugHexLog" );
	WriteInfoLogc( __FILE__ , __LINE__ , "call InfoHexLog ok" );
	
	for( i = 0 ; i <= 255 ; i++ )
	{
		buf[i] = i ;
	}
	
	WriteDebugHexLogc( __FILE__ , __LINE__ , buf , 256 , "call DebugHexLog" );
	WriteInfoLogc( __FILE__ , __LINE__ , "call InfoHexLog ok" );
	
	return 0;
}

int main()
{
	return -test_logc();
}
