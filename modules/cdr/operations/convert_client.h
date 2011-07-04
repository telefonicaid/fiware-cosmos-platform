
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_cdr_convert_client
#define _H_SAMSON_cdr_convert_client


#include <samson/module/samson.h>


namespace samson{
namespace cdr{


	class convert_client : public samson::Map
	{
		samson::system::Void  void_data;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt cdr.User
output: system.UInt64 system.Void

helpLine: Convert Clients from Gasset format to Mob format.
extendedHelp: 		Convert Clients from Gasset format to Mob format.

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			/*
			  in  UInt GSTUser
			  out Uint64 Void
			*/

				//input
			  samson::system::UInt user_id;
			  User  user;
				//output
				samson::system::UInt64 client;



				for (int i=0  ; i< inputs[0].num_kvs ;i++){
			    user_id.parse(inputs[0].kvs[i]->key);
			    user.parse(inputs[0].kvs[i]->value);

			    client.value = user.id.value;

			    writer->emit(0, &client, &void_data);

			  }
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace samson
} // end of namespace cdr

#endif
