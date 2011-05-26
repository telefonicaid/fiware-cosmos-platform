
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_terasort_parse_file
#define _H_SAMSON_terasort_parse_file


#include <samson/module/samson.h>


namespace samson{
namespace terasort{


	class parse_file : public samson::Parser
	{

	public:


		void run( char *data , size_t length , samson::KVWriter *writer )
		{
			//Datas to emit
			Partition key;
			Record val;

			char *p_data = data;
			const char *p_line_begin = data;
			char *end_data = data + length;


			while( p_data < end_data )
			{
				// How is the input file?
				// Each line ends with "\r\n"
				if (*p_data == '\r')
				{
					*p_data = '\0';

					key.set(p_line_begin);
					//OLM_T(LMT_User01, ("For value:'%s', partition:%d", p_line_begin, int(key.value)));


#define NUMBYTES_KEY 10
#define NUMBYTES_SEP 2
#define NUMBYTES_NUMREC 32
#define NUMBYTES_VAL 52

					val.keyf.value = std::string(p_line_begin, NUMBYTES_KEY);
					p_line_begin += NUMBYTES_KEY + NUMBYTES_SEP;
					val.num_rec.value = std::string(p_line_begin, NUMBYTES_NUMREC);
					p_line_begin += NUMBYTES_NUMREC + NUMBYTES_SEP;
					val.valf.value = std::string(p_line_begin, NUMBYTES_VAL);

					writer->emit(0, &key, &val);

					// We know we have a '\n' after the '\r'
					p_line_begin = ++p_data;
					++p_line_begin;
				}
				++p_data;
			}
		}


	};


} // end of namespace samson
} // end of namespace terasort

#endif
