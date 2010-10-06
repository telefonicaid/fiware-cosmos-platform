
#pragma once


namespace ss {

	class KVSetHeaderFull : public protocol::KVSetHeader
	{
	public:
		void increaseNumKVs( size_t _num_kvs )
		{
			set_num_kvs( num_kvs() + _num_kvs );
		}
		
		void increaseSize( size_t _size )
		{
			set_size( size() + _size );
		}
		
	};

	/*
	 Object to serialize key-value pairs 
	 */

	class KVSerializer : public KVWriter
	{
		// Buffer to store content
		DynamicBuffer *_buffer;
		
		// Header of the serialization ( setup parameters can only be modified at constructor )
		KVSetHeaderFull header;
		
		
	public:
		
		KVSerializer( DynamicBuffer *buffer , int style )
		{
			// Normal Serializer
			_buffer = buffer;
			
			// Init internal variables
			header.set_style( style );
			header.set_num_kvs(0);
			header.set_size(0);
			header.mutable_format()->set_key("Unknown"); 
			header.mutable_format()->set_value("Unknown"); 
			header.set_magic_number( 123 );
		}
		
		void emit( const SSMessage &key , const SSMessage &value)
		{
			// Depending of the format we emit with different styles
			header.increaseNumKVs(1);
			
			// Default style
			switch (header.style()) {
				case 0:
				{
					// Write the size of the key
					header.increaseSize( _buffer->writeVarInt(key.ByteSize()) );
					
					// write the key 
					header.increaseSize( _buffer->writeMessage(key) ); 
					
					// Write the size of the value
					header.increaseSize( _buffer->writeVarInt(value.ByteSize()) );
					
					// write the value 
					header.increaseSize( _buffer->writeMessage(value) );
				}
					
					break;
				default:
					break;
			}
			
		}
		
		/* Total size needed to store the output */
		size_t ByteSize()
		{
			size_t header_size = header.ByteSize();
			return staticVarInt64Size(header_size) + header_size + _buffer->getLength();
		}
		
		void SerializeToStaticBuffer( StaticBuffer &buffer )
		{
			// Target buffer information
			char *targetData = buffer.getData();
			size_t targetLength = buffer.getLength();
			
			size_t length = ByteSize();
			assert( length == targetLength );	// Size of the destination static buffer has to be exactly the size to serialize
			
			size_t offset = 0;
			size_t header_size = header.ByteSize();
			
			// Serialze the size of the header
			offset += staticVarInt64Serialize(targetData+offset, header_size );
			
			// Serialize the header
			header.SerializeToArray(targetData+offset, header_size);
			offset += header_size;
			
			// Seralize the synamic buffer content
			_buffer->serializeTo(targetData+offset);
			offset += _buffer->getLength();
			
			assert( offset == length);	// Assert that the total size writed to the output is correct
			
		}
		
	};
	
	
	class KVSerializerCollection 
	{
		
		std::vector<KVSerializer> serializers;
		
	public:
		
		void addKVSerializer( DynamicBuffer *buffer , int style )
		{
			serializers.push_back( KVSerializer( buffer , style ) );
		}
		
	
		KVWriters getWriters()
		{
			std::vector<KVWriter *>	writers;
			for (size_t i = 0 ; i < serializers.size(); i++)
				writers.push_back( &serializers[i] );
			return writers;
		}
	
	};
	
}
