#include "LOG.h"

int bench_time( long count )
{
	long		l ;
	struct timeval	tv ;
	
	for( l = 0 ; l < count ; l++ )
	{
		gettimeofday( & tv , NULL );
	}
	
	return 0;
}

int main( int argc , char *argv[] )
{
	if( argc == 1 + 1 )
	{
		return -bench_time( atol(argv[1]) );
	}
	else
	{
		printf( "USAGE : bench_time count\n" );
		return 7;
	}
}
