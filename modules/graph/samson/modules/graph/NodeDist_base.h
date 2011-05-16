
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_graph_NodeDist_BASE
#define _H_SAMSON_graph_NodeDist_BASE


#include <samson/modules/graph/Link.h>
#include <samson/modules/system/UInt.h>


namespace ss{
namespace graph{


	class NodeDist_base : public ss::DataInstance{

	public:
	::ss::system::UInt id;
	::ss::graph::Link *links;
	int links_length;
	int links_max_length;
	::ss::system::UInt distance;

	NodeDist_base() : ss::DataInstance(){
		links_length=0;
		links_max_length=0;
		links = NULL;
	}

	~NodeDist_base() {
		if( links )
			delete[] links ;
	}

	int parse(char *data){
		int offset=0;
		offset += id.parse(data+offset);
		{ //Parsing vector links
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
		 	linksSetLength( _length );
			for (int i = 0 ; i < (int)links_length ; i++){
				offset += links[i].parse(data+offset);
			}
		}
		offset += distance.parse(data+offset);
		return offset;
	}

	int serialize(char *data){
		int offset=0;
		offset += id.serialize(data+offset);
		{ //Serialization vector links
			offset += ss::staticVarIntSerialize( data+offset , links_length );
			for (int i = 0 ; i < (int)links_length ; i++){
				offset += links[i].serialize(data+offset);
			}
		}
		offset += distance.serialize(data+offset);
		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		offset += ::ss::system::UInt::size(data+offset);
		{ //Getting size of vector links
			size_t _length;
			offset += ss::staticVarIntParse( data+offset , &_length );
			::ss::graph::Link _tmp;
			for (int i = 0 ; i < (int)_length ; i++){
				offset += ::ss::graph::Link::size(data+offset);
			}
		}
		offset += ::ss::system::UInt::size(data+offset);
		return offset;
	}

	int hash(int max_num_partitions){
		return id.hash(max_num_partitions);
	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing id
			int tmp = ::ss::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}
		{ // Comparing vector links
			size_t _length1,_length2;
			*offset1 += ss::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += ss::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++){
				{ // comparing links[i]
					int tmp = ::ss::graph::Link::compare(data1,data2,offset1 , offset2);
					if( tmp != 0) return tmp;
				}
			}
		}
		{ // comparing distance
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

	void linksSetLength(int _length){
		if( _length > links_max_length){ 
			::ss::graph::Link *_previous = links;
			int previous_length = links_length;
			if(links_max_length == 0) links_max_length = _length;
			while(links_max_length < _length) links_max_length *= 2;
			links = new ::ss::graph::Link[links_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					links[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		links_length=_length;
	}

	::ss::graph::Link* linksAdd(){
		linksSetLength( links_length + 1 );
		return &links[links_length-1];
	}

	void copyFrom( NodeDist_base *other ){
		id.copyFrom(&other->id);
			{ // CopyFrom field links
				linksSetLength( other->links_length);
				for (int i = 0 ; i < links_length ; i++){
					links[i].copyFrom(&other->links[i]);
				}
			}
		distance.copyFrom(&other->distance);
	};

	std::string str(){
		std::ostringstream o;
		o << id.str();

		o<<" ";
		{// toString of vector links
			for(int i = 0 ; i < links_length ; i++){
				o << links[i].str();
				 o << " ";
			}
		}
		o<<" ";
		o << distance.str();

		o<<" ";
		return o.str();
	}

	}; //class NodeDist_base

} // end of namespace ss
} // end of namespace graph

#endif
