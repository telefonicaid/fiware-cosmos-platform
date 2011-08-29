
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_terasort_Partition
#define _H_SAMSON_terasort_Partition


#include <samson/modules/system/UInt8.h>


namespace samson{
namespace terasort{


	class Partition : public samson::system::UInt8
	{
		typedef unsigned char uchar;

		public:

		// Function to get a value depending on the range of the string key
		inline void set ( const char * _value)
		{
			// We compute the partition based on the two first characters.
			// As they are ASCII (95 values, in fact), we use the extra bit to partitionate
			// based on the second character.
			value = (_value[0]<<1) + ((_value[1] < 'O' ? 0 : 1));
			//OLM_T(LMT_User01, ("For value:'%s', partition:%d", _value, int(value)));
		}

		// Particular behaviour for sorting
		int partition( int num_partitions )
		{
			return ( num_partitions * value / 256 );
		}
	};


} // end of namespace samson
} // end of namespace terasort

#endif
