
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sort_parse_file_by_buckets
#define _H_SAMSON_sort_parse_file_by_buckets


#include <samson/module/samson.h>


namespace samson{
namespace sort{


	class parse_file_by_buckets : public samson::Parser
	{

	public:


		void run( char *data , size_t length , samson::KVWriter *writer )
		{
			//Datas to emit
			samson::system::UInt8 key;
			samson::system::UInt val;
			samson::system::Void dummy;

			//size_t offset = 0;
			//size_t line_begin = 0;

			char *p_data = data;
			char *p_line_begin = data;
			char *end_data = data + length;


			while( p_data < end_data )
			{
				// How is the input file?
				if (*p_data == '\n')
				{
					*p_data = '\0';

					val.value = strtoull( p_line_begin , NULL , 10 );

					key.value = val.value >> 56;
					writer->emit(0, &key, &val);

					p_line_begin = p_data+1;
				}
				++p_data;
			}
		}


	};


} // end of namespace samson
} // end of namespace sort

#endif