#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int bench_fcntl()
{
	int		fd ;
	struct flock	lock ;
	
	int		nret ;
	
	fd = open( "/tmp/iLOG3.lock" , O_CREAT|O_APPEND|O_WRONLY , S_IRWXU|S_IRWXG|S_IRWXO ) ;
	if( fd == -1 )
	{
		printf( "open failed\n" );
		return -1;
	}
	
	memset( & lock , 0x00 , sizeof(lock) );
	lock.l_type = F_WRLCK ;
	lock.l_whence = SEEK_SET ;
	lock.l_start = 0 ;
	lock.l_len = 0 ;
	printf( "before WRLCK\n" ); fflush(stdout);
	nret = fcntl( fd , F_SETLKW , & lock ) ;
	printf( "after WRLCK\n" ); fflush(stdout);
	if( nret == -1 )
	{
		printf( "fcntl WRLCK failed\n" );
		return -1;
	}
	
	sleep(5);
	
	memset( & lock , 0x00 , sizeof(lock) );
	lock.l_type = F_UNLCK ;
	lock.l_whence = SEEK_SET ;
	lock.l_start = 0 ;
	lock.l_len = 0 ;
	printf( "before UNLCK\n" ); fflush(stdout);
	nret = fcntl( fd , F_SETLK , & lock ) ;
	printf( "after UNLCK\n" ); fflush(stdout);
	if( nret == -1 )
	{
		printf( "fcntl UNLCK failed\n" );
		return -1;
	}
	
	close(fd);
	
	return 0;
}

int main()
{
	return -bench_fcntl();
}

