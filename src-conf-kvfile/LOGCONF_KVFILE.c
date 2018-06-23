/*
 * iLOG3 - log function library written in c
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "LOGCONF_KVFILE.h"

int ReadKvFileForLogHandle( LOG *g , FILE *fp , char *id , int id_bufsize )
{
	char		filebuffer[ 1024 + 1 ] ;
	char		key[ 64 + 1 ] ;
	char		value[ 512 + 1 ] = "" ;
	int		value_len = 0 ;
	char		value2[ 512 + 1 ] = "" ;
	int		value2_len = 0 ;
	char		*p_value = NULL ;
	char		*p_value2 = NULL ;
	
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
		if( key[0] == '#' )
			continue;
		
		value_len = strlen(value) ;
		value2_len = strlen(value2) ;
		
		while( value_len > 0 && ( value[value_len-1] == '\r' || value[value_len-1] == '\n' ) )
		{
			value[value_len-1] = '\0' ;
			value_len--;
		}
		
		if( value[0] == '\"' )
		{
			p_value = value + 1 ;
			value_len -= 2 ;
			p_value[value_len] = '\0' ;
		}
		else
		{
			p_value = value ;
		}
		
		while( value2_len > 0 && ( value2[value2_len-1] == '\r' || value2[value2_len-1] == '\n' ) )
		{
			value2[value2_len-1] = '\0' ;
			value2_len--;
		}
		
		if( value2[0] == '\"' )
		{
			p_value2 = value2 + 1 ;
			value2_len -= 2 ;
			p_value2[value2_len] = '\0' ;
		}
		else
		{
			p_value2 = value2 ;
		}
		
		/* LOGS id */
		if( STRCMP( key , == , "id" ) )
		{
			if( id )
				strncpy( id , p_value , id_bufsize-1 );
		}
		/* 日志选项 */
		else if( STRCMP( key , == , "options" ) )
		{
			options = 0 ;
			pc = strtok( p_value , "|" ) ;
			while( pc )
			{
				option = ConvertLogOption_atol( pc ) ;
				if( option == LOG_OPTION_INVALID )
					return (int)option;
				
				options |= option ;
				
				pc = strtok( NULL , "|" ) ;
			}
			
			nret = SetLogOptions( g , options ) ;
			if( nret )
				return nret;
		}
		/* 日志输出 */
		else if( STRCMP( key , == , "output" ) )
		{
			output = ConvertLogOutput_atoi( p_value ) ;
			if( output == LOG_OUTPUT_INVALID )
				return (int)output;
			
			nret = SetLogOutput( g , output , p_value2 , LOG_NO_OUTPUTFUNC ) ;
			if( nret )
				return nret;
		}
		/* 日志等级 */
		else if( STRCMP( key , == , "level" ) )
		{
			level = ConvertLogLevel_atoi( p_value ) ;
			if( level == LOG_LEVEL_INVALID )
				return (int)level;
			
			nret = SetLogLevel( g , level ) ;
			if( nret )
				return nret;
		}
		/* 日志风格组合 */
		else if( STRCMP( key , == , "styles" ) )
		{
			styles = 0 ;
			pc = strtok( p_value , "|" ) ;
			while( pc )
			{
				style = ConvertLogStyle_atol( pc ) ;
				if( style == LOG_STYLE_INVALID )
					return (int)style;
				
				styles |= style ;
				
				pc = strtok( NULL , "|" ) ;
			}
			
			nret = SetLogStyles( g , styles , LOG_NO_STYLEFUNC ) ;
			if( nret )
				return nret;
		}
		/* 自定义标签1 */
		else if( STRCMP( key , == , "custlabel1" ) )
		{
			nret = SetLogCustLabel( g , 1 , p_value ) ;
			if( nret )
				return nret;
		}
		/* 自定义标签2 */
		else if( STRCMP( key , == , "custlabel2" ) )
		{
			nret = SetLogCustLabel( g , 2 , p_value ) ;
			if( nret )
				return nret;
		}
		/* 自定义标签3 */
		else if( STRCMP( key , == , "custlabel3" ) )
		{
			nret = SetLogCustLabel( g , 3 , p_value ) ;
			if( nret )
				return nret;
		}
		/* 自定义标签4 */
		else if( STRCMP( key , == , "custlabel4" ) )
		{
			nret = SetLogCustLabel( g , 4 , p_value ) ;
			if( nret )
				return nret;
		}
		/* 自定义标签5 */
		else if( STRCMP( key , == , "custlabel5" ) )
		{
			nret = SetLogCustLabel( g , 5 , p_value ) ;
			if( nret )
				return nret;
		}
		/* 转档模式 */
		else if( STRCMP( key , == , "rotate_mode" ) )
		{
			log_rotate_mode = ConvertLogRotateMode_atoi( p_value ) ;
			if( log_rotate_mode == LOG_ROTATEMODE_INVALID )
				return (int)log_rotate_mode;
			
			nret = SetLogRotateMode( g , log_rotate_mode ) ;
			if( nret )
				return nret;
		}
		/* 转档大小 */
		else if( STRCMP( key , == , "rotate_size" ) )
		{
			log_rotate_size = ConvertBufferSize_atol( p_value ) ;
			
			nret = SetLogRotateSize( g , log_rotate_size ) ;
			if( nret )
				return nret;
		}
		/* 转档压力因子 */
		else if( STRCMP( key , == , "rotate_pressure_factor" ) )
		{
			nret = SetLogRotatePressureFactor( g , atol(p_value) ) ;
			if( nret )
				return nret;
		}
		/* 行日志缓冲区大小 */
		else if( STRCMP( key , == , "log_bufsize" ) )
		{
			if( STRCMP( p_value , != , "" ) )
			{
				log_bufsize = ConvertBufferSize_atol( p_value ) ;
			}
			else
			{
				log_bufsize = -1 ;
			}
			
			if( STRCMP( p_value2 , != , "" ) )
			{
				max_log_bufsize = ConvertBufferSize_atol( p_value2 ) ;
			}
			else
			{
				max_log_bufsize = -1 ;
			}
			
			nret = SetLogBufferSize( g , log_bufsize , max_log_bufsize ) ;
			if( nret )
				return nret;
		}
		/* 十六进制日志缓冲区大小 */
		else if( STRCMP( key , == , "hexlog_bufsize" ) )
		{
			if( STRCMP( p_value , != , "" ) )
			{
				hexlog_bufsize = ConvertBufferSize_atol( p_value ) ;
			}
			else
			{
				hexlog_bufsize = -1 ;
			}
			
			if( STRCMP( p_value2 , != , "" ) )
			{
				max_hexlog_bufsize = ConvertBufferSize_atol( p_value2 ) ;
			}
			else
			{
				max_hexlog_bufsize = -1 ;
			}
			
			nret = SetHexLogBufferSize( g , hexlog_bufsize , max_hexlog_bufsize ) ;
			if( nret )
				return nret;
		}
	}
	
	return 0;
}

