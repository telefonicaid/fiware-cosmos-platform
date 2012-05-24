#include "TEST_Module.h"
#include <set>

#define mob_conf_max_minutes_in_moves 360
#define mob_conf_min_minutes_in_moves 0
#define mob_conf_include_intra_moves false

#define GET_INIT_MX pos=0; pos_field=0; char *offset; int pos_cell=0; int length_cell=0; int cont=0; int length_line=strlen(line);
#define GET_NEXT_FIELD pos_field=pos; while( line[pos] != '|' and line[pos] != '\0' ) pos++; line[pos]='\0';  pos++;
#define GET_LONG( _long ) _long = atoll(&line[pos_field]);
#define GET_INT( _int ) _int = atoi(&line[pos_field]);
#define GET_HEXA(_hexa) _hexa = strtol(&line[pos_field],NULL,16);

////////////////////////////
/////// GROUP MATRIX ///////
////////////////////////////
void MACRO_mobmx_matrix_get_group_catalogue::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input txt
	// Outputs
	MRData_UInt cell;
	MRData_TwoInt bts_group;
	
	//long unsigned int cellId;

  	for (std::vector<char*>::iterator iter = lines.begin() ; iter < lines.end() ; iter++)
	{
		MR_PARSER_INIT(*iter);		
		GET_INIT_MX
		GET_NEXT_FIELD
		GET_HEXA(cell.value)
		//gstCodedStrToInt(&line[pos_field],&cellId, 20);		//CHILE: Conversión de String a decimal
		//cell.value = cellId;
		GET_NEXT_FIELD
		GET_LONG(bts_group.num1)
		GET_NEXT_FIELD
		GET_INT(bts_group.num2)
		writer->emit(&cell,&bts_group);
	}
}
void MACRO_mobmx_matrix_join_cell_group::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_CdrMx cdr;
	MRData_TwoInt bts_group;
	// Outputs
	MRData_UInt node;
	MRData_MatrixTime mtx_time;

	// Cell catalogue
	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		bts_group.parse(inputs[1].kvs[i].value);
		
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			cdr.parse(inputs[0].kvs[j].value);
			node.value = cdr.phone;
			mtx_time.date = cdr.date;
			mtx_time.time = cdr.time;
			mtx_time.bts = bts_group.num1;
			mtx_time.group = bts_group.num2;
			writer[0]->emit(&node,&mtx_time);
		}
	}
}
void MACRO_mobmx_matrix_move_client::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_MatrixTime cur_loc;
	MRData_MatrixTime next_loc;
	// Outputs
	MRData_ItinMovement move;

	//Intermediate
	int difmonth, difday, difhour, difmin, distance;
	int nMinsMonth;
	MRData_ItinTime src, tgt;

	node.parse(inputs[0].kvs[0].key);

	for(int elem=0; elem<inputs[0].num_kvs - 1; elem++)
	{
		cur_loc.parse(inputs[0].kvs[elem].value);
		next_loc.parse(inputs[0].kvs[elem+1].value);
		
		if(cur_loc.group == next_loc.group && cur_loc.bts == next_loc.bts) // Movement at the same bts -> No movement
		{	continue;}
		// Intra movements ??
		if(!mob_conf_include_intra_moves && cur_loc.group == next_loc.group)
		{	continue;}
		//Calculate diference of time between locs
		difmonth = next_loc.date.month - cur_loc.date.month;
		if(difmonth > 1){	continue;}	// Movement over 1 month
		difday = next_loc.date.day - cur_loc.date.day;
		difhour = next_loc.time.hour - cur_loc.time.hour;
		difmin = next_loc.time.minute - cur_loc.time.minute;
		if(cur_loc.date.month==4 || cur_loc.date.month==6 || cur_loc.date.month==9 || 
		cur_loc.date.month==11)
		{
			nMinsMonth = 1440 * 30;
		}
		else if(cur_loc.date.month == 2)
		{
			nMinsMonth = 1440 * 28;
		}
		else
		{
			nMinsMonth = 1440 * 31;
		}
		distance = (nMinsMonth * difmonth) + (1440 * difday) + (60 * difhour) + difmin;
		
		if(distance <= mob_conf_max_minutes_in_moves &&
			distance >= mob_conf_min_minutes_in_moves)  // Filter movements by diff of time
		{
			src.date = cur_loc.date;
			src.time = cur_loc.time;
			src.bts = cur_loc.group;
			move.source.copyFrom(&src);
			tgt.date = next_loc.date;
			tgt.time = next_loc.time;
			tgt.bts = next_loc.group;
			move.target.copyFrom(&tgt);
			writer[0]->emit(&node,&move);
		}
	}
}
void MACRO_mobmx_matrix_get_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_ItinMovement move;
	// Outputs
	MRData_MatrixRange moveRange;
	MRData_Double perc_moves;

	int diff, hourSrc, hourTgt, minutSrc, minutTgt;
	double dur;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		move.parse(inputs[0].kvs[i].value);
		moveRange.poiSrc = move.source.bts;
		moveRange.poiTgt = move.target.bts;
		moveRange.node = moveRange.poiSrc * moveRange.poiTgt; // Common to every pair of bts
		// Calculate portion of moves by hour
		hourSrc = move.source.time.hour;
		hourTgt = move.target.time.hour;
		
		if(move.source.date.week_day != move.target.date.week_day){	hourTgt += 24;}
		diff = (hourTgt - hourSrc);
		if(diff == 0) // Source hour and target hour are the same.
		{
			moveRange.range = move.source.time.hour;
			moveRange.group = move.source.date.week_day;
			perc_moves.value = 1.0;
			writer[0]->emit(&moveRange,&perc_moves);
		}
		else
		{
			minutSrc = move.source.time.minute;
			minutTgt = move.target.time.minute;
			dur = (diff * 60) + (minutTgt - minutSrc);
			// Comunication in source hour
			moveRange.range = move.source.time.hour;
			moveRange.group = move.source.date.week_day;
			perc_moves.value = (60 - minutSrc)/dur;
			writer[0]->emit(&moveRange,&perc_moves);
			// Comunication in target hour
			moveRange.range = move.target.time.hour;
			moveRange.group = move.target.date.week_day;
			perc_moves.value = minutTgt/dur;
			writer[0]->emit(&moveRange,&perc_moves);
			// Fill the intermediate hours
			for(int i=1; i<diff; i++)
			{
				moveRange.range = move.source.time.hour + i;
				moveRange.group = move.source.date.week_day;
				if(moveRange.range > 23)
				{
					moveRange.range -= 24;
					moveRange.group ++;
				}
				perc_moves.value = 60/dur;
				writer[0]->emit(&moveRange,&perc_moves);
			}
		}
	}
}
void MACRO_mobmx_matrix_count_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_MatrixRange moveRange;
	MRData_Double perc_moves;
	//Outputs
	MRData_ItinRange range;
	MRData_ItinPercMove dist_moves;

	double num_moves = 0.0;

	moveRange.parse(inputs[0].kvs[0].key);
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		perc_moves.parse(inputs[0].kvs[i].value);
		num_moves += perc_moves.value;
	}
	
	range.poiSrc = moveRange.poiSrc;
	range.poiTgt = moveRange.poiTgt;
	range.node = moveRange.node;
	range.group = range.range = 0;

	dist_moves.group = moveRange.group;
	dist_moves.range = moveRange.range;
	dist_moves.perc_moves = num_moves;

	writer[0]->emit(&range,&dist_moves);
}
//////////////////////
//// GROUP MATRIX ////
//////////////////////
void MACRO_mobmx_matrix_get_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_ItinRange moveRange;
	MRData_ItinPercMove perc_moves;
	//Outputs
	MRData_TwoInt pairgroup;
	MRData_ClusterVector moves;

	double num_moves=0;
	
	// Initialization of vector
	moves.comsSetLength(168);
	for(int i=0; i<168; i++)
	{
		moves.coms[i].value = 0;
	}

	moveRange.parse(inputs[0].kvs[0].key);
	pairgroup.num1 = moveRange.poiSrc;
	pairgroup.num2 = moveRange.poiTgt;
	// Create vector
	for(int i=0;i<inputs[0].num_kvs; i++)
	{
		perc_moves.parse(inputs[0].kvs[i].value);
		int j = perc_moves.group - 1; // Vector starts on Monday
		j = j>=0 ? j : 6;	// Sunday at the end
		j *= 24;
		j += perc_moves.range;
		moves.coms[j].value = perc_moves.perc_moves;
		num_moves += perc_moves.perc_moves;
	}
	writer[0]->emit(&pairgroup,&moves);
}
void MACRO_mobmx_matrix_join_bts1_group::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_PairIdMtx pairVector;
	MRData_UInt group;
	//Outputs
	MRData_UInt bts;

	for(int i=0; i<inputs[1].num_kvs && i<1; i++)
	{
		group.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			pairVector.parse(inputs[0].kvs[j].value);
			pairVector.id1 = group.value;
			bts.value = pairVector.id2;
			writer[0]->emit(&bts,&pairVector);
		}
	}
}
void MACRO_mobmx_matrix_join_bts2_group::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_PairIdMtx pairVector;
	MRData_UInt group;
	//Outputs
	MRData_TwoInt pairGroup;
	
	for(int i=0; i<inputs[1].num_kvs && i<1; i++)
	{
		group.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			pairVector.parse(inputs[0].kvs[j].value);
			pairVector.id2 = group.value;
			// Filter intra movements
			if(pairVector.id1 == pairVector.id2 && !mob_conf_include_intra_moves){continue;}

			pairGroup.num1 = pairVector.id1;
			pairGroup.num2 = pairVector.id2;
			writer[0]->emit(&pairGroup,&pairVector);
		}
	}
}
void MACRO_mobmx_matrix_sum_group_vectors::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt pairGroup;
	MRData_PairIdMtx pairVector;
	//Outputs
	MRData_ClusterVector totVector;

	pairGroup.parse(inputs[0].kvs[0].key);
	pairVector.parse(inputs[0].kvs[0].value);
	totVector.comsSetLength(0);
	
	// Clean total vector
	for(int i=0; i<pairVector.coms_length; i++)
	{
		MRData_Double elem;
		elem.value = 0;
		totVector.comsAdd()->copyFrom(&elem);
	}
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		pairVector.parse(inputs[0].kvs[i].value);
		for(int j=0; j<pairVector.coms_length; j++)
		{
			totVector.coms[j].value += pairVector.coms[j].value;
		}
	}
	writer[0]->emit(&pairGroup,&totVector);
}
void MACRO_mobmx_matrix_borrar_celltobts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_CellMx cellinfo;
	//Outputs
	MRData_UInt bts;
	MRData_UInt state;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		cellinfo.parse(inputs[0].kvs[i].value);
		bts.value = cellinfo.bts;
		state.value = cellinfo.sta;
		writer[0]->emit(&bts,&state);
	}
}
//////////////////////////////////
////   COMMON FUNCTIONALITY   ////
//////////////////////////////////
void MACRO_mobmx_matrix_spread_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts; //num1: node	num2: bts
	MRData_ItinTime cdrTime;
	//Outputs
	MRData_UInt node;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodbts.parse(inputs[0].kvs[i].key);
		cdrTime.parse(inputs[0].kvs[i].value);
		node.value = nodbts.num1;
		writer[0]->emit(&node,&cdrTime);
	}
}

