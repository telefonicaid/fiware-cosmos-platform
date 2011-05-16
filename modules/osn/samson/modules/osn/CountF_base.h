
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_osn_CountF_BASE
#define _H_SAMSON_osn_CountF_BASE


#include <samson/modules/system/Float.h>
#include <samson/modules/system/String.h>


namespace ss{
namespace osn{


	class CountF_base : public ss::DataInstance{

	public:
	::ss::system::Float count;
	::ss::system::String element;

	CountF_base() : ss::DataInstance(){
	}

	~CountF_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += count.parse(data+offset);
		offset += element.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += count.serialize(data+offset);
		offset += element.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::Float::size(data+offset);
		offset += ::ss::system::String::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return count.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing count
			int tmp = ::ss::system::Float::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing element
			int tmp = ::ss::system::String::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( CountF_base *other ){
		count.copyFrom(&other->count);
		element.copyFrom(&other->element);
	};

	std::string str(){
		std::ostringstream o;
		o << count.str();

		o<<" ";
		o << element.str();

		o<<" ";
		return o.str();
	}

	}; //class CountF_base

} // end of namespace ss
} // end of namespace osn

#endif