static int _CreateLogHandleFromKvFile( LOG *g , char *config_filename , char *file_ext_name )
{
	char		config_pathfilename[ MAXLEN_FILENAME + 1 ] ;
	FILE		*fp = NULL ;
	
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
	
	fp = fopen( config_pathfilename , "r" ) ;
	if( fp == NULL )
		return LOG_RETURN_ERROR_CONFIGFILE_NOTFOUND;
	
	nret = ReadKvFileForLogHandle( g , fp , NULL , -1 ) ;
	if( nret )
	{
		fclose(fp);
		return nret;
	}
	
	fclose(fp);
	
	return 0;
}

LOG *CreateLogHandleFromKvFile( char *config_filename , char *file_ext_name )
{
	LOG		*g = NULL ;
	
	int		nret ;
	
	g = CreateLogHandle() ;
	if( g == NULL )
		return NULL;
	
	nret = _CreateLogHandleFromKvFile( g , config_filename , file_ext_name ) ;
	if( nret )
	{
		DestroyLogHandle( g );
		return NULL;
	}
	
	return g;
}

#if ( defined _WIN32 ) || ( defined __linux__ ) || ( defined _AIX ) || ( defined __hpux )
LOG *CreateLogHandleFromKvFileG( char *config_filename , char *file_ext_name )
{
	LOG		*g = NULL ;
	
	int		nret ;
	
	g = CreateLogHandleG() ;
	if( g == NULL )
		return NULL;
	
	nret = _CreateLogHandleFromKvFile( g , config_filename , file_ext_name ) ;
	if( nret )
	{
		DestroyLogHandleG( g );
		return NULL;
	}
	
	return g;
}
#endif
