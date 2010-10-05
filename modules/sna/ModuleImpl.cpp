
#include "Module.h"
#include <samson/modules/base/Module.h>

#define GET_CDRS_INIT                   int pos=0; int pos_field=0;
#define GET_CDRS_NEXT_FIELD             pos_field=pos; while( line[pos] != '|' ) pos++; line[pos]='\0'; pos++;
#define GET_CDRS_GET_NUMBER( num )      num = atoll( &line[pos_field] );
#define GET_CDRS_GET_DURATION( num )    num = atoll( &line[pos_field] );


/**

Module: SNA operations (cdrs, graph, cliques, communities,...)
Operation: parse_cdrs
Inputs: system.String-system.Void 
Outputs: base.UInt-sna.CDR 

*/


void ss::sna::parse_cdrs::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs )
{
	ss::system::String s;
	ss::base::UInt node;
	ss::sna::CDR cdr;
	
	for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
	{
		s.parse( inputs[0].kvs[i]->key );
		char *line = (char*)s.content.c_str();
		
		GET_CDRS_INIT
		
		GET_CDRS_NEXT_FIELD
		// First field useless
		
		GET_CDRS_NEXT_FIELD
		GET_CDRS_GET_NUMBER( node.value )
		
		GET_CDRS_NEXT_FIELD
		GET_CDRS_GET_NUMBER( cdr.node )
		
		outputs[0]->emit( &node , &cdr );
		
	}
		
}

/**

Module: SNA operations (cdrs, graph, cliques, communities,...)
Operation: spread_cdrs_to_links
Inputs: base.UInt-sna.CDR 
Outputs: base.UInt2-system.Void 

*/


void ss::sna::spread_cdrs_to_links::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs )
{
	ss::base::UInt node;
	ss::sna::CDR cdr;
	
	ss::base::UInt2 pair;
	ss::system::Void void_data;
	
	for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
	{
		node.parse( inputs[0].kvs[i]->key );
		cdr.parse( inputs[0].kvs[i]->value );

		if( node.value > cdr.node )
		{
			pair.value = node.value;
			pair.value2 = cdr.node;
		}
		else
		{
			pair.value2 = node.value;
			pair.value = cdr.node;
		}
		
		outputs[0]->emit( &pair , &void_data );
		
		
	}

	// Your code goes here
}

/**

Module: SNA operations (cdrs, graph, cliques, communities,...)
Operation: accumulate_links
Inputs: base.UInt2-system.Void base.UInt2-base.UInt 
Outputs: base.UInt2-base.UInt 

*/


void ss::sna::accumulate_links::run(  ss::KVSET* inputs , std::vector<ss::KVWriter*>& outputs )
{
	ss::base::UInt2 key;
	ss::base::UInt total;
	

	// Just count the number of inputs
	if( inputs[0].num_kvs != 0)
		key.parse( inputs[0].kvs[0]->key );
	else
		key.parse( inputs[1].kvs[0]->key );
	
	if( inputs[1].kvs != 0)
		total.parse( inputs[1].kvs[0]->key );
	else
		total.value = 0;

	// add the new entried
	total.value += inputs[0].num_kvs;

	
	outputs[0]->emit( &key , &total);
	
	
}