void MACRO_mobmx_matrix_spread_dist_moves_by_pair::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_ItinRange itrang;
	MRData_Double perc;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		itrang.parse(inputs[0].kvs[i].key);
		itrang.node = itrang.poiSrc * itrang.poiTgt;
		perc.parse(inputs[0].kvs[i].value);
		writer[0]->emit(&itrang,&perc);
	}
}

void MACRO_mobmx_matrix_spread_vector_by_pair::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_ItinRange itrang;
	MRData_ClusterVector moves;
	//Outputs
	MRData_TwoInt pbts;	//num1:bts source	num2: bts target

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		itrang.parse(inputs[0].kvs[i].key);
		moves.parse(inputs[0].kvs[i].value);
		pbts.num1 = itrang.poiSrc;
		pbts.num2 = itrang.poiTgt;
		writer[0]->emit(&pbts,&moves);
	}
}

int MACRO_mobmx_matrix_get_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_TwoInt pair_id;
	MRData_ClusterVector moves;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		pair_id.parse(kvSet.kvs[i].key);
		moves.parse(kvSet.kvs[i].value);
		// BTS1/GROUP1|BTS2/GROUP2|MOVES_MON_0|...|MOVES_SUN_23
		total += fprintf(file,"%lu|%lu", pair_id.num1, pair_id.num2);
		for(int j=0; j<moves.coms_length; j++)
		{
			total += fprintf(file,"|%.4f",moves.coms[j].value);
		}
		total += fprintf(file,"\n");
	}
	return total;
}
int MACRO_mobmx_matrix_get_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}

