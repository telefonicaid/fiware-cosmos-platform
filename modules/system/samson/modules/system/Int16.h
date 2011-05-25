
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_Int16
#define _H_SAMSON_system_Int16

#include <samson/modules/system/FixedLengthDataInstance.h>


namespace samson{
namespace system{


	class Int16 : public FixedLengthDataInstance<short>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
		
	};


} // end of namespace samson
} // end of namespace system

#endif
