/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "LOGSCONF_KVFILE.h"

static int _CreateLogsHandleFromKvFile( LOGS *gs , char *config_filename , char *file_ext_name )
{
	char		config_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	FILE		*fp = NULL ;
	LOG		*g = NULL ;
	char		id[ LOGS_MAXLEN_ID + 1 ] ;
	
	int		nret ;
	
	if( file_ext_name )
	{
		memset( config_pathfilename , 0x00 , sizeof(config_pathfilename) );
		SNPRINTF( config_pathfilename , sizeof(config_pathfilename)-1 , "%s%s" , config_filename , file_ext_name );
	}
	else
	{
		memset( config_pathfilename , 0x00 , sizeof(config_pathfilename) );
		SNPRINTF( config_pathfilename , sizeof(config_pathfilename)-1 , "%s" , config_filename );
	}
	
	nret = ExpandPathFilename( config_pathfilename , sizeof(config_pathfilename) ) ;
	if( nret )
		return nret;
	
printf( "config_pathfilename[%s]\n" , config_pathfilename );
	fp = fopen( config_pathfilename , "r" ) ;
	if( fp == NULL )
		return LOG_RETURN_ERROR_CONFIGFILE_NOTFOUND;
	
	while( ! feof(fp) )
	{
		g = CreateLogHandle() ;
		if( g == NULL )
			return LOG_RETURN_ERROR_ALLOC;
		
		memset( id , 0x00 , sizeof(id) );
		nret = ReadKvFileForLogHandle( g , fp , id , sizeof(id) ) ;
printf( "nret[%d]\n" , nret );
		if( nret )
		{
			DestroyLogHandle( g );
			return nret;
		}
		
printf( "id[%s]\n" , id );
		nret = AddLogToLogs( gs , id , g ) ;
		if( nret )
		{
			DestroyLogHandle( g );
			return nret;
		}
	}
	
	fclose(fp);
	
	return 0;
}

LOGS *CreateLogsHandleFromKvFile( char *config_filename , char *file_ext_name )
{
	LOGS		*gs = NULL ;
	
	int		nret ;
	
	gs = CreateLogsHandle() ;
	if( gs == NULL )
		return NULL;
	
	nret = _CreateLogsHandleFromKvFile( gs , config_filename , file_ext_name ) ;
	if( nret )
	{
		DestroyLogsHandle( gs );
		return NULL;
	}
	
	return gs;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOGS *CreateLogsHandleFromKvFileG( char *config_filename , char *file_ext_name )
{
	LOGS		*gs = NULL ;
	
	int		nret ;
	
printf( "111\n" );
	gs = CreateLogsHandleG() ;
	if( gs == NULL )
		return NULL;
	
printf( "222\n" );
	nret = _CreateLogsHandleFromKvFile( gs , config_filename , file_ext_name ) ;
printf( "nret[%d]\n" , nret );
	if( nret )
	{
		DestroyLogsHandleG( gs );
		return NULL;
	}
	
	return gs;
}
#endif
