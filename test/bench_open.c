#include "LOG.h"

#if ( defined _WIN32 )
#define PATHFILENAME		"../../bench_open.log"
#elif ( defined __linux__ ) || ( defined __unix )
#define PATHFILENAME		"bench_open.log"
#endif

int bench_stat( long count )
{
	int		fd ;
	long		l ;
	
	for( l = 0 ; l < count ; l++ )
	{
		fd = open( PATHFILENAME , O_CREAT|O_APPEND|O_WRONLY , S_IRWXU|S_IRWXG|S_IRWXO ) ;
		if( fd == -1 )
		{
			printf( "open failed , errno[%d]\n" , errno );
			return -1;
		}
		
		close( fd );
	}
	
	return 0;
}

int main( int argc , char *argv[] )
{
	if( argc == 1 + 1 )
	{
		return -bench_stat( atol(argv[1]) );
	}
	else
	{
		printf( "USAGE : bench_open count\n" );
		return 7;
	}
}
