#ifndef KV_WRITER_H
#define KV_WRITER_H

#include <samson/DataInstance.h>        /* DataInstance                             */



namespace ss {

	class KVWriter
	{
	public:

		// Emit key values to a particular output
		virtual void emit( int output , DataInstance *key , DataInstance *value )=0;
		
		//virtual void emit(DataInstance* key, DataInstance* value) = 0;
		//virtual void close()=0;
		virtual ~KVWriter(){}
	};
	
	
	class TXTWriter
	{
	public:
		virtual void emit( const char * data , size_t length)=0;
		void emit( std::string data )
		{
			emit(data.c_str(), data.length());
		}
	};
	
}

#endif
