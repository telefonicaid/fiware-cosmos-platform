#include "TEST_Module.h"

/*#define GET_INIT_MX pos=0; pos_field=0; char *offset; int pos_cell=0; int length_cell=0; int cont=0; int length_line=strlen(line);
#define GET_NEXT_FIELD pos_field=pos; while( line[pos] != '|' and line[pos] != '\0' ) pos++; line[pos]='\0';  pos++;
#define GET_INT( _int ) _int = atoi(&line[pos_field]);
#define GET_LONG( _long ) _long = atoll( &line[pos_field] );
#define GET_DOUBLE( _double  ) _double = atof(&line[pos_field]);



void MACRO_mobmx_comp_parse_v1_results::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Inputs: txt
	// Outputs
	MRData_UInt node;
	MRData_Results1 results;

	for (std::vector<char*>::iterator iter = lines.begin(); iter < lines.end() ; iter++)
	{
		MR_PARSER_INIT(*iter);
		GET_INIT_MX
		GET_NEXT_FIELD
		GET_LONG(node.value)
		GET_NEXT_FIELD
		GET_INT(results.mobDegree)
		GET_NEXT_FIELD
		GET_INT(results.homeBts)
		GET_NEXT_FIELD
		GET_NEXT_FIELD
		GET_INT(results.workBts)
		writer->emit(&node,&results);
	}
}

void MACRO_mobmx_spread_results1_homepoi_workpoi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt node;
	MRData_Results1 results;
	//Outputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_Void mr_void;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		results.parse(inputs[0].kvs[i].value);
		nodbts.num1 = node.value;
		// Home place
		nodbts.num2 = results.homeBts;
		writer[0]->emit(&nodbts,&mr_void);
		// Work place
		nodbts.num2 = results.workBts;
		writer[1]->emit(&nodbts,&mr_void);
	}
}

void MACRO_mobmx_join_v1_v2_results_home::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_Poi poi;
	//Outputs
	MRData_UInt node;
	MRData_UInt join;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{	
		nodbts.parse(inputs[0].kvs[i].key);
		poi.parse(inputs[0].kvs[i].value);
		node.value = nodbts.num1;
		
		if(poi.labelgroupnodebts == 3 && poi.confidentnodebts == 1) // Label level 1 of home in v2
		{
			if(inputs[1].num_kvs != 0)	// Match with v1
			{
				join.value = 0;
				writer[0]->emit(&node,&join);
			}
			else
			{
				join.value = 1;
				writer[0]->emit(&node,&join);
			}
		}
		else // POI different to Home with match in V1.
		{
			if(inputs[1].num_kvs != 0)
			{
				join.value = 2;
				writer[0]->emit(&node,&join);
			}
		}
	}
}


void MACRO_mobmx_join_v1_v2_results_work::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_Poi poi;
	//Outputs
	MRData_UInt node;
	MRData_UInt join;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{	
		nodbts.parse(inputs[0].kvs[i].key);
		poi.parse(inputs[0].kvs[i].value);
		node.value = nodbts.num1;
		
		if((poi.labelgroupnodebts == 1 || poi.labelgroupnodebts == 2) 
				&& poi.confidentnodebts == 1) // Label level 1 of work in v2
		{
			if(inputs[1].num_kvs != 0)	// Match with v1
			{
				join.value = 0;
				writer[0]->emit(&node,&join);
			}
			else
			{
				join.value = 1;
				writer[0]->emit(&node,&join);
			}
		}
		else // POI different to Work with match in V1.
		{
			if(inputs[1].num_kvs != 0)
			{
				join.value = 2;
				writer[0]->emit(&node,&join);
			}
		}
	}
}

void MACRO_mobmx_get_node_comparative::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt node;
	MRData_UInt join;
	//Outputs
	MRData_Void mr_void;

	int match = 0;

	node.parse(inputs[0].kvs[0].key);

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		join.parse(inputs[0].kvs[i].value);
		if(join.value == 0)
		{
			match = 1;
		}
		else if(join.value == 2)
		{
			match = 2;
		}
	}
	join.value = match;
	writer[0]->emit(&join,&mr_void);
	if(match == 1 && inputs[0].num_kvs > 1)	// More than 1 home labeled in V2. One match
	{
		match = 0;
	}
	else if(match == 2)
	{
		match = 0;
	}
	join.value = match;
	writer[1]->emit(&join,&mr_void);
}


void MACRO_mobmx_join_v1_v2_results_home_onlyV2::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_Poi poi;
	//Outputs
	MRData_UInt node;
	MRData_UInt join;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{	
		nodbts.parse(inputs[0].kvs[i].key);
		poi.parse(inputs[0].kvs[i].value);
		node.value = nodbts.num1;
		
		if(poi.labelgroupnodebts == 3 && poi.confidentnodebts == 1) // Label level 1 of home in v2
		{
			if(inputs[1].num_kvs != 0)	// Match with v1
			{
				join.value = 1;
				writer[0]->emit(&node,&join);
			}
			else
			{
				join.value = 0;
				writer[0]->emit(&node,&join);
			}
		}
	}
}
void MACRO_mobmx_join_v1_v2_results_work_onlyV2::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_Poi poi;
	//Outputs
	MRData_UInt node;
	MRData_UInt join;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{	
		nodbts.parse(inputs[0].kvs[i].key);
		poi.parse(inputs[0].kvs[i].value);
		node.value = nodbts.num1;
		
		if((poi.labelgroupnodebts == 1 || poi.labelgroupnodebts == 2) 
				&& poi.confidentnodebts == 1) // Label level 1 of work in v2
		{
			if(inputs[1].num_kvs != 0)	// Match with v1
			{
				join.value = 1;
				writer[0]->emit(&node,&join);
			}
			else
			{
				join.value = 0;
				writer[0]->emit(&node,&join);
			}
		}
	}
}
void MACRO_mobmx_get_node_comparative_onlyV2::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt node;
	MRData_UInt join;
	//Outputs
	MRData_Void mr_void;

	int match = 0;

	node.parse(inputs[0].kvs[0].key);

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		join.parse(inputs[0].kvs[i].value);
		if(join.value == 1)
		{
			match = 1;
			break;
		}
	}
	if(match == 1 && inputs[0].num_kvs > 1)	// More than 1 home/work labeled in V2. One match
	{
		match = 2;
	}
	join.value = match;
	writer[0]->emit(&join,&mr_void);
}

void MACRO_mobmx_join_v1_v2_results_home_onlyV1::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_Poi poi;
	//Outputs
	MRData_UInt join;
	MRData_Void mr_void;

	for(int i=0; i<inputs[1].num_kvs; i++)
	{	
		nodbts.parse(inputs[1].kvs[i].key);
		
		if(inputs[0].num_kvs == 0)	// No match with v2
		{
			join.value = 0;
			writer[0]->emit(&join,&mr_void);
		}
		else
		{
			for(int j=0; j<inputs[0].num_kvs; j++)
			{
				poi.parse(inputs[0].kvs[j].value);
				if(poi.labelgroupnodebts == 3 && poi.confidentnodebts == 1) // Match at home with v2
				{
					join.value = 1;
				}
				else //Match but is a different POI or isn't confident
				{
					join.value = 2;
				}
				writer[0]->emit(&join,&mr_void);
			}
		}
	}
}

void MACRO_mobmx_join_v1_v2_results_work_onlyV1::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_Poi poi;
	//Outputs
	MRData_UInt join;
	MRData_Void mr_void;

	for(int i=0; i<inputs[1].num_kvs; i++)
	{	
		nodbts.parse(inputs[1].kvs[i].key);
		
		if(inputs[0].num_kvs == 0)	// No match with v2
		{
			join.value = 0;
			writer[0]->emit(&join,&mr_void);
		}
		else
		{
			for(int j=0; j<inputs[0].num_kvs; j++)
			{
				poi.parse(inputs[0].kvs[j].value);
				if((poi.labelgroupnodebts == 1 || poi.labelgroupnodebts == 2) 
				&& poi.confidentnodebts == 1) // Label level 1 of work in v2
				{
					join.value = 1;
				}
				else //Match but is a different POI or isn't confident
				{
					join.value = 2;
				}
				writer[0]->emit(&join,&mr_void);
			}
		}
	}

}*/