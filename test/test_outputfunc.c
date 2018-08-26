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
	
	p_log_level_desc = ConvertLogLevel_itoa( log_level );
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
	
	WriteDebugLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	WriteInfoLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	WriteWarnLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	WriteErrorLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	WriteFatalLog( g , __FILE__ , __LINE__ , "hello iLOG3\n" );
	
	DestroyLogHandle( g );
	printf( "销毁日志句柄\n" );
	
	return 0;
}

int main()
{
	return -test_outputfunc();
}
