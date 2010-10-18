
#include "Delilah.h"		//ss:Delilah
#include "traces.h"			// LMT_*

int main(int argc, const char *argv[])
{
	ss::samsonInitTrace( argc , argv );
	
	ss::Network network;								// Real network interface element
	ss::Delilah delilah( argc, argv , &network  );		// Dalilah instance
	delilah.run();										// Main run loop
}
