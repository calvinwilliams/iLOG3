#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bench_snprintf( int type , long count )
{
	long		l ;
	
	pid_t		pid ;
	char		buf[ 20 + 1 ] ;
	
	int		nret ;
	
	if( type == 1 )
	{
		for( l = 0 ; l < count ; l++ )
		{
			pid = getpid() ;
			snprintf( buf , sizeof(buf) , "%ld" , pid );
		}
	}
	else if( type == 2 )
	{
		for( l = 0 ; l < count ; l++ )
		{
			pid = getpid() ;
			if( pid != 123456 )
			{
				memcpy( buf , "123456" , 6 ); buf[6] = '\0' ;
			}
		}
	}
	
	return 0;
}

int main( int argc , char *argv[] )
{
	if( argc == 1 + 2 )
	{
		return -bench_snprintf( atoi(argv[1]) , atol(argv[2]) );
	}
	else
	{
		printf( "USAGE : bench_snprintf type count\n" );
		return 7;
	}
}
