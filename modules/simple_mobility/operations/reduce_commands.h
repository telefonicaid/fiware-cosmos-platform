
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_simple_mobility_reduce_commands
#define _H_SAMSON_simple_mobility_reduce_commands


#include <samson/module/samson.h>
#include <samson/modules/simple_mobility/User.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/StringVector.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace simple_mobility{


	class reduce_commands : public samson::Reduce
	{

	   samson::system::UInt key;              // User identifier
	   samson::simple_mobility::User user;    // State information

	   samson::system::StringVector command;         // Input command to update this state
	   samson::system::String message;               // Message emited at the output

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt system.StringVector  
input: system.UInt simple_mobility.User  
output: system.UInt system.String
output: system.UInt simple_mobility.User

helpLine: Update internal state
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
           if ( inputs[1].num_kvs == 0 )
           {
              // New user state
              user.init();
              key.parse( inputs[0].kvs[0]->key );
           }
           else
           {
			  // Previous state
			  user.parse( inputs[1].kvs[0]->value ); // We only consider one state
			  key.parse( inputs[1].kvs[0]->key );
           }

		   for (size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
           {
              command.parse( inputs[0].kvs[i]->value );

              if( command.values_length == 0)
                 continue;

              if( command.values[0].value == "TRACK_ON" )
              {
				 message.value = "Traking user on";
				 writer->emit( 0 , &key,  &message );

				 user.setTraking( true );
              }
              else if( command.values[0].value == "TRACK_OFF" )
              {
                 message.value = "Stop traking user off";
				 writer->emit( 0 , &key,  &message );

				 user.setTraking( false );
              }
			  else if ( command.values[0].value == "AREA_CREATE" )
			  {
				 // AREA_CREATE name x y radius

				 if ( command.values_length < 5 )
				 {
					message.value = au::str( "Error creating area. #arguments %d < 5" , (int) command.values_length );
					writer->emit( 0 , &key,  &message );
				 }
				 else
				 {
					
					samson::simple_mobility::UserArea *area = user.areasAdd();
					
					area->name.value = command.values[1].value;
					area->x.value    = atoi( command.values[2].value.c_str() );
					area->y.value    = atoi( command.values[3].value.c_str() );
					area->radius.value   = atoi( command.values[4].value.c_str() );
					
					message.value = au::str( "Area '%s' created at point [%d,%d] with radiud %d " , area->name.value.c_str() , area->x.value , area->y.value , area->radius.value );
					writer->emit( 0 , &key,  &message );
				 }

			  }
			  else if ( command.values[0].value == "CLEAR_AREAS" )
			  {
				 // remove areas for this user
				 user.areasSetLength(0);

				 message.value = "Areas removed";
                 writer->emit( 0 , &key,  &message );
			  }


           }

		   // Emit the state again
		   writer->emit( 1 , &key , &user );

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace simple_mobility
} // end of namespace samson

#endif
