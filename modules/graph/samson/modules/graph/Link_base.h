
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_graph_Link_BASE
#define _H_SAMSON_graph_Link_BASE


#include <samson/modules/system/UInt.h>


namespace samson{
namespace graph{


	class Link_base : public samson::DataInstance{

	public:
	::samson::system::UInt id;
	::samson::system::UInt weight;

	Link_base() : samson::DataInstance(){
	}

	~Link_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += id.parse(data+offset);
		offset += weight.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += id.serialize(data+offset);
		offset += weight.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::samson::system::UInt::size(data+offset);
		offset += ::samson::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return id.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing id
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing weight
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

	void copyFrom( Link_base *other ){
		id.copyFrom(&other->id);
		weight.copyFrom(&other->weight);
	};

	std::string str(){
		std::ostringstream o;
		o << id.str();

		o<<" ";
		o << weight.str();

		o<<" ";
		return o.str();
	}

	}; //class Link_base

} // end of namespace samson
} // end of namespace graph

#endif
