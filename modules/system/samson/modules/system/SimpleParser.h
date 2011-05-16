
#ifndef _H_SAMSON_SIMPLE_PARSER
#define _H_SAMSON_SIMPLE_PARSER


#include <samson/Operation.h>

namespace ss{
namespace system{


	class SimpleParser : public ss::Parser
	{

	public:

		virtual void parseLine( char * line , ss::KVWriter *writer )=0;
		
		void run( char *data , size_t length , ss::KVWriter *writer )
		{
			
			size_t line_begin = 0;
			size_t offset = 0;
			
			while( offset < length )
			{
				
				if( data[offset] == '\n' || data[offset] == '\0' )
				{
					data[offset] = '\0';
					
					parseLine( data+line_begin , writer );
					line_begin = offset+1;
				}
				
				offset++;
			}
			
			if( line_begin < (length-1) )
			{
				data[length-1] = '\0';
				parseLine( data+line_begin , writer );
			}
			
		}
	};

} // end of namespace system
} // end of namespace ss

#endif
