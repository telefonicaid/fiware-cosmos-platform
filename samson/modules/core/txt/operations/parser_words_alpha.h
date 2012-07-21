
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_parser_words_alpha
#define _H_SAMSON_txt_parser_words_alpha


#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/SimpleParser.h>


namespace samson{
namespace txt{


	class parser_words_alpha : public samson::system::SimpleParser
	{

	   samson::system::String key;
	   samson::system::UInt value;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  output: system.String system.UInt
//  
// helpLine: Parse text generating all words separated by any non-alphanumeric character
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		   value.value = 1;
		}

		bool isSeparator( char c )
		{
		   if( c == ' ')
			  return  true;
		   if( c == '\t')
			  return  true;
		   
		   return false;
		}

		void parseLine( char * line, samson::KVWriter *writer )
		{
		   size_t len = strlen( line );
		   size_t pos = 0;
		   for ( size_t i = 0 ; i < (len+1) ; i++ )
		   {
			  if( isSeparator( line[i] ) || line[i]=='\0' )
			  {
				 if( pos < i )
				 {
					// New word
					key.value ="";
					key.value.append( &line[pos] ,i-pos );					

					writer->emit( 0 , &key , &value );
				 }
				 // Go to the next
				 pos = i+1;
			  }
		   }

		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace txt
} // end of namespace samson

#endif