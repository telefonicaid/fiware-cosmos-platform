
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_light_Metric_Communities_BASE
#define _H_SAMSON_sna_light_Metric_Communities_BASE


#include <samson/modules/system/UInt.h>


namespace ss{
namespace sna_light{


	class Metric_Communities_base : public ss::DataInstance{

	public:
	::ss::system::UInt id_comm;
	::ss::system::UInt count_nodes;
	::ss::system::UInt count_nodes_strong;
	::ss::system::UInt count_nodes_associated;
	::ss::system::UInt count_nodes_competitor;
	::ss::system::UInt count_nodes_telefonica;
	::ss::system::UInt percent_nodes_competitor;
	::ss::system::UInt percent_nodes_strong;
	::ss::system::UInt percent_nodes_associated;
	::ss::system::UInt percent_nodes_telefonica;
	::ss::system::UInt count_regular_existing_links;
	::ss::system::UInt count_regular_possible_links;
	::ss::system::UInt count_all_existing_links;
	::ss::system::UInt count_all_possible_links;
	::ss::system::UInt density_regular_members;
	::ss::system::UInt density_all_members;
	::ss::system::UInt cohesion_regular_members;
	::ss::system::UInt cohesion_all_members;
	::ss::system::UInt value_total;
	::ss::system::UInt value_per_node;
	::ss::system::UInt value_per_edge;
	::ss::system::UInt reach_one_step;
	::ss::system::UInt reach_one_step_competitors;
	::ss::system::UInt reach_comm_on_step;

	Metric_Communities_base() : ss::DataInstance(){
	}

	~Metric_Communities_base() {
	}

