
#pragma mark once

#include "test.pb.h"
#include <sstream>
#include "StaticBuffer.h"
#include "DynamicBuffer.h"
#include "VarInt.h"
#include "Common.h"

// Example of a full class

namespace ss {


	class cdrFull : public protocol::cdr
	{
	public:
		std::string str()
		{
			std::ostringstream o;
			o << a() << "-" << b();
			return o.str();
		}
		
		bool operator <(const cdrFull& b2) const
		{
			if( a() < b2.a() )
				return true;
			if( a() > b2.a() )
				return false;
			
			if( b() < b2.b() )
				return true;
			if( b() > b2.b() )
				return false;
			
			return false;
		}
		
	};



	/** Simple buffer for debuggin */

	class SimpleStaticBuffer : public StaticBuffer
	{
	public:
		SimpleStaticBuffer(size_t size) 
		{
			// Init the static buffer
			initStaticBuffer( (char*) malloc( size ) , size );
		}
		
		
		~SimpleStaticBuffer()
		{
			free( getData() );
		}
	};

	/** Simple buffer for debuggin */

	class SimpleDynamicBuffer : public DynamicBuffer
	{
		char *_data;
		size_t _length;
		size_t _maxLength;
	public:
		
		SimpleDynamicBuffer( size_t maxLength )
		{
			_data = (char*) malloc( maxLength );
			_maxLength = maxLength;
			_length = 0;
		}
		
		~SimpleDynamicBuffer( )
		{
			if( _data )
				free( _data );
		}
		
		size_t write( char *data , size_t length)
		{
			assert( length + _length < _maxLength ); // This is just for debugging
			
			memcpy(_data + _length, data, length);
			_length += length;
			return length;
		}
		
		virtual size_t writeVarInt( size_t value )
		{
			size_t size = staticVarInt64Size(value);
			assert( _length + size  < _maxLength);	// this is just debuggin
			staticVarInt64Serialize(_data+_length, value);	
			_length += size;
			
			return size;
		}
		
		virtual size_t writeMessage( const SSMessage &message )
		{
			size_t message_size =  message.ByteSize();
			assert( _length + message_size < _maxLength );
			
			message.SerializeToArray( _data + _length , message_size );
			_length += message_size;
			
			return message_size;
		}
		
		virtual size_t serializeTo( char *data )
		{
			memcpy(data, _data, _length);
			return _length;
		}
		
		
		virtual size_t getLength()
		{
			return _length;
		}
	};
}