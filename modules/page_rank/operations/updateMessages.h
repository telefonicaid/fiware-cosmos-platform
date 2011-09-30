
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_page_rank_updateMessages
#define _H_SAMSON_page_rank_updateMessages


#include <map>
#include <string>

#include <samson/module/samson.h>
#include <samson/modules/page_rank/Message.h>
#include <samson/modules/page_rank/Node.h>
#include <samson/modules/system/String.h>


namespace samson{
namespace page_rank{


	class updateMessages : public samson::Reduce
	{

	public:


	   std::map< std::string , double > messages;

	   samson::system::String key;// key
	   samson::page_rank::Node node;// Node

	   samson::page_rank::Message message;

	   samson::system::String output_message_key;
	   samson::page_rank::Message output_message_value;


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.String page_rank.Message  
input: system.String page_rank.Node  
output: system.String page_rank.Message
output: system.String page_rank.Node

helpLine: Update intenal information about rank of pages pointing to me
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

		   // Parse key & node ( or to init a new node if there was not a previous one )
		   // ------------------------------------------------------------------------------------------------------
           if( inputs[1].num_kvs > 0 )
           {
              if( inputs[1].num_kvs > 1 )
                 tracer->setUserError("Error since we receive two version of the internal state");

              // There is state
              key.parse( inputs[1].kvs[0]->key );
              node.parse( inputs[1].kvs[0]->value );

			  if( inputs[0].num_kvs == 0)
			  {
				 // No update...
				 node.updated_outputs.value = 0;
				 writer->emit(1, &key , &node );
				 return;
			  }


           }
           else
           {
              // No previous state
              key.parse( inputs[0].kvs[0]->key );

              // Init the node
              node.linksSetLength(0);
              node.messagesSetLength(0);
              node.rank.value = 1;			  

              node.updated_outputs.value = 0;
              node.update_count.value = 0;

		   }
		   
		   // update the cicle counter
		   node.update_count.value++;

		   // Put the messages to the previus messages to the auxiliar map
		   // ------------------------------------------------------------------------------------------------------
		   messages.clear();
		   for (int i = 0 ; i < node.messages_length ; i++ )
			  messages.insert( std::pair< std::string , double >( node.messages[i].node.value , node.messages[i].contribution.value ) );

		   // Add of modify incomming messages
		   for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
		   {
			  // Parse the incomming message
			  message.parse( inputs[0].kvs[i]->value );

			  if( message.contribution.value == -1 )
			  {
				 // Remove this node
				 messages.erase( message.node.value );
			  }
			  else
			  {

				 std::map<std::string , double>::iterator message_it;
				 message_it = messages.find( message.node.value );
				 
				 if( message_it == messages.end() )
					messages.insert( std::pair< std::string , double >( message.node.value , message.contribution.value ) );
				 else
					message_it->second = message.contribution.value;
			  }
		   }

		   // Put the resulting messages back to node
		   // ------------------------------------------------------------------------------------------------------

		   node.messagesSetLength(0);
		   for( std::map<std::string , double>::iterator message_it = messages.begin() ; message_it != messages.end() ; message_it++ )
		   {
			  samson::page_rank::Message *m = node.messagesAdd();
			  m->node.value = message_it->first;
			  m->contribution.value = message_it->second;
		   }


		   double previous_rank = node.rank.value;
		   node.recompute_rank();

		   // Re-emit my contribution to all the nodes if necessary
		   // ------------------------------------------------------------------------------------------------------

		   // Prepare output messages ( always with my name as node )
           output_message_value.node.value = key.value;

		   node.updated_outputs.value = 0;
		   
		   if( fabs( previous_rank - node.rank.value ) > 0.001 )
		   {
			  //printf("Node %s not emiting rank  to %d connections\n" , key.value.c_str() , node.links_length );

			  for (int i = 0 ; i < node.links_length ; i++)
			  {
					output_message_key.value = node.links[i].value;
					output_message_value.contribution.value = node.contribution(); // contribution
					writer->emit( 0 , &output_message_key , &output_message_value );

					node.updated_outputs.value++;
			  }
		   }
		   else
		   {
			  //printf("Node %s not emiting rank  old:%f new:%f\n" , key.value.c_str() , previous_rank , node.rank.value);
		   }

		   // ------------------------------------------------------------------------------------------------------


		   // Emit the node at the output...
		   writer->emit( 1 , &key , &node );

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace page_rank
} // end of namespace samson

#endif
