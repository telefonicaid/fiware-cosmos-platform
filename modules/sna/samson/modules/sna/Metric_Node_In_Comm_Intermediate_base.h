
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_Metric_Node_In_Comm_Intermediate_BASE
#define _H_SAMSON_sna_Metric_Node_In_Comm_Intermediate_BASE


#include <samson/modules/sna/Clique.h>
#include <samson/modules/system/UInt.h>


namespace ss{
namespace sna{


	class Metric_Node_In_Comm_Intermediate_base : public ss::DataInstance{

	public:
	::ss::system::UInt node_id;
	::ss::sna::Clique comm_id;
	::ss::system::UInt sum_weights_node;
	::ss::system::UInt count_links_node;

	Metric_Node_In_Comm_Intermediate_base() : ss::DataInstance(){
	}

	~Metric_Node_In_Comm_Intermediate_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += node_id.parse(data+offset);
		offset += comm_id.parse(data+offset);
		offset += sum_weights_node.parse(data+offset);
		offset += count_links_node.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += node_id.serialize(data+offset);
		offset += comm_id.serialize(data+offset);
		offset += sum_weights_node.serialize(data+offset);
		offset += count_links_node.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::sna::Clique::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
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
		{ // comparing comm_id
			int tmp = ::ss::sna::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing sum_weights_node
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_links_node
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( Metric_Node_In_Comm_Intermediate_base *other ){
		node_id.copyFrom(&other->node_id);
		comm_id.copyFrom(&other->comm_id);
		sum_weights_node.copyFrom(&other->sum_weights_node);
		count_links_node.copyFrom(&other->count_links_node);
	};

	std::string str(){
		std::ostringstream o;
		o << node_id.str();

		o<<" ";
		o << comm_id.str();

		o<<" ";
		o << sum_weights_node.str();

		o<<" ";
		o << count_links_node.str();

		o<<" ";
		return o.str();
	}

	}; //class Metric_Node_In_Comm_Intermediate_base

} // end of namespace ss
} // end of namespace sna

#endif
