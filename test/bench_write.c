#include "LOG.h"

#if ( defined _WIN32 )
#define PATHFILENAME		"../../bench_write.log"
#elif ( defined __linux__ ) || ( defined __unix )
#define PATHFILENAME		"bench_write.log"
#endif

int bench_write( long count )
{
	long		l ;
	
	int		fd ;
	char		buffer[ 64 + 1 ] ;
	long		buflen ;
	
	sprintf( buffer , "YYYY-MM-DD hh:mm:ss | DEBUG | 1234:87654321:bench_write.c:20 | loglog\n" );
	buflen = strlen(buffer) ;
	
	for( l = 0 ; l < count ; l++ )
	{
		fd = open( PATHFILENAME , O_CREAT|O_APPEND|O_WRONLY , S_IRWXU|S_IRWXG|S_IRWXO ) ;
		if( fd == -1 )
		{
			printf( "open failed , errno[%d]\n" , errno );
			return -1;
		}
		
		write( fd , buffer , buflen );
		
		close( fd );
	}
	
	return 0;
}

int main( int argc , char *argv[] )
{
	if( argc == 1 + 1 )
	{
		return -bench_write( atol(argv[1]) );
	}
	else
	{
		printf( "USAGE : bench_write count\n" );
		return 7;
	}
}
