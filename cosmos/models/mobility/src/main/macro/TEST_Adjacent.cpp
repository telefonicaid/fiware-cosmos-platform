#include "TEST_Module.h"

#define GET_ADJ_INIT_MX pos=0; pos_field=0; char *offset; int pos_cell=0; int length_cell=0; int cont=0; int length_line=strlen(line);
#define GET_ADJ_NEXT_FIELD pos_field=pos; while( line[pos] != '|' and line[pos] != '\0' ) pos++; line[pos]='\0';  pos++;
#define GET_ADJ_GET_INT( _int ) _int = atoi(&line[pos_field]);

void MACRO_mobmx_adj_parse_adj_bts::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_TwoInt pairbts;
	MRData_Void mrvoid;

	for (std::vector<char*>::iterator iter = lines.begin(); iter < lines.end() ; iter++)
	{
		MR_PARSER_INIT(*iter);		
		GET_ADJ_INIT_MX
		GET_ADJ_NEXT_FIELD
		GET_ADJ_GET_INT(pairbts.num1)
		GET_ADJ_NEXT_FIELD
		GET_ADJ_GET_INT(pairbts.num2)
		writer->emit(&pairbts,&mrvoid);
	}
}

void MACRO_mobmx_adj_add_unique_id_poi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt nod_bts;	// num1: Node	num2: Bts
	MRData_Poi poi;
	// Outputs
	MRData_TwoInt nod_lbl;	// num1: Node	num2: Poi group label
	MRData_PoiNew poiId;
	MRData_UInt tableId;
	MRData_TwoInt poi_poimod;

	if(inputs[0].num_kvs == 0)
	{
		return;
	}
	nod_bts.parse(inputs[0].kvs[0].key);
	int hash = nod_bts.getPartition(100);
	size_t counter = 0;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nod_bts.parse(inputs[0].kvs[i].key);
		poi.parse(inputs[0].kvs[i].value);
		nod_lbl.num1 = poi.node;
		tableId.value = poiId.id = (100 * counter ++) + hash;
		poi_poimod.num1 = poi_poimod.num2 = tableId.value;
		poiId.node = poi.node;
		poiId.bts = poi.bts;
		if(poi.confidentnodebts == 1)
		{
			poiId.labelgroupnodebts = poi.labelgroupnodebts;
			poiId.confidentnodebts = poi.confidentnodebts;
		}
		else
		{
			poiId.labelgroupnodebts = 0;
			poiId.confidentnodebts = poi.confidentnodebts;
		}
		nod_lbl.num2 = poiId.labelgroupnodebts;
		writer[0]->emit(&nod_lbl,&poiId);
		writer[1]->emit(&tableId,&poi_poimod);
	}
}

void MACRO_mobmx_adj_group_type_poi_client::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_PoiNew curPoi;
	// Outputs
	MRData_TwoInt pairBts;	// num1: bts1	num2: bts2
	MRData_TwoInt pairPois;	// num1: poi1	num2: poi2

	MRData_PoiNew tempPoi;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		curPoi.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			tempPoi.parse(inputs[0].kvs[j].value);
			if(curPoi.id < tempPoi.id)
			{
				pairBts.num1 = curPoi.bts;
				pairBts.num2 = tempPoi.bts;
				pairPois.num1 = curPoi.id;
				pairPois.num2 = tempPoi.id;
				writer[0]->emit(&pairBts,&pairPois);
			}
		}
	}
}

void MACRO_mobmx_adj_join_pairbts_adjbts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{	
	// Inputs
	MRData_TwoInt pairPois;	// Num1: Poi1	Num2: Poi2
	// Outputs
	MRData_UInt poi;

	if(inputs[1].num_kvs > 0)	// Pair of adjacent bts
	{
		for(int i=0; i<inputs[0].num_kvs; i++)
		{
			pairPois.parse(inputs[0].kvs[i].value);
			poi.value = pairPois.num1;
			writer[0]->emit(&poi,&pairPois);
		}
	}
}

