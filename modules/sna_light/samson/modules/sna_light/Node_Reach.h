
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_sna_light_Node_Reach
#define _H_SAMSON_sna_light_Node_Reach


#include <samson/modules/sna_light/Node_Reach_base.h>


namespace ss{
namespace sna_light{


	class Node_Reach : public Node_Reach_base
	{
	public:
		bool isExtern(){
		  return (flags.value & ss::sna_light::Node::NODE_FLAG_EXTERN);
		}

	};


} // end of namespace ss
} // end of namespace sna_light

#endif
