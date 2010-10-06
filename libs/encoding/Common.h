
#pragma once


/**
 Differnt codification styles
 */

#define KEY_VALUE_STYLE_STANDARD		0	// KeySize Key ValueSize Value
#define KEY_VALUE_STYLE_HASH			1	// Hash Key KeySize Key ValueSize Value
#define KEY_VALUE_STYLE_TXT				2	// TxtSize Text


typedef ::google::protobuf::Message SSMessage;

namespace ss {

/* Interface to emit key-value pairs */

class KVWriter
{
public:
	virtual void emit( const SSMessage &key , const SSMessage &value)=0;
};


/** Interface that the KVManager has to satify to work with Buffers */
	
class KVManagerBuffersInterface
{
	
	StaticBuffer *newStaticBuffer( size_t size )
	
};

typedef std::vector<KVWriter *> KVWriters;

/**
 Buffer to hold data of a key or a value
 */

struct DataBuffer
{
	char *data;
	size_t length;
	
	bool operator <(const DataBuffer& b2) const
	{
		if ( length < b2.length)
			return true;
		if ( length > b2.length)
			return false;
		
		// Same length
		for (size_t i = 0 ; i < length ; i++)
		{
			if( data[i] < b2.data[i] )
				return true;
			if( data[i] > b2.data[i] )
				return false;
		}
		
		// The same
		return false;
	}

	/** Function to fill a Google Protocol Buffer structure */
	inline void set( SSMessage &message )
	{
		message.ParseFromArray( (const void*)data ,length );
	}
	
};

typedef struct KeyValueBuffer
{
	DataBuffer key;
	DataBuffer value;
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

typedef std::vector<DataBuffer> ValuesVector;

struct KeyMultiValueBuffer
{
	KeyMultiValueBuffer( int num_outputs)
	{
		for (int i = 0 ; i < num_outputs ; i++)
			values.push_back( ValuesVector() );	
	}
	
	DataBuffer key;
	std::vector<ValuesVector> values;
};

class KeyMultiValueVector : public std::vector<KeyMultiValueBuffer>
{

};
	
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