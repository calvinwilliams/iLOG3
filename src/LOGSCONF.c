/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#if ( defined _WIN32 )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC		_declspec(dllexport)
#endif
#elif ( defined __unix ) || ( defined __linux__ ) || ( defined __hpux )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOGSCONF.h"

extern LOG *ReadLogHandleFromConfig( FILE *fp , char *id );

_WINDLL_FUNC LOGS *ReadLogsHandleFromConfig( FILE *fp )
{
	LOGS		*gs = NULL ;
	
	char		id[ 256 + 1 ] ;
	LOG		*g = NULL ;
	
	int		nret = 0 ;
	
	gs = CreateLogsHandle();
	if( gs == NULL )
		return NULL;
	
	while( ! feof(fp) )
	{
		memset( id , 0x00 , sizeof(id) );
		g = ReadLogHandleFromConfig( fp , id ) ;
		if( g == NULL )
		{
			DestroyLogsHandle( gs );
			return NULL;
		}
		
		nret = AddLogToLogs( gs , id , g ) ;
		if( nret )
		{
			DestroyLogsHandle( gs );
			return NULL;
		}
	}
	
	return gs;
}

extern int ExpandPathFilename( char *pathfilename , long pathfilename_bufsize );

LOGS *CreateLogsHandleFromConfig( char *config_filename , char *postfix )
{
	char		config_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	FILE		*fp = NULL ;
	LOGS		*gs = NULL ;
	
	int		nret ;
	
	if( postfix )
	{
		memset( config_pathfilename , 0x00 , sizeof(config_pathfilename) );
		SNPRINTF( config_pathfilename , sizeof(config_pathfilename)-1 , "%s%s" , config_filename , postfix );
	}
	else
	{
		memset( config_pathfilename , 0x00 , sizeof(config_pathfilename) );
		SNPRINTF( config_pathfilename , sizeof(config_pathfilename)-1 , "%s" , config_filename );
	}
	
	nret = ExpandPathFilename( config_pathfilename , sizeof(config_pathfilename) ) ;
	if( nret )
		return NULL;
	
	fp = fopen( config_pathfilename , "r" ) ;
	if( fp == NULL )
		return NULL;
	
	gs = ReadLogsHandleFromConfig( fp ) ;
	if( gs == NULL )
		return NULL;
	
	fclose(fp);
	
	return gs;
}
