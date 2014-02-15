#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#elif defined(__linux__) || defined(__unix)
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include "LOG.h"

funcBeforeRotateFile BeforeRotateFile ;
int BeforeRotateFile( LOG *g , char *rotate_log_pathfilename )
{
	strcat( rotate_log_pathfilename , ".rar" );
	return 0;
}

funcAfterRotateFile AfterRotateFile ;
int AfterRotateFile( LOG *g , char *rotate_log_pathfilename )
{
	char	cmd[ 256 + 1 ] ;
	
	memset( cmd , 0x00 , sizeof(cmd) );
	SNPRINTF( cmd , sizeof(cmd)-1 , "Rar.exe m %s.rar %s" , rotate_log_pathfilename , rotate_log_pathfilename );
	system( cmd );
	
	return 0;
}

#define LOG_STYLES_PRESS	( LOG_STYLE_DATETIME | LOG_STYLE_FORMAT | LOG_STYLE_NEWLINE )

int test_afterrotatefile()
{
	long		l ;
	
	if( CreateLogHandleG() == NULL )
	{
		printf( "创建press日志句柄失败errno[%d]\n" , errno );
		return -1;
	}
	else
	{
		printf( "创建press日志句柄成功\n" );
	}
	
	SetLogOutputG( LOG_OUTPUT_FILE , "test_afterrotatefile.log" , LOG_NO_OUTPUTFUNC );
	SetLogLevelG( LOG_LEVEL_INFO );
	SetLogStylesG( LOG_STYLES_PRESS , LOG_NO_STYLEFUNC );
	SetLogRotateModeG( LOG_ROTATEMODE_SIZE );
	SetLogRotateSizeG( 100*1024 );
	SetBeforeRotateFileFuncG( & BeforeRotateFile );
	SetAfterRotateFileFuncG( & AfterRotateFile );
	
	for( l = 1 ; l <= 10000 ; l++ )
	{
		InfoLogG( __FILE__ , __LINE__ , "log" );
	}
	
	DestroyLogHandleG();
	printf( "销毁句柄环境\n" );
	
	return 0;
}

int main()
{
	return -test_afterrotatefile();
}
