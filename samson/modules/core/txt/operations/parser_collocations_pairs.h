
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_parser_collocations_pairs
#define _H_SAMSON_txt_parser_collocations_pairs


#include <samson/modules/system/SimpleParser.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <samson/module/samson.h>


namespace samson{
namespace txt{


	class parser_collocations_pairs : public samson::Parser
	{
		samson::txt::Collocation key;
		samson::txt::Collocation key2;
		samson::system::UInt value;
		int tam_ventana;

	public:

		parser_collocations_pairs()
		{
			value.value = 1; // the counter  is always 1
		}

		void init(samson::KVWriter *writer)
		{
			tam_ventana = environment->getInt( "txt.tam_ventana" , 3 );
		}

		void run( char *data , size_t length , samson::KVWriter *writer )
		{
			size_t pos_begin=0;
			size_t pos_end;

			std::string word;
			std::vector<std::string>entorno;

			entorno.clear();

			while (true)
			{
				while( (data[pos_begin]!='\0') && ((data[pos_begin]==' ') || (data[pos_begin]=='\r') || (data[pos_begin]=='\n')))
				{
					pos_begin++;
				}

				if (data[pos_begin] == '\0')
				{
					break;
				}

				pos_end = pos_begin+1;

				while( (data[pos_end]!='\0') && (data[pos_end]!=' ') && (data[pos_end]!='\r') && (data[pos_end]!='\n'))
				{
					pos_end++;
				}

				if( data[pos_end] == '\0' )
				{
					if (data[pos_begin] != '\0')
					{
						word = std::string(&data[pos_begin]);
						entorno.push_back(word);
					}
				}
				else
				{
					data[pos_end]='\0';
					if (data[pos_begin] != '\0')
					{
						entorno.push_back(&data[pos_begin]);
					}
				}

				pos_begin = pos_end+1;
			}

			std::vector<std::string>::iterator iter1 = entorno.begin();
			while (iter1 < entorno.end())
			{
				std::vector<std::string>::iterator iter2 = iter1+1;
				key.w1 = *iter1;
				key2.w2 = *iter1;
				for (int i = 0; ((i < tam_ventana) && (iter2 < entorno.end())); i++, iter2++)
				{
					key.w2 = *iter2;
					key2.w1 = *iter2;

					writer->emit(0, &key, &value);

					writer->emit(0, &key2, &value);
				}
				iter1++;
			}
		}
	};


} // end of namespace samson
} // end of namespace txt

#endif