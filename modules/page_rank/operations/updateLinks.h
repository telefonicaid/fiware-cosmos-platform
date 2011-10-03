
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_page_rank_updateLinks
#define _H_SAMSON_page_rank_updateLinks

#include <set>

#include "strCompare.h"

#include <samson/module/samson.h>
#include <samson/modules/page_rank/LinkVector.h>
#include <samson/modules/page_rank/Message.h>
#include <samson/modules/page_rank/Node.h>
#include <samson/modules/system/String.h>


namespace samson{
namespace page_rank{

	class updateLinks : public samson::Reduce
	{

	   samson::system::String input_key;
	   samson::page_rank::LinkVector input_value;

	   samson::page_rank::Node node; // Output node

	   samson::system::String output_message_key;
	   samson::page_rank::Message output_message_value;


	   std::set<const char*,strCompare> previous_links;
	   std::set<const char*,strCompare> new_links;

	   
	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.String page_rank.Links  
input: system.String page_rank.Node  
output: system.String page_rank.Message
output: system.String page_rank.Node

helpLine: Update intenal information about links of nodes. Comming from crawling...
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		   if( inputs[1].num_kvs > 0 )
		   {
			  if( inputs[1].num_kvs > 1 )
				 tracer->setUserError("Error since we receive two version of the internal state");

			  // There is state
			  input_key.parse( inputs[1].kvs[0]->key );
			  node.parse( inputs[1].kvs[0]->value );
		   }
		   else
		   {
			  // No previous state
			  input_key.parse( inputs[0].kvs[0]->key );

              // Init the node
			  node.linksSetLength(0);
			  node.messagesSetLength(0);
			  node.rank.value = 1;
			  node.updated_outputs.value = 0;
			  node.update_count.value = 0;
		   }


		   if( inputs[0].num_kvs == 0)
		   {
			  // No modification since there are no input messages
			  node.updated_outputs.value = 0;
			  writer->emit( 1 , &input_key ,  &node );
			  return;
		   }

		   // Increase the counter of cicles
		   node.update_count.value++;

		   // Take the last input ( there is no sence to update twice the vector of links )
		   input_value.parse( inputs[0].kvs[ inputs[0].num_kvs -1 ]->value );

		   // Send -1 to links that have disapeared
		   // Send my contribution to all if # number of output links changed
		   // Send my contribution to new nodes


		   bool changed_num_links = ( input_value.link_length != node.links_length );

		   previous_links.clear();
		   new_links.clear();


		   // Add the previos nodes to the list
		   for (int i = 0 ; i < node.links_length ; i++)
			  previous_links.insert( node.links[i].value.c_str() );

		   // Add the new nodes to the list
		   for (int i = 0 ; i < input_value.link_length ; i++)
			  new_links.insert( input_value.link[i].value.c_str() );

		   // Put the new links into the node structure
		   // --------------------------------------------------------------------------------------------
		   node.linksSetLength(0);		   
		   for( std::set<const char* , strCompare>::iterator it = new_links.begin() ; it != new_links.end() ; it++)
			  node.linksAdd()->value = *it; // Add the new links to the state
		   // --------------------------------------------------------------------------------------------


		   // Prepare output messages ( always with my name as node )
		   output_message_value.node.value = input_key.value;

		   node.updated_outputs.value = 0;

		   // Nodes removed
		   for( std::set<const char*,strCompare>::iterator it = previous_links.begin() ; it != previous_links.end() ; it++)
		   {
			  if( new_links.find( *it ) == new_links.end() )
			  {
				 // Emit the node as "disconnected"
				 output_message_key.value = *it;
				 output_message_value.contribution.value = -1;
				 writer->emit( 0 , &output_message_key , &output_message_value );				 

				 node.updated_outputs.value++;
			  }
		   }



		   // Compute the new rank if changed the number of outputs
		   // --------------------------------------------------------------------------------------------
		   if( changed_num_links )
			  node.recompute_rank();
		   // --------------------------------------------------------------------------------------------		   



		   // Send contributions to new nodes ( or to all of them if the number of outputs changed )
		   // --------------------------------------------------------------------------------------------
		   for( std::set<const char*,strCompare>::iterator it = new_links.begin() ; it != new_links.end() ; it++)
		   {
			  if( ( changed_num_links ) || ( previous_links.find(*it) == previous_links.end() ) )
			  {
				 output_message_key.value = *it;
                 output_message_value.contribution.value = node.contribution(); // contribution
                 writer->emit( 0 , &output_message_key , &output_message_value );				 

				 node.updated_outputs.value++;
			  }
		   }

		   // Emit the node at the end
		   writer->emit( 1 , &input_key , &node );


		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace page_rank
} // end of namespace samson

#endif
