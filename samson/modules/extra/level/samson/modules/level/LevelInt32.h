


#ifndef _H_SAMSON_level_int32
#define _H_SAMSON_level_int32

#include <samson/modules/system/Int32.h>
#include <samson/modules/system/String.h>

namespace samson{
namespace level{


   class LevelInt32
   {
	  samson::KVWriter *writer;
	  int output_channel;

	  std::map< std::string , int > values;

   public:

	  LevelInt32( samson::KVWriter * _writer , int _output_channel )
	  {
		 writer = _writer;
		 output_channel = _output_channel;
	  }


	  void push( std::string name , int value )
	  {
		 std::map< std::string , int >::iterator it_values;

		 it_values = values.find( name );

		 if( it_values == values.end() )
			values.insert( std::pair< std::string , int >( name , value) );
		 else
			it_values->second += value;

		 // Flush if too many elements 
		 if( values.size() > 1000 )
			flush();

	  }

	  void flush()
	  {
		 std::map< std::string , int >::iterator it_values;

		 for ( it_values = values.begin() ; it_values != values.end() ; it_values++ )
		 {
			samson::system::String key;
			samson::system::Int32 value;

			key.value = it_values->first;
			value.value = it_values->second;

			writer->emit( output_channel , &key , & value );
		 }
	  }

   };


} // end of namespace level
} // end of namespace samson

#endif
