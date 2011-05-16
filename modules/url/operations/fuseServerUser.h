
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_fuseServerUser
#define _H_SAMSON_url_fuseServerUser


#include <samson/samson.h>

/********************************************************************
reduce fuseServerUser
{
	in system.String	url.UserPath	# Server(string) User(id)-Path(string)
	in system.String	system.UInt	    # Server(string) server(id)
	out system.UInt		system.UInt	    # Server - User
	out system.UInt		system.UInt	    # User - Server

	helpLine "From the URL information and server encoding. generates the subsets related to user and server visits, needed in the next steps. Splitted from reduce fuseServer because of limited number of outputs in the platform"
}
*********************************************************************/

namespace ss{
namespace url{


	class fuseServerUser : public ss::Reduce
	{

	public:


		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
		    if ((inputs[1].num_kvs == 0) && (inputs[0].num_kvs == 0))
		    {
		      OLM_E(("Server without key-values"));
		      return;
		    }

		    if ( inputs[0].num_kvs == 0)
		    {
		      ss::system::String s;
		      s.parse( inputs[1].kvs[0]->key );
		      OLM_D(("'%s' has no hits",s.value.c_str()));
		      return;
		    }

		    if ( inputs[1].num_kvs == 0)
		    {
		      ss::system::String s;
		      s.parse( inputs[0].kvs[0]->key );
		      OLM_E(("Input '%s' not included in server encoding table",s.value.c_str()));
		      return;
		    }

		    if ( inputs[1].num_kvs != 1)
		    {
		      ss::system::String s;
		      s.parse( inputs[1].kvs[0]->key );
		      OLM_E(("Input '%s' with multiple entries in server encoding table: inputs[1].num_kvs:%d",s.value.c_str(), inputs[1].num_kvs));
		    }

		  ss::system::UInt server;


		  server.parse(inputs[1].kvs[0]->value);

		  for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
		  {
			  UserPath userPath;


		    userPath.parse( inputs[0].kvs[i]->value );
		    writer->emit( 0 , &server , &userPath.user );
		    writer->emit( 1 , &userPath.user , &server );
		  }

		}


	};


} // end of namespace ss
} // end of namespace url

#endif
