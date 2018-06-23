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
#elif ( defined __unix ) || ( defined _AIX ) || ( defined __linux__ ) || ( defined __hpux )
#ifndef _WINDLL_FUNC
#define _WINDLL_FUNC
#endif
#endif

#include "LOGCONF.h"

_WINDLL_FUNC LOG *ReadLogHandleFromConfig( FILE *fp , char *id )
{
	LOG		*g = NULL ;
	
	char		filebuffer[ 1024 + 1 ] ;
	char		key[ 64 + 1 ] ;
	char		value[ 512 + 1 ] ;
	char		value2[ 512 + 1 ] ;
	
	long		option , options ;
	int		output ;
	int		level ;
	long		style , styles ;
	int		log_rotate_mode ;
	long		log_rotate_size ;
	long		log_bufsize , max_log_bufsize ;
	long		hexlog_bufsize , max_hexlog_bufsize ;
	
	char		*pc = NULL ;
	
	int		nret ;
	
	g = CreateLogHandle() ;
	if( g == NULL )
		return NULL;
	
	while(1)
	{
		/* 读入一行并分解之 */ /* read a row and decomposition */
		memset( filebuffer , 0x00 , sizeof(filebuffer) );
		if( fgets( filebuffer , sizeof(filebuffer)-1 , fp ) == NULL )
			break;
		
		memset( key , 0x00 , sizeof(key) );
		memset( value , 0x00 , sizeof(value) );
		memset( value2 , 0x00 , sizeof(value2) );
		sscanf( filebuffer , "%s %s %[^\n]" , key , value , value2 );
		if( key[0] == '\0' )
			break;
		
		if( value2[0] == '\"' )
		{
			long	len2 ;
			len2 = strlen(value2) ;
			if( value2[len2-1] == '\"' ) /* 如果最后为引号，去掉之 */ /* by found */
				value2[len2-1] = '\0' ;
			memmove( value2 , value2 + 1 , len2 - 1 );
		}
		
		if( STRCMP( key , == , "id" ) )
		{
			if( id )
				strcpy( id , value );
		}
		else if( STRCMP( key , == , "options" ) )
		{
			options = 0 ;
			pc = strtok( value , "|" ) ;
			while( pc )
			{
				nret = ConvertLogOption_atol( pc , & option ) ;
				if( nret )
					goto ERR;
				
				options |= option ;
				
				pc = strtok( NULL , "|" ) ;
			}
			
			nret = SetLogOptions( g , options ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "output" ) )
		{
			nret = ConvertLogOutput_atoi( value , & output ) ;
			if( nret )
				goto ERR;
			
			nret = SetLogOutput( g , output , value2 , LOG_NO_OUTPUTFUNC ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "level" ) )
		{
			nret = ConvertLogLevel_atoi( value , & level ) ;
			if( nret )
				goto ERR;
			
			nret = SetLogLevel( g , level ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "styles" ) )
		{
			styles = 0 ;
			pc = strtok( value , "|" ) ;
			while( pc )
			{
				nret = ConvertLogStyle_atol( pc , & style ) ;
				if( nret )
					goto ERR;
				
				styles |= style ;
				
				pc = strtok( NULL , "|" ) ;
			}
			
			nret = SetLogStyles( g , styles , LOG_NO_STYLEFUNC ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "custlabel1" ) )
		{
			nret = SetLogCustLabel( g , 1 , value ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "custlabel2" ) )
		{
			nret = SetLogCustLabel( g , 2 , value ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "custlabel3" ) )
		{
			nret = SetLogCustLabel( g , 3 , value ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "rotate_mode" ) )
		{
			nret = ConvertLogRotateMode_atoi( value , & log_rotate_mode ) ;
			if( nret )
				goto ERR;
			
			nret = SetLogRotateMode( g , log_rotate_mode ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "rotate_size" ) )
		{
			nret = ConvertBufferSize_atol( value , & log_rotate_size ) ;
			if( nret )
				goto ERR;
			
			nret = SetLogRotateSize( g , log_rotate_size ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "rotate_pressure_factor" ) )
		{
			nret = SetLogRotatePressureFactor( g , atol(value) ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "log_bufsize" ) )
		{
			if( STRCMP( value , != , "" ) )
			{
				nret = ConvertBufferSize_atol( value , & log_bufsize ) ;
				if( nret )
					goto ERR;
			}
			else
			{
				log_bufsize = -1 ;
			}
			
			if( STRCMP( value2 , != , "" ) )
			{
				nret = ConvertBufferSize_atol( value2 , & max_log_bufsize ) ;
				if( nret )
					goto ERR;
			}
			else
			{
				max_log_bufsize = -1 ;
			}
			
			nret = SetLogBufferSize( g , log_bufsize , max_log_bufsize ) ;
			if( nret )
				goto ERR;
		}
		else if( STRCMP( key , == , "hexlog_bufsize" ) )
		{
			if( STRCMP( value , != , "" ) )
			{
				nret = ConvertBufferSize_atol( value , & hexlog_bufsize ) ;
				if( nret )
					goto ERR;
			}
			else
			{
				hexlog_bufsize = -1 ;
			}
			
			if( STRCMP( value2 , != , "" ) )
			{
				nret = ConvertBufferSize_atol( value2 , & max_hexlog_bufsize ) ;
				if( nret )
					goto ERR;
			}
			else
			{
				max_hexlog_bufsize = -1 ;
			}
			
			nret = SetHexLogBufferSize( g , hexlog_bufsize , max_hexlog_bufsize ) ;
			if( nret )
				goto ERR;
		}
	}
	
	return g;
	
ERR :
	
	DestroyLogHandle( g );
	
	return NULL;
}

extern int ExpandPathFilename( char *pathfilename , long pathfilename_bufsize );

LOG *CreateLogHandleFromConfig( char *config_filename , char *postfix )
{
	char		config_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	FILE		*fp = NULL ;
	LOG		*g = NULL ;
	
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
	
	g = ReadLogHandleFromConfig( fp , NULL ) ;
	if( g == NULL )
		return NULL;
	
	fclose(fp);
	
	return g;
}
