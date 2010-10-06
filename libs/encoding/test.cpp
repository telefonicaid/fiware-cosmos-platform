
#include "Test.h"
#include "KVSerializer.h"
#include "KVDeSerializer.h"
#include <iostream>

namespace ss {
		
	void generatorCDRs( KVWriters &writers )
	{
		cdrFull c,c2;
		for (int i = 0 ; i < 10 ; i++)
		{
			c.set_a(i%2);
			c.set_b(i%2);
			c2.set_a(rand()%100);
			c2.set_b(rand()%100);
			
			writers[0]->emit( c, c2 );
			
			c.set_a(i%2);
			c.set_b(i%2);
			c2.set_a(rand()%100);
			c2.set_b(rand()%100);
			
			writers[1]->emit( c, c2 );
			
		}
		
	}

	// Map interface
	
	void parseCDRs( KeyValueVector& keyValues , KVWriters &writers )
	{
		cdrFull key,value;
		
		protocol::String output_key;
		protocol::Void output_value;
		
		for (size_t i = 0 ; i < keyValues.size() ; i++)
		{
			std::ostringstream o;
			
			keyValues[i].key.set( key );
			keyValues[i].value.set( value );
			
			o << "Map Item Key: " << key.str() << " Value: " << value.str() << std::endl;
			output_key.set_txt( o.str() );
		}
		
	}


	void printCDRs( KeyValueVector &keyValues )
	{
		cdrFull key,value;
		for (size_t i = 0 ; i < keyValues.size() ; i++)
		{
			keyValues[i].key.set( key );
			keyValues[i].value.set( value );
			
			std::cout << "Map Item Key: " << key.str() << " Value: " << value.str() << std::endl;
		}
		
	}

	void printCDRs(KeyMultiValueVector &keyMultiValues)
	{
		// Print outputs for reduce operations
		cdrFull key,value;
		for (size_t i = 0 ; i < keyMultiValues.size() ; i++)
		{
			keyMultiValues[i].key.set( key );
			
			std::cout << "Reduce Item Key: " << key.str();
			
			for(int s = 0 ; s < 2 ; s++)
			{
				std::cout << " Values["<<s<<"]: ";
				for (size_t v = 0 ; v < keyMultiValues[i].values[0].size() ; v++)
				{
					keyMultiValues[i].values[s][v].set( value );
					std::cout << value.str() << " ";
				}
			}
			
			std::cout << std::endl;
		}
		
	}

	int test()
	{
		// Dynamic buffers
		SimpleDynamicBuffer dynamicBuffer( 10000 );
		SimpleDynamicBuffer dynamicBuffer2( 10000 );
		
		KVSerializerCollection output;
		output.addKVSerializer(&dynamicBuffer, KEY_VALUE_STYLE_STANDARD);
		output.addKVSerializer(&dynamicBuffer2, KEY_VALUE_STYLE_STANDARD);
		KVWriters writers = output.getWriters();
		
		// Call a generator
		generatorCDRs( writers );
		
		
		// Serialize to static buffers
		SimpleStaticBuffer staticBuffer( serializer.ByteSize() );
		SimpleStaticBuffer staticBuffer2( serializer2.ByteSize() );
		serializer.SerializeToStaticBuffer( staticBuffer );
		serializer2.SerializeToStaticBuffer( staticBuffer2 );
		
		std::cout << "Serialized in " << staticBuffer.getLength() << " bytes\n";
		std::cout << "Serialized in " << staticBuffer2.getLength() << " bytes\n";

		
		// Deserialization in "map" style
		KVDeSerializer deSerializer( &staticBuffer );
		KeyValueVector keyValues;
		deSerializer.fill( keyValues );
		
		KVDeSerializer deSerializer2( &staticBuffer2 );
		KeyValueVector keyValues2;
		deSerializer.fill( keyValues2 );
		
		// Deserialization as "reduce" style
		KVDeSerializerCollection deSerializerCollection;
		deSerializerCollection.addKVDeSerializer( &staticBuffer );
		deSerializerCollection.addKVDeSerializer( &staticBuffer2 );
		KeyMultiValueVector keyMultiValues;
		deSerializerCollection.fill( keyMultiValues );

		
		// Print outputs for map operations
		printCDRs(keyValues);
		printCDRs(keyValues2);
		

		printCDRs(keyMultiValues);
		
		return 0;
	}

}


int main( int arg, char *argv[] )
{
	return ss::test();
}
