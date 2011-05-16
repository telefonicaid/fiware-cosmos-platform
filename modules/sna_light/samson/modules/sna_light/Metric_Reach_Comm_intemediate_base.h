
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_light_Metric_Reach_Comm_intemediate_BASE
#define _H_SAMSON_sna_light_Metric_Reach_Comm_intemediate_BASE


#include <samson/modules/sna_light/Clique.h>


namespace ss{
namespace sna_light{


	class Metric_Reach_Comm_intemediate_base : public ss::DataInstance{

	public:
	::ss::sna_light::Clique comm;
	::ss::sna_light::Clique reach_nodes;
	::ss::sna_light::Clique reach_nodes_competitors;

	Metric_Reach_Comm_intemediate_base() : ss::DataInstance(){
	}

	~Metric_Reach_Comm_intemediate_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += comm.parse(data+offset);
		offset += reach_nodes.parse(data+offset);
		offset += reach_nodes_competitors.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += comm.serialize(data+offset);
		offset += reach_nodes.serialize(data+offset);
		offset += reach_nodes_competitors.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::sna_light::Clique::size(data+offset);
		offset += ::ss::sna_light::Clique::size(data+offset);
		offset += ::ss::sna_light::Clique::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return comm.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing comm
			int tmp = ::ss::sna_light::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing reach_nodes
			int tmp = ::ss::sna_light::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing reach_nodes_competitors
			int tmp = ::ss::sna_light::Clique::compare(data1,data2,offset1 , offset2);
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

	void copyFrom( Metric_Reach_Comm_intemediate_base *other ){
		comm.copyFrom(&other->comm);
		reach_nodes.copyFrom(&other->reach_nodes);
		reach_nodes_competitors.copyFrom(&other->reach_nodes_competitors);
	};

	std::string str(){
		std::ostringstream o;
		o << comm.str();

		o<<" ";
		o << reach_nodes.str();

		o<<" ";
		o << reach_nodes_competitors.str();

		o<<" ";
		return o.str();
	}

	}; //class Metric_Reach_Comm_intemediate_base

} // end of namespace ss
} // end of namespace sna_light

#endif
