
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_fakeServers
#define _H_SAMSON_url_fakeServers


#include <samson/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>

namespace ss{
namespace url{


	class fakeServers : public ss::Generator
	{

	public:


		void run( ss::KVWriter *writer )
		{
		  ss::system::String server;
		    ss::system::UInt id; 

                   // Recover the number of servers from the environment
                   size_t num_servers = environment->getSizeT( "url.num_servers" ,  1000000 );
		   for ( size_t i = 0 ; i < num_servers ; i++)
		   {
		     std::ostringstream o;
		     o << "ServerName_" << i;
		     server.value = o.str();

		     id.value = i;

		     writer->emit( 0 , &server , &id );

		     // It can also be convenient to have the reversed data-set
		     writer->emit( 1, &id, &server);
		    
		   }

		}


	};


} // end of namespace ss
} // end of namespace url

#endif
