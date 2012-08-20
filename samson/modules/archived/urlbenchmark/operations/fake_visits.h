
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_urlbenchmark_fake_visits
#define _H_SAMSON_urlbenchmark_fake_visits


#include <samson/module/samson.h>

/*********************************************************
generator fake_visits
{
	out	system.UInt system.String  # UserId - URL visited

	helpLine "Generate a fixed set of visits from a group of of users and a number of URLs"
}
 **********************************************************/

namespace samson{
namespace urlbenchmark{


class fake_visits : public samson::Generator
{

	bool active;    // Flag to know if we have to run or not

public:

	void setup( int worker , int num_workers, int process , int num_processes )
	{
		if( ( worker == 0 ) && ( process == 0 ) )
			active = true;
		else
			active = false;
	}


	void run( samson::KVWriter *writer )
	{
		if(!active)
			return;

		// Recover the number of users in this demo
		size_t num_users  = environment->get( "urlbenchmark.num_users" ,  1000000 );

		// Recover the number of urls per user
		size_t num_urls_per_user = environment->get( "urlbenchmark.num_urls_per_user" ,  100 );

		// Recover the number of paths per server
		size_t num_paths_per_server = environment->get( "urlbenchmark.num_paths_per_server" ,  10 );

		// Recover the number of servers from the environment
		size_t num_servers = environment->get( "urlbenchmark.num_servers" ,  1000000 );


		samson::system::UInt userId;
		samson::system::String url;

		for (size_t u = 0 ; u < num_users ; u++ )
		{
			if( !(u%1000) )
				operationController->reportProgress( (double) u / (double) num_users );

			for ( size_t t = 0 ; t < num_urls_per_user ; t++ )
			{
				size_t user = rand()%num_users;
				size_t server = rand()%num_servers;
				size_t path = rand()%num_paths_per_server;


				std::ostringstream o;
				o << "http://" << "ServerName_" << server << "/" << "path_to_somewhere_" << path;
				url.value = o.str();
				o.str(""); //Clear content;

				userId.value = user;

				writer->emit( 0 , &userId, &url);
			}
		}
	}

};


} // end of namespace samson
} // end of namespace urlbenchmark

#endif
