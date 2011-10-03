
/**
   File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_page_rank_Node
#define _H_SAMSON_page_rank_Node

//#define D_FACTOR 0.85
#define D_FACTOR 0.5

#include <samson/modules/page_rank/Node_base.h>

namespace samson{
	namespace page_rank{
		
		
		class Node : public Node_base
		{
			
		public:
			
			double contribution()
			{
				return rank.value / (double) links_length; 
			}
			
			void recompute_rank()
			{
				
				rank.value = 0;
				for (int i = 0 ; i < messages_length ; i++ )
					rank.value +=  messages[i].contribution.value;
				
				rank.value *= D_FACTOR;
				
				rank.value += (1.0-D_FACTOR);
				
			}

			std::string str()
            {

			   std::ostringstream o;
			   o << rank.value << " " << update_count.value << " " << updated_outputs.value;
			   return o.str();

            }


			
			std::string _str()
			{
				
				std::ostringstream o;
				
				o << "Node ( rank:" << rank.str() << " " << " loops:" << update_count.value << " changedOutputs:" << updated_outputs.value <<  " ) ";

				o << "Links [ ";
				
				for(int i = 0 ; i < links_length ; i++)
				{
					{ //Texting links
						o << links[i].str();
					}
					o << " ";
				}
				o << "] ";
				
				
				o << " Messages: [ ";
				for(int i = 0 ; i < messages_length ; i++)
				{
					o << messages[i].str();
					o << " ";
				}
				
				
				o<<"]";
				
				return o.str();
			}
			
			
		};
		
		
	} // end of namespace samson
} // end of namespace page_rank

#endif