void MACRO_mobmx_adj_put_max_id::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt poi;
	MRData_TwoInt pairPois;	// num1: poiId1		num2: poiId2

	unsigned int max=0;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poi.parse(inputs[0].kvs[i].key);
		pairPois.parse(inputs[0].kvs[i].value);
		if(pairPois.num2 > max)
		{
			max = pairPois.num2;
		}
	}
	pairPois.num2 = max;
	writer[0]->emit(&poi,&pairPois);
}

void MACRO_mobmx_adj_update_pois_table::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt postId;
	MRData_TwoInt poi_poimod;
	MRData_TwoInt pairPois;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		postId.parse(inputs[0].kvs[i].key);
		poi_poimod.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[1].num_kvs; j++)	// POI id needs to be updated
		{
			pairPois.parse(inputs[1].kvs[j].value);
			postId.value = poi_poimod.num2 = pairPois.num2;
		}
		writer[0]->emit(&postId,&poi_poimod);
	}
}
void MACRO_mobmx_adj_swap_poiId_st1::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt pairPois;
	MRData_TwoInt pairIndex;
	// Outputs
	MRData_UInt poi;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		pairPois.parse(inputs[0].kvs[i].value);
		
		// Change the poi1 to index poi
		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			pairIndex.parse(inputs[1].kvs[j].value);
			pairPois.num1 = pairIndex.num2;
        }
		poi.value = pairPois.num2;
		writer[0]->emit(&poi,&pairPois);
	}
}
void MACRO_mobmx_adj_swap_poiId_st2::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt pairPois;
	MRData_TwoInt pairIndex;
	// Outputs
	MRData_UInt poi;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		pairPois.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			pairIndex.parse(inputs[1].kvs[j].value);
			pairPois.num2 = pairIndex.num2;
        }
		if(pairPois.num1 == pairPois.num2){	continue;}
		if(pairPois.num1 > pairPois.num2)
		{
			int tmp = pairPois.num1;
			pairPois.num1 = pairPois.num2;
			pairPois.num2 = tmp;
		}
		poi.value = pairPois.num1;
		writer[0]->emit(&poi,&pairPois);
	}
}

void MACRO_mobmx_adj_spread_count::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Outputs
	MRData_UInt key;
	MRData_UInt nkvs;

	key.value = 0;
	nkvs.value = inputs[0].num_kvs;
	writer[0]->emit(&key,&nkvs);
}
void MACRO_mobmx_adj_count_indexes::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt nkvs;
	// Outputs
	MRData_UInt nIndex;
	MRData_Void mr_void;

	int sum = 0;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nkvs.parse(inputs[0].kvs[i].value);
		sum += nkvs.value;
	}
	nIndex.value = sum;
	writer[0]->emit(&nIndex,&mr_void);
}
void MACRO_mobmx_adj_spread_table_by_poiId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt poi_poimod;
	// Outputs
	MRData_UInt poiId;
	MRData_UInt poiMod;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poi_poimod.parse(inputs[0].kvs[i].value);
		poiId.value = poi_poimod.num1;
		poiMod.value = poi_poimod.num2;
		writer[0]->emit(&poiId,&poiMod);
	}
}
void MACRO_mobmx_adj_spread_pois_by_poiId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_PoiNew poi;
	// Outputs
	MRData_UInt poiId;
    
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poi.parse(inputs[0].kvs[i].value);
		poiId.value = poi.id;
		writer[0]->emit(&poiId,&poi);
	}
}
void MACRO_mobmx_adj_join_new_poiId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt poiMod;
	MRData_PoiNew poi;
	// Outputs
	MRData_TwoInt nod_bts;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poiMod.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			poi.parse(inputs[1].kvs[j].value);
			nod_bts.num1 = poi.node;
			nod_bts.num2 = poi.bts;
			poi.id = poiMod.value;
			writer[0]->emit(&nod_bts,&poi);
		}
	}
}
void MACRO_mobmx_adj_change_poisId::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt nod_bts;
	MRData_Poi poi;
	MRData_PoiNew newPoi;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nod_bts.parse(inputs[0].kvs[i].key);
		poi.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			newPoi.parse(inputs[1].kvs[j].value);
			poi.id = newPoi.id;
			writer[0]->emit(&nod_bts,&poi);
		}
	}
}