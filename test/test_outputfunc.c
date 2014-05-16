#include <stdio.h>
#include <errno.h>

#include "LOG.h"

#define LOG_STYLES_LOG	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT )

funcOpenLog MyOpenLogFirst ;
int MyOpenLogFirst( LOG *g , char *log_pathfilename , void **open_handle )
{
	if( IsLogOpened( g ) == 1 )
		return 0;
	
	printf( "MyOpenLogFirst[%s]\n" , log_pathfilename );
	SetOpenFlag( g , 1 );
	return 0;
}

funcWriteLog MyWriteLog ;
int MyWriteLog( LOG *g , void **open_handle , int log_level , char *buf , long len , long *writelen )
{
	char	*p_log_level_desc = NULL ;
	
	if( IsLogOpened( g ) == 0 )
		return 0;
	
	ConvertLogLevel_itoa( log_level , & p_log_level_desc );
	(*writelen) = printf( "MyWriteLog[%s][%ld][%s]\n" , p_log_level_desc , len , buf ) ;
	
	return 0;
}

funcCloseLog MyCloseLogFinally ;
int MyCloseLogFinally( LOG *g , void **open_handle )
{
	if( IsLogOpened( g ) == 0 )
		return 0;
	
	printf( "MyCloseLogFinally\n" );
	SetOpenFlag( g , 0 );
	return 0;
}

int test_outputfunc()
{
	LOG		*g = NULL ;
	
	g = CreateLogHandle() ;
	if( g == NULL )
	{
		printf( "创建日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	else
	{
		printf( "创建日志句柄成功\n" );
	}
	
	SetLogOutput( g , LOG_OUTPUT_CALLBACK , "127.0.0.1:514" , & MyOpenLogFirst , NULL , & MyWriteLog , NULL , NULL , & MyCloseLogFinally );
	SetLogLevel( g , LOG_LEVEL_INFO );
	SetLogStyles( g , LOG_STYLES_LOG , LOG_NO_STYLEFUNC );
	
	DebugLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	InfoLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	WarnLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	ErrorLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	FatalLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	
	DestroyLogHandle( g );
	printf( "销毁日志句柄\n" );
	
	return 0;
}

int main()
{
	return -test_outputfunc();
}
