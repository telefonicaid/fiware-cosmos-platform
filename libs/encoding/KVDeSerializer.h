
#pragma once


namespace ss {

class KVDeSerializer
{
public:
	
	StaticBuffer *_buffer;	// Buffer holding data
	
	char* dataBuffer;			// Pointer to where data begins
	size_t lengthDataBuffer;	// Length of the payload
	
	protocol::KVSetHeader header;		// Decoded header
	
	KVDeSerializer( StaticBuffer *buffer )
	{
		_buffer = buffer;
		
		// Read the header
		size_t header_size;
		char *data = _buffer->getData();
		size_t offset = staticVarInt64Parse(data, header_size);
		header.ParseFromArray(data+offset, header_size);
		offset += header_size;
		
		// Get the data pointer when finish the header
		dataBuffer = data + offset;
		lengthDataBuffer = _buffer->getLength() - offset;
	}
	
	// Functions to get the necessary elements for using this key-values in operations
	
	void fill (KeyValueVector& keyValueVector )
	{
		keyValueVector.clear();	// Just to make sure it is not re-used
		
		switch (header.style()) {
			case 0:
			{
				// Default serialization
				size_t offset = 0;
				while ( offset < lengthDataBuffer) 
				{
					KeyValueBuffer keyValueBuffer;
					offset += readSizeAndDataBuffer( dataBuffer+offset , keyValueBuffer.key );
					offset += readSizeAndDataBuffer( dataBuffer+offset , keyValueBuffer.value );
					
					keyValueVector.push_back( keyValueBuffer );
				}
				
			}
				break;
			default:
				break;
		}		
	}
	
private:
	static inline size_t readSizeAndDataBuffer( char *data , DataBuffer &buffer )
	{
		size_t offset = staticVarInt64Parse( data , buffer.length );
		buffer.data = data + offset;
		offset += buffer.length;
		
		return offset;
	}
	
};



class KVDeSerializerCollection
{
	std::vector<KVDeSerializer> deserializers;
	
	
public:
	
	void addKVDeSerializer( StaticBuffer *buffer )
	{
		deserializers.push_back( KVDeSerializer( buffer ) );
	}

	
	void fill(  KeyMultiValueVector& output )
	{
		size_t num_deserializers = deserializers.size();
		output.clear();	// Make sure we do not do strange stuff

		// Map vectors that keep the position of the key
		std::map<DataBuffer,size_t> position;
		size_t pos = 0;
		
		for (size_t s = 0 ; s < num_deserializers ; s++)
		{
			KeyValueVector keyValueVector;
			deserializers[s].fill( keyValueVector );
			
			for (size_t i = 0 ; i < keyValueVector.size() ; i++)
			{
				KeyValueBuffer keyValue = keyValueVector[i];
				std::map<DataBuffer,size_t>::iterator i = position.find( keyValue.key );
				
				if( i == position.end() )
				{
					
					KeyMultiValueBuffer item( num_deserializers );
					item.key = keyValue.key;
					item.values[s].push_back(keyValue.value);
					
					output.push_back(item);
					position.insert( std::pair<DataBuffer,size_t>( keyValue.key , pos ) );
					pos++;
				}
				else
				{
					output[i->second].values[s].push_back( keyValue.value );
				}		
			}			
		}
		

		
		

	}
	
	
};


}
