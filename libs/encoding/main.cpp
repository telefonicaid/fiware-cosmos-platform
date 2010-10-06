
#include <iostream>
#include "test.pb.h"
#include <sstream>

/**
 Differnt codification styles
 */

#define KEY_VALUE_STYLE_STANDARD		0	// KeySize Key ValueSize Value
#define KEY_VALUE_STYLE_HASH			1	// Hash Key KeySize Key ValueSize Value
#define KEY_VALUE_STYLE_TXT				2	// TxtSize Text


typedef ::google::protobuf::Message SSMessage;



/* Interface to emit key-value pairs */

class KVWriter
{
public:
	virtual void emit( const SSMessage &key , const SSMessage &value)=0;
};

/** 
 A vector containing a simple vector of key-values 
 */

class KeyValueVector : public std::vector<KeyValueBuffer> 
{
};

/**
 Vector containing a vector of key , multiples values pairs
 */
 
class KeyMultiValueVector : public std::vector<KeyMultiValueBuffer>
{
public:

	void fill( KeyValueVector &keyValueVector )
	{
		clear();	// Just to make sure we are not re-using
		
		std::map<DataBuffer,size_t> position;	// Keep the position of this key ( if previously introducied )
		size_t pos = 0;

		for (size_t i = 0 ; i < keyValueVector.size() ; i++)
		{
			KeyValueBuffer keyValue = keyValueVector[i];
			std::map<DataBuffer,size_t>::iterator i = position.find( keyValue.key );
			
			if( i == position.end() )
			{

				KeyMultiValueBuffer item;
				item.key = keyValue.key;
				item.values.push_back(keyValue.value);
				
				push_back(item);
				position.insert( std::pair<DataBuffer,size_t>( keyValue.key , pos ) );
				pos++;
			}
			else
			{
				(*this)[i->second].values.push_back( keyValue.value );
			}		
		}
	}
};




/** Usefull function to get a key or a value */

int main( int arg, char *argv[] )
{
	return test();
	
}

/*

bool compare_pair( const Pair &p  , const  Pair &p2)
{
	return( p.a() < p2.a() );
}

void read( char *buffer , size_t len)
{
	
	Message m;
	
	m.ParsePartialFromArray(buffer, len);
	Codes codes = m.c();
	
	// Add an element in the process
	codes.add_c(101);
	
	
	// Sort codes
	::google::protobuf::RepeatedField< ::google::protobuf::int32 >* m_codes = codes.mutable_c();
	std::sort( m_codes->begin() , m_codes->end() );
	
	// Sort pairs
	::google::protobuf::RepeatedPtrField< ::Pair >* m_pairs = m.mutable_pair();
	
	
	// Sort pairs
	std::sort( m_pairs->begin() , m_pairs->end() , compare_pair );
	
	
	// Show codes
	for (int i = 0 ; i < codes.c_size() ;i ++)
		std::cout << codes.c(i) << " ";
	std::cout << std::endl;
	
	
	// Show pairs
	for (int i = 0 ; i < m.pair_size() ; i++)
		std::cout << "[ " << m.pair(i).a() << " , " << m.pair(i).b() << " ] ";
	std::cout << std::endl;
	
}




int main2( int arg, char *argv[] )
{
	
	char buffer[10000];
	size_t len;
	
	std::cout << "Testing PB\n";
	

	Message m;
	Codes *codes = m.mutable_c();
	
	for (int i = 0 ; i < 20 ; i++)
	{
		codes->add_c( rand()%1000 );
		
		Pair *p = m.add_pair();
		p->set_a(rand()%100);
		p->set_b(rand()%100);
	}
	
	len = m.ByteSize();
	m.SerializeToArray(buffer, 10000);

	
	std::cout << "Serialized in " << len << " bytes\n";

	
	read( buffer , len );
	
	return 0;
	
}

*/