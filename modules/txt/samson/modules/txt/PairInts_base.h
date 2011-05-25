
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_txt_PairInts_BASE
#define _H_SAMSON_txt_PairInts_BASE


#include <samson/modules/system/UInt.h>


namespace samson{
namespace txt{


	class PairInts_base : public samson::DataInstance{

	public:
	::samson::system::UInt i1;
	::samson::system::UInt i2;

	PairInts_base() : samson::DataInstance(){
	}

	~PairInts_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += i1.parse(data+offset);
		offset += i2.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += i1.serialize(data+offset);
		offset += i2.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::samson::system::UInt::size(data+offset);
		offset += ::samson::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return i1.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing i1
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing i2
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}

	void copyFrom( PairInts_base *other ){
		i1.copyFrom(&other->i1);
		i2.copyFrom(&other->i2);
	};

	std::string str(){
		std::ostringstream o;
		o << i1.str();

		o<<" ";
		o << i2.str();

		o<<" ";
		return o.str();
	}

	}; //class PairInts_base

} // end of namespace samson
} // end of namespace txt

#endif
