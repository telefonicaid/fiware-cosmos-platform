
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_UInt16
#define _H_SAMSON_system_UInt16




namespace samson{
namespace system{


	class UInt16 : public FixedLengthDataInstance<unsigned short>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
		}
	};


} // end of namespace ss
} // end of namespace system

#endif
