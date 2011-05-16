
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_collocation_count
#define _H_SAMSON_txt_collocation_count


#include <samson/Operation.h>
#include <iostream>
#include <fstream>
#include <string>



namespace ss{
namespace txt{


	class collocation_count : public ss::Reduce
	{

	public:

		ss::txt::Collocation key;
		ss::txt::Collocation keyPrev;
		ss::system::UInt count;
		ss::system::UInt total;

		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{


			if( inputs[0].num_kvs == 0 )
			{
				return;
			}

			total.value = 0;
			//LM_M(("Reducing %d pairs", inputs[0].num_kvs));
			key.parse(inputs[0].kvs[0]->key);

#define DEBUG_FILES
#ifdef DEBUG_FILES
			const char *cad1 = key.w1.value.c_str();
			const char *cad2 = key.w2.value.c_str();
			std::string filename = "/tmp/collocation_count_" + key.w1.value + ".log";
			std::ofstream fs(filename.c_str(), std::ios::app); 
			fs << "[0]: Treating: (" << key.w1.value << ", " << key.w2.value << ") con " << inputs[0].num_kvs << " values " << std::endl;
			fs.close();
#endif /* de DEBUG_FILES */

			for (size_t i = 0; i < inputs[0].num_kvs ; i++)
			{
			key.parse(inputs[0].kvs[i]->key);

#ifdef DEBUG_FILES
			const char *cad1 = key.w1.value.c_str();
			const char *cad2 = key.w2.value.c_str();
			std::string filename = "/tmp/collocation_count_" + key.w1.value + ".log";
			std::ofstream fs(filename.c_str(), std::ios::app); 
			fs << "[" << i << "]: Treating: (" << key.w1.value << ", " << key.w2.value << ") con " << inputs[0].num_kvs << " values " << std::endl;
			fs.close();
#endif /* de DEBUG_FILES */

				count.parse( inputs[0].kvs[i]->value );

				total.value += count.value;
			}

			if (total.value > 0)
			{
				writer->emit( 0, &key , &total );
			}
		}
	};


} // end of namespace ss
} // end of namespace txt

#endif
