#include "TEST_Module.h"

#define GET_CLUST_INIT_MX pos=0; pos_field=0; char *offset; int pos_cell=0; int length_cell=0; int cont=0; int length_line=strlen(line);
#define GET_CLUST_NEXT_FIELD pos_field=pos; while( line[pos] != '|' and line[pos] != '\0' ) pos++; line[pos]='\0';  pos++;
#define GET_CLUST_GET_LABEL( label ) label = &line[pos_field];
#define GET_CLUST_GET_COORD( coord ) coord = atof(&line[pos_field]);
#define GET_BTS_ID( id ) id = atoi(&line[pos_field]);

/******* Extraer bts Rocío ******/
void MACRO_mobmx_borrar_get_bts_vector::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_Node_Bts nodbts;
	MRData_ClusterVector btsCoord;

	MRData_Double coord;
	int id;

	for (std::vector<char*>::iterator iter = lines.begin(); iter < lines.end() ; iter++)
	{
		btsCoord.comsSetLength(0);
		MR_PARSER_INIT(*iter);		
		GET_CLUST_INIT_MX
		GET_CLUST_NEXT_FIELD
		GET_BTS_ID( id )
		nodbts.bts = id;
		GET_CLUST_NEXT_FIELD
		GET_CLUST_NEXT_FIELD
		nodbts.phone = nodbts.wday = nodbts.range = 0;
		for(int j=0; j<96; j++)
		{
			GET_CLUST_NEXT_FIELD
			GET_CLUST_GET_COORD(coord.value)
			btsCoord.comsAdd()->copyFrom(&coord);
		}
		writer->emit(&nodbts,&btsCoord);
	}
}
/******** Extraer bts area Rocío **************/
void MACRO_mobmx_borrar_get_bts_comarea::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_UInt btsId;
	MRData_Bts bts;

	MRData_Double coord;
	//int id;

	for (std::vector<char*>::iterator iter = lines.begin(); iter < lines.end() ; iter++)
	{
		MR_PARSER_INIT(*iter);		
		GET_CLUST_INIT_MX
		GET_CLUST_NEXT_FIELD
		GET_BTS_ID(bts.id)	// Extract id
		btsId.value = bts.id;
		GET_CLUST_NEXT_FIELD
		GET_BTS_ID(bts.comms);	// Extract num of comms
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_COORD(bts.area)
		
		writer->emit(&btsId,&bts);
	}
}
/*************** Extraer ejemplos Arturo ********************/
void MACRO_mobmx_borrar_get_examples::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs/Outputs
	MRData_UInt node;
	MRData_CdrMx cdr;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		if(node.value == 78180607 || node.value == 89002906 || node.value == 96240043)
		{
			cdr.parse(inputs[0].kvs[i].value);
			writer[0]->emit(&node,&cdr);
		}
	}
}

int MACRO_mobmx_borrar_get_pospoi_inout::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_UInt node;
	MRData_PoiPos poipos;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		node.parse(kvSet.kvs[i].key);
		poipos.parse(kvSet.kvs[i].value);
		// NODE|BTS|LABEL|POSX|POSY|INOUTWEEK|INOUTWEND|RADIUSWEEK|DISTCMWEEK|RADIUSWEND|DISTCMWEND
		total += fprintf(file,"%lu|%lu|%d|%.6f|%.6f|%d|%d|%.6f|%.6f|%.6f|%.6f",poipos.node, poipos.bts, poipos.label, poipos.posx, poipos.posy, poipos.inoutWeek, poipos.inoutWend, poipos.radiusWeek, poipos.distCMWeek, poipos.radiusWend, poipos.distCMWend);
		total += fprintf(file,"\n");
	}
	return total;
}
int MACRO_mobmx_borrar_get_pospoi_inout::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}

// Itineraries statistics
void MACRO_mobmx_borrar_get_nclient_with_itineraries::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_Itinerary itin;
	// Outputs
	MRData_TelMonth nodpoipoi;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		itin.parse(inputs[0].kvs[i].value);
		nodpoipoi.phone = node.value;
		nodpoipoi.month = itin.source;
		nodpoipoi.workingday = itin.target;
		writer[0]->emit(&nodpoipoi,&itin);
	}
}
void MACRO_mobmx_borrar_delete_itindup::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TelMonth nodpoipoi;
	// Outputs
	MRData_UInt node;
	MRData_TwoInt poipoi;
	MRData_UInt nPeaks;
	MRData_Void mr_void;
	
	nodpoipoi.parse(inputs[0].kvs[0].key);
	node.value = nodpoipoi.phone;
	poipoi.num1 = nodpoipoi.month;
	poipoi.num2 = nodpoipoi.workingday;
	writer[0]->emit(&node,&poipoi);
	nPeaks.value = inputs[0].num_kvs;
	writer[1]->emit(&nPeaks,&mr_void);
}
void MACRO_mobmx_borrar_get_numItin::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Outputs
	MRData_UInt nItin;
	MRData_Void mr_void;
	
	nItin.value = inputs[0].num_kvs;
	writer[0]->emit(&nItin,&mr_void);
}
void MACRO_mobmx_borrar_get_statistics::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt statValue;
	// Outputs
	MRData_UInt numStats;
	
	statValue.parse(inputs[0].kvs[0].key);
	numStats.value = inputs[0].num_kvs;
	writer[0]->emit(&statValue,&numStats);
}