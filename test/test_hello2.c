#include <stdio.h>
#include <errno.h>

#include "LOG.h"

#define LOG_STYLES_HELLO	( LOG_STYLE_DATETIMEMS | LOG_STYLE_LOGLEVEL | LOG_STYLE_PID | LOG_STYLE_TID | LOG_STYLE_SOURCE | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_hello()
{
	LOG		*g = NULL ;
	
	char		buffer[ 64 + 1 ] = "" ;
	long		buflen = sizeof(buffer) - 1 ;
	
	g = create_log_handle() ;
	if( g == NULL )
	{
		printf( "create log handle failed , errno[%d]\n" , errno );
		return -1;
	}
	
	printf( "create log handle ok\n" );
	
	set_log_output( g , LOG_OUTPUT_FILE , "$HOME$/log/test_hello.log" , LOG_NO_OUTPUTFUNC );
	set_log_level( g , LOG_LEVEL_INFO );
	set_log_styles( g , LOG_STYLES_HELLO , LOG_NO_STYLEFUNC );
	
	debug_log( g , __FILE__ , __LINE__ , "hello DEBUG" );
	info_log( g , __FILE__ , __LINE__ , "hello INFO" );
	warn_log( g , __FILE__ , __LINE__ , "hello WARN" );
	error_log( g , __FILE__ , __LINE__ , "hello ERROR" );
	fatal_log( g , __FILE__ , __LINE__ , "hello FATAL" );
	
	debug_hex_log( g , __FILE__ , __LINE__ , buffer , buflen , "buflen[%ld]" , buflen );
	info_hex_log( g , __FILE__ , __LINE__ , buffer , buflen , "buflen[%ld]" , buflen );
	warn_hex_log( g , __FILE__ , __LINE__ , buffer , buflen , "buflen[%ld]" , buflen );
	error_hex_log( g , __FILE__ , __LINE__ , buffer , buflen , "buflen[%ld]" , buflen );
	fatal_hex_log( g , __FILE__ , __LINE__ , buffer , buflen , "buflen[%ld]" , buflen );
	
	destroy_log_handle( g );
	printf( "destroy log handle\n" );
	
	return 0;
}

int main()
{
	return -test_hello();
}