	int parse(char *data){
		int offset=0;
		offset += id_comm.parse(data+offset);
		offset += count_nodes.parse(data+offset);
		offset += count_nodes_strong.parse(data+offset);
		offset += count_nodes_associated.parse(data+offset);
		offset += count_nodes_competitor.parse(data+offset);
		offset += count_nodes_telefonica.parse(data+offset);
		offset += percent_nodes_competitor.parse(data+offset);
		offset += percent_nodes_strong.parse(data+offset);
		offset += percent_nodes_associated.parse(data+offset);
		offset += percent_nodes_telefonica.parse(data+offset);
		offset += count_regular_existing_links.parse(data+offset);
		offset += count_regular_possible_links.parse(data+offset);
		offset += count_all_existing_links.parse(data+offset);
		offset += count_all_possible_links.parse(data+offset);
		offset += density_regular_members.parse(data+offset);
		offset += density_all_members.parse(data+offset);
		offset += cohesion_regular_members.parse(data+offset);
		offset += cohesion_all_members.parse(data+offset);
		offset += value_total.parse(data+offset);
		offset += value_per_node.parse(data+offset);
		offset += value_per_edge.parse(data+offset);
		offset += reach_one_step.parse(data+offset);
		offset += reach_one_step_competitors.parse(data+offset);
		offset += reach_comm_on_step.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += id_comm.serialize(data+offset);
		offset += count_nodes.serialize(data+offset);
		offset += count_nodes_strong.serialize(data+offset);
		offset += count_nodes_associated.serialize(data+offset);
		offset += count_nodes_competitor.serialize(data+offset);
		offset += count_nodes_telefonica.serialize(data+offset);
		offset += percent_nodes_competitor.serialize(data+offset);
		offset += percent_nodes_strong.serialize(data+offset);
		offset += percent_nodes_associated.serialize(data+offset);
		offset += percent_nodes_telefonica.serialize(data+offset);
		offset += count_regular_existing_links.serialize(data+offset);
		offset += count_regular_possible_links.serialize(data+offset);
		offset += count_all_existing_links.serialize(data+offset);
		offset += count_all_possible_links.serialize(data+offset);
		offset += density_regular_members.serialize(data+offset);
		offset += density_all_members.serialize(data+offset);
		offset += cohesion_regular_members.serialize(data+offset);
		offset += cohesion_all_members.serialize(data+offset);
		offset += value_total.serialize(data+offset);
		offset += value_per_node.serialize(data+offset);
		offset += value_per_edge.serialize(data+offset);
		offset += reach_one_step.serialize(data+offset);
		offset += reach_one_step_competitors.serialize(data+offset);
		offset += reach_comm_on_step.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		offset += ::ss::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return id_comm.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing id_comm
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_nodes
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_nodes_strong
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_nodes_associated
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_nodes_competitor
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_nodes_telefonica
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing percent_nodes_competitor
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing percent_nodes_strong
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing percent_nodes_associated
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing percent_nodes_telefonica
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_regular_existing_links
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_regular_possible_links
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_all_existing_links
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing count_all_possible_links
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing density_regular_members
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing density_all_members
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing cohesion_regular_members
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing cohesion_all_members
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing value_total
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing value_per_node
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing value_per_edge
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing reach_one_step
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing reach_one_step_competitors
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // comparing reach_comm_on_step
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

	void copyFrom( Metric_Communities_base *other ){
		id_comm.copyFrom(&other->id_comm);
		count_nodes.copyFrom(&other->count_nodes);
		count_nodes_strong.copyFrom(&other->count_nodes_strong);
		count_nodes_associated.copyFrom(&other->count_nodes_associated);
		count_nodes_competitor.copyFrom(&other->count_nodes_competitor);
		count_nodes_telefonica.copyFrom(&other->count_nodes_telefonica);
		percent_nodes_competitor.copyFrom(&other->percent_nodes_competitor);
		percent_nodes_strong.copyFrom(&other->percent_nodes_strong);
		percent_nodes_associated.copyFrom(&other->percent_nodes_associated);
		percent_nodes_telefonica.copyFrom(&other->percent_nodes_telefonica);
		count_regular_existing_links.copyFrom(&other->count_regular_existing_links);
		count_regular_possible_links.copyFrom(&other->count_regular_possible_links);
		count_all_existing_links.copyFrom(&other->count_all_existing_links);
		count_all_possible_links.copyFrom(&other->count_all_possible_links);
		density_regular_members.copyFrom(&other->density_regular_members);
		density_all_members.copyFrom(&other->density_all_members);
		cohesion_regular_members.copyFrom(&other->cohesion_regular_members);
		cohesion_all_members.copyFrom(&other->cohesion_all_members);
		value_total.copyFrom(&other->value_total);
		value_per_node.copyFrom(&other->value_per_node);
		value_per_edge.copyFrom(&other->value_per_edge);
		reach_one_step.copyFrom(&other->reach_one_step);
		reach_one_step_competitors.copyFrom(&other->reach_one_step_competitors);
		reach_comm_on_step.copyFrom(&other->reach_comm_on_step);
	};

	std::string str(){
		std::ostringstream o;
		o << id_comm.str();

		o<<" ";
		o << count_nodes.str();

		o<<" ";
		o << count_nodes_strong.str();

		o<<" ";
		o << count_nodes_associated.str();

		o<<" ";
		o << count_nodes_competitor.str();

		o<<" ";
		o << count_nodes_telefonica.str();

		o<<" ";
		o << percent_nodes_competitor.str();

		o<<" ";
		o << percent_nodes_strong.str();

		o<<" ";
		o << percent_nodes_associated.str();

		o<<" ";
		o << percent_nodes_telefonica.str();

		o<<" ";
		o << count_regular_existing_links.str();

		o<<" ";
		o << count_regular_possible_links.str();

		o<<" ";
		o << count_all_existing_links.str();

		o<<" ";
		o << count_all_possible_links.str();

		o<<" ";
		o << density_regular_members.str();

		o<<" ";
		o << density_all_members.str();

		o<<" ";
		o << cohesion_regular_members.str();

		o<<" ";
		o << cohesion_all_members.str();

		o<<" ";
		o << value_total.str();

		o<<" ";
		o << value_per_node.str();

		o<<" ";
		o << value_per_edge.str();

		o<<" ";
		o << reach_one_step.str();

		o<<" ";
		o << reach_one_step_competitors.str();

		o<<" ";
		o << reach_comm_on_step.str();

		o<<" ";
		return o.str();
	}

	}; //class Metric_Communities_base

} // end of namespace ss
} // end of namespace sna_light

#endif
