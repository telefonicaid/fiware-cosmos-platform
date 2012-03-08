#ifndef KV_WRITER_H
#define KV_WRITER_H

#include "samson/module/DataInstance.h"        /* DataInstance                             */
#include <string>

namespace samson {
    

	class KVWriter
	{
        
        
	public:

		// Emit key values to a particular output
		virtual void emitKV( int output , DataInstance *key , DataInstance *value )=0;
		
		//virtual void emit(DataInstance* key, DataInstance* value) = 0;
		//virtual void close()=0;
		virtual ~KVWriter(){}
	};
	
	
	class TXTWriter
	{
	public:
        
		virtual ~TXTWriter(){}
        
        
		virtual void emitKV( const char * data , size_t length)=0;
		void emitKV( std::string data )
		{
			emitKV(data.c_str(), data.length());
		}
	};
	
}

#endif
