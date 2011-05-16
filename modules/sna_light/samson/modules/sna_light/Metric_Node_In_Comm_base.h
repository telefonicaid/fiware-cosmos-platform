
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_light_Metric_Node_In_Comm_BASE
#define _H_SAMSON_sna_light_Metric_Node_In_Comm_BASE


#include <samson/modules/sna_light/Clique.h>
#include <samson/modules/system/UInt.h>


namespace ss{
namespace sna_light{


	class Metric_Node_In_Comm_base : public ss::DataInstance{

	public:
	::ss::sna_light::Clique comm;
	::ss::sna_light::Clique orphan;
	::ss::system::UInt id_comm;
	::ss::system::UInt node_id;
	::ss::system::UInt absolute_weights;
	::ss::system::UInt realtive_weights;
	::ss::system::UInt absolute_neighbours;
	::ss::system::UInt relative_neighbours;

	Metric_Node_In_Comm_base() : ss::DataInstance(){
	}

	~Metric_Node_In_Comm_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += comm.parse(data+offset);
		offset += orphan.parse(data+offset);
		offset += id_comm.parse(data+offset);
		offset += node_id.parse(data+offset);
		offset += absolute_weights.parse(data+offset);
		offset += realtive_weights.parse(data+offset);
		offset += absolute_neighbours.parse(data+offset);
		offset += relative_neighbours.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += comm.serialize(data+offset);
		offset += orphan.serialize(data+offset);
		offset += id_comm.serialize(data+offset);
		offset += node_id.serialize(data+offset);
		offset += absolute_weights.serialize(data+offset);
		offset += realtive_weights.serialize(data+offset);
		offset += absolute_neighbours.serialize(data+offset);
		offset += relative_neighbours.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::sna_light::Clique::size(data+offset);
		offset += ::ss::sna_light::Clique::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
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
		{ // comparing orphan
			int tmp = ::ss::sna_light::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing id_comm
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing node_id
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing absolute_weights
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing realtive_weights
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing absolute_neighbours
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing relative_neighbours
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

	void copyFrom( Metric_Node_In_Comm_base *other ){
		comm.copyFrom(&other->comm);
		orphan.copyFrom(&other->orphan);
		id_comm.copyFrom(&other->id_comm);
		node_id.copyFrom(&other->node_id);
		absolute_weights.copyFrom(&other->absolute_weights);
		realtive_weights.copyFrom(&other->realtive_weights);
		absolute_neighbours.copyFrom(&other->absolute_neighbours);
		relative_neighbours.copyFrom(&other->relative_neighbours);
	};

	std::string str(){
		std::ostringstream o;
		o << comm.str();

		o<<" ";
		o << orphan.str();

		o<<" ";
		o << id_comm.str();

		o<<" ";
		o << node_id.str();

		o<<" ";
		o << absolute_weights.str();

		o<<" ";
		o << realtive_weights.str();

		o<<" ";
		o << absolute_neighbours.str();

		o<<" ";
		o << relative_neighbours.str();

		o<<" ";
		return o.str();
	}

	}; //class Metric_Node_In_Comm_base

} // end of namespace ss
} // end of namespace sna_light

#endif
