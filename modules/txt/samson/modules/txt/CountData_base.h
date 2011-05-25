
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_txt_CountData_BASE
#define _H_SAMSON_txt_CountData_BASE


#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace txt{


	class CountData_base : public samson::DataInstance{

	public:
	::samson::system::String word;
	::samson::system::UInt count;

	CountData_base() : samson::DataInstance(){
	}

	~CountData_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += word.parse(data+offset);
		offset += count.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += word.serialize(data+offset);
		offset += count.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::samson::system::String::size(data+offset);
		offset += ::samson::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return word.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing word
			int tmp = ::samson::system::String::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count
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

	void copyFrom( CountData_base *other ){
		word.copyFrom(&other->word);
		count.copyFrom(&other->count);
	};

	std::string str(){
		std::ostringstream o;
		o << word.str();

		o<<" ";
		o << count.str();

		o<<" ";
		return o.str();
	}

	}; //class CountData_base

} // end of namespace samson
} // end of namespace txt

#endif
