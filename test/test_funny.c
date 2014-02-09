#include <stdio.h>
#include <errno.h>

#include "LOG.h"

#define LOG_STYLES_FUNNY1	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )
#define LOG_STYLES_FUNNY2	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_funny( char *program_name )
{
	LOG		*g = NULL , *g2 = NULL ;
	
	g = CreateLogHandle() ;
	if( g == NULL )
	{
		printf( "创建日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	
	SetLogOptions( g , LOG_OPTION_FILENAME_APPEND_DOT_LOG );
	SetLogOutput( g , LOG_OUTPUT_FILE , program_name , LOG_NO_OUTPUTFUNC );
	SetLogLevel( g , LOG_LEVEL_INFO );
	SetLogStyles( g , LOG_STYLES_FUNNY1 , LOG_NO_STYLEFUNC );
	
	g2 = CreateLogHandle() ;
	if( g2 == NULL )
	{
		printf( "创建日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	
	SetLogOptions( g2 , LOG_OPTION_SET_OUTPUT_BY_FILENAME );
	SetLogOutput( g2 , LOG_OUTPUT_FILE , "$TEST_FUNNY_LOG_OUTPUT$" , LOG_NO_OUTPUTFUNC );
	SetLogLevel( g2 , LOG_LEVEL_INFO );
	SetLogStyles( g2 , LOG_STYLES_FUNNY2 , LOG_NO_STYLEFUNC );
	
	printf( "创建日志句柄成功\n" );
	
	DebugLog( g , __FILE__ , __LINE__ , "hello iLOG3" );
	InfoLog( g , __FILE__ , __LINE__ , "hello iLOG3" );
	WarnLog( g , __FILE__ , __LINE__ , "hello iLOG3" );
	ErrorLog( g , __FILE__ , __LINE__ , "hello iLOG3" );
	FatalLog( g , __FILE__ , __LINE__ , "hello iLOG3" );
	
	DebugLog( g2 , __FILE__ , __LINE__ , "hello iLOG3" );
	InfoLog( g2 , __FILE__ , __LINE__ , "hello iLOG3" );
	WarnLog( g2 , __FILE__ , __LINE__ , "hello iLOG3" );
	ErrorLog( g2 , __FILE__ , __LINE__ , "hello iLOG3" );
	FatalLog( g2 , __FILE__ , __LINE__ , "hello iLOG3" );
	
	DestroyLogHandle( g );
	DestroyLogHandle( g2 );
	printf( "销毁日志句柄\n" );
	
	return 0;
}

int main( int argc , char *argv[] )
{
	return -test_funny( argv[0] );
}
