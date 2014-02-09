#include "LOG.h"

int bench_time( long count )
{
	long		l ;
	long		tt ;
	
	for( l = 0 ; l < count ; l++ )
	{
		time( & tt );
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
