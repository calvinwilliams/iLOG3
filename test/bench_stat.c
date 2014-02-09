#include "LOG.h"

#if ( defined _WIN32 )
#define PATHFILENAME		"../../bench_stat.c"
#elif ( defined __linux__ ) || ( defined __unix )
#define PATHFILENAME		"bench_stat.c"
#endif

int bench_stat( long count )
{
	long		l ;
	struct STAT	st ;
	
	int		nret ;
	
	for( l = 0 ; l < count ; l++ )
	{
		nret = STAT( PATHFILENAME , & st ) ;
		if( nret )
		{
			printf( "stat failed[%d] , errno[%d]\n" , nret , errno );
			return -1;
		}
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
		printf( "USAGE : bench_stat count\n" );
		return 7;
	}
}
