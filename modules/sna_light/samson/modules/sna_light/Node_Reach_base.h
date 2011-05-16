
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_light_Node_Reach_BASE
#define _H_SAMSON_sna_light_Node_Reach_BASE


#include <samson/modules/system/Int8.h>
#include <samson/modules/system/UInt.h>


namespace ss{
namespace sna_light{


	class Node_Reach_base : public ss::DataInstance{

	public:
	::ss::system::UInt node_id;
	::ss::system::UInt node_reached;
	::ss::system::UInt weight;
	::ss::system::Int8 flags;

	Node_Reach_base() : ss::DataInstance(){
	}

	~Node_Reach_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += node_id.parse(data+offset);
		offset += node_reached.parse(data+offset);
		offset += weight.parse(data+offset);
		offset += flags.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += node_id.serialize(data+offset);
		offset += node_reached.serialize(data+offset);
		offset += weight.serialize(data+offset);
		offset += flags.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::Int8::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return node_id.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing node_id
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing node_reached
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing weight
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing flags
			int tmp = ::ss::system::Int8::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( Node_Reach_base *other ){
		node_id.copyFrom(&other->node_id);
		node_reached.copyFrom(&other->node_reached);
		weight.copyFrom(&other->weight);
		flags.copyFrom(&other->flags);
	};

	std::string str(){
		std::ostringstream o;
		o << node_id.str();

		o<<" ";
		o << node_reached.str();

		o<<" ";
		o << weight.str();

		o<<" ";
		o << flags.str();

		o<<" ";
		return o.str();
	}

	}; //class Node_Reach_base

} // end of namespace ss
} // end of namespace sna_light

#endif
