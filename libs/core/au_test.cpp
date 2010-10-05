#include "au.h"

au::Lock lock;
au::Lock lock2;


void *task( void *)
{
	lock2.lock();
	std::cout << "locked2\n";
	sleep(5);
	std::cout << "Go for lock\n";
	lock.lock();
	
	return NULL;
}


int main( int args , char *argv[])
{
	fprintf(stdout ,"Test of AU Lib\n");
	fprintf(stdout, "-----------------------------\n\n");
	{

		
		
/*		
		// Test simple auto-loop
		lock.lock();
		lock.lock();
 */
		
		
		lock.lock();
		std::cout << "locked1\n";
		
		
		pthread_t t;
		pthread_create(&t, 0, task, NULL);
		
		sleep(5);
		std::cout << "Go for lock2\n";
		lock2.lock();
		
		
		
		
		
	}	
	
}