//////////////////////////////
//////// SOCIAL MATRIX ///////
//////////////////////////////
void MACRO_mobmx_parse_client_profile::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input txt
	// Outputs
	MRData_UInt nodeId;
	MRData_UInt profile;

	char * nse;

  	for (std::vector<char*>::iterator iter = lines.begin() ; iter < lines.end() ; iter++)
	{
		MR_PARSER_INIT(*iter);		
		GET_INIT_MX
		GET_NEXT_FIELD
		GET_LONG(nodeId.value)
		GET_NEXT_FIELD
		GET_INT(profile.value)
		writer->emit(&nodeId,&profile);
	}
}
void MACRO_mobmx_matrix_spread_moves_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_ItinRange itinrng;
	MRData_Double count;
	// Outputs
	MRData_UInt node;
	MRData_ProfileMtx profile;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		itinrng.parse(inputs[0].kvs[i].key);
		count.parse(inputs[0].kvs[i].value);
		node.value = itinrng.node;
		profile.itrange.copyFrom(&itinrng);
		profile.count = count.value;
		writer[0]->emit(&node,&profile);
	}
}
void MACRO_mobmx_matrix_join_moves_profile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_ProfileMtx move;
	MRData_UInt profile;
	// Outputs
	MRData_ItinRange itrange;
	MRData_Double count;
	
	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		profile.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			move.parse(inputs[0].kvs[j].value);
			itrange.copyFrom(&move.itrange);
			itrange.node = profile.value;
			count.value = move.count;
			writer[0]->emit(&itrange,&count);
		}
	}
}

int MACRO_mobmx_matrix_profile_get_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_ItinRange dimensions;
	MRData_ClusterVector moves;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		dimensions.parse(kvSet.kvs[i].key);
		moves.parse(kvSet.kvs[i].value);
		//BTS1/GROUP1|BTS2/GROUP2|PROFILE|MOVES_MON_0|...|MOVES_SUN_23
		total += fprintf(file,"%lu|%lu|%d",dimensions.poiSrc,dimensions.poiTgt,dimensions.node);
		for(int j=0; j<moves.coms_length; j++)
		{
			total += fprintf(file,"|%.4f",moves.coms[j].value);
		}
		total += fprintf(file,"\n");
	}
	return total;
}
int MACRO_mobmx_matrix_profile_get_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}