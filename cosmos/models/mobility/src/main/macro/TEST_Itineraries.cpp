#include "TEST_Module.h"
#include <set>

#define mob_conf_max_minutes_in_moves 360
#define mob_conf_min_minutes_in_moves 0
#define mob_conf_min_itin_moves 5.9
#define mob_conf_perc_absolute_max 20.0

void MACRO_mobmx_itin_join_cell_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_CdrMx cdr;
	MRData_CellMx cell_info;
	// Outputs
	MRData_TwoInt node_bts;	// num1: node	num2: bts
	MRData_ItinTime it_time;

	// Cell catalogue
	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		cell_info.parse(inputs[1].kvs[i].value);
		
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			cdr.parse(inputs[0].kvs[j].value);
			node_bts.num1 = cdr.phone;
			node_bts.num2 = cell_info.bts;
			it_time.date = cdr.date;
			it_time.time = cdr.time;
			it_time.bts = cell_info.bts;
			writer[0]->emit(&node_bts,&it_time);
		}
	}
}

void MACRO_mobmx_itin_filter_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt node_bts;
	MRData_ItinTime it_time;
	MRData_Poi poi;
	// Outputs
	MRData_UInt node;

	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		node_bts.parse(inputs[1].kvs[i].key);
		poi.parse(inputs[1].kvs[i].value);
		node.value = node_bts.num1;

		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			it_time.parse(inputs[0].kvs[j].value);
			it_time.bts = poi.id; // Change bts to poi id (inclusion of adjacent)
			writer[0]->emit(&node,&it_time);
		}
	}
}

void MACRO_mobmx_itin_move_client_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_ItinTime cur_loc;
	MRData_ItinTime next_loc;
	// Outputs
	MRData_ItinMovement move;

	//Intermediate
	int difmonth, difday, difhour, difmin, distance;
	int nMinsMonth;

	node.parse(inputs[0].kvs[0].key);

	for(int elem=0; elem<inputs[0].num_kvs - 1; elem++)
	{
		cur_loc.parse(inputs[0].kvs[elem].value);
		next_loc.parse(inputs[0].kvs[elem+1].value);
		
		if(cur_loc.bts != next_loc.bts) // Movement between POIs
		{
			//Calculate diference of time between locs
			difmonth = next_loc.date.month - cur_loc.date.month;
			if(difmonth > 1){	continue;}
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
				move.source.copyFrom(&cur_loc);
				move.target.copyFrom(&next_loc);
				writer[0]->emit(&node,&move);
			}
		}
	}
}

void MACRO_mobmx_itin_get_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_ItinMovement move;
	// Outputs
	MRData_ItinRange moveRange;
	MRData_Double perc_moves;

	int diff, hourSrc, hourTgt, minutSrc, minutTgt;
	double dur;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		move.parse(inputs[0].kvs[i].value);
		moveRange.node = node.value;
		moveRange.poiSrc = move.source.bts;
		moveRange.poiTgt = move.target.bts;
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

void MACRO_mobmx_itin_count_ranges::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_ItinRange moveRange;
	MRData_Double perc_moves;
	//Outputs
	MRData_ItinPercMove dist_moves;

	double num_moves = 0.0;

	moveRange.parse(inputs[0].kvs[0].key);
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		perc_moves.parse(inputs[0].kvs[i].value);
		num_moves += perc_moves.value;
	}
	dist_moves.group = moveRange.group;
	dist_moves.range = moveRange.range;
	dist_moves.perc_moves = num_moves;
	moveRange.group = moveRange.range = 0;

	writer[0]->emit(&moveRange,&dist_moves);
}

void MACRO_mobmx_itin_get_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_ItinRange moveRange;
	MRData_ItinPercMove perc_moves;
	//Outputs
	MRData_ClusterVector dist_moves;

	double num_moves=0;
	
	// Initialization of vector
	dist_moves.comsSetLength(168);
	for(int i=0; i<168; i++)
	{
		dist_moves.coms[i].value = 0;
	}

	moveRange.parse(inputs[0].kvs[0].key);
	// Create vector
	for(int i=0;i<inputs[0].num_kvs; i++)
	{
		perc_moves.parse(inputs[0].kvs[i].value);
		int j = perc_moves.group - 1; // Vector starts on Monday
		j = j>=0 ? j : 6;	// Sunday at the end
		j *= 24;
		j += perc_moves.range;
		dist_moves.coms[j].value = perc_moves.perc_moves;
		num_moves += perc_moves.perc_moves;
	}
	// Filter by min number of moves
	if(num_moves >= mob_conf_min_itin_moves)
	{
		writer[0]->emit(&moveRange,&dist_moves);
	}
}

void MACRO_mobmx_itin_get_itinerary::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_ItinRange moveRange;
	MRData_ClusterVector dist_moves;
	//Outputs
	MRData_UInt node;
	MRData_Itinerary itin;
	
	MRData_ClusterVector peaks_moves;
	double abs_max = 0;
	int init, finish, point;
	MRData_Double elem;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		moveRange.parse(inputs[0].kvs[i].key);
		dist_moves.parse(inputs[0].kvs[i].value);
		peaks_moves.comsSetLength(0);
		// VECTOR NORMALIZATION	
		for(int j=0; j<dist_moves.coms_length; j++)
		{
			if(j>=72  && j<= 95)	// 25 Thursdays in the period	
			{
				elem.value = dist_moves.coms[j].value/25.0;
			}
			else	// 26 Mondays, Tuesdays, Wednesdays, Fridays, Saturdays and Sundays in the period
			{
				elem.value = dist_moves.coms[j].value/26.0;
			}
			peaks_moves.comsAdd()->copyFrom(&elem);
			// Get absolute maximum
			if(elem.value > abs_max)
			{
				abs_max = elem.value;
			}
		}
		// FILTER LOW MOVEMENTS
		for(int j=0; j<peaks_moves.coms_length; j++)
		{
			if(peaks_moves.coms[j].value < (abs_max * mob_conf_perc_absolute_max/100.0))
			{
				peaks_moves.coms[j].value = 0;
			}
		}
		// EXTRACT ITINERARIES
		node.value = moveRange.node;
		itin.source = moveRange.poiSrc;
		itin.target = moveRange.poiTgt;
		for(int j=0; j<peaks_moves.coms_length; j++)  // Vector
		{
			if(peaks_moves.coms[j].value != 0) // Zone starts
			{
				double peak = 0;
				itin.wday_init = itin.range_init = j;
				for(j; j<peaks_moves.coms_length && peaks_moves.coms[j].value != 0; j++) 
				{
					if(peaks_moves.coms[j].value > peak) // Peak starts
					{
						peak = peaks_moves.coms[j].value;
						itin.wday_peak_init = itin.range_peak_init = j;
						itin.wday_peak_fin = itin.range_peak_fin = j;
						for(j; j<peaks_moves.coms_length - 1 && peaks_moves.coms[j].value == peaks_moves.coms[j+1].value; j++)
						{
							itin.wday_peak_fin = itin.range_peak_fin = j+1;
						}
					}
					itin.wday_fin = itin.range_fin = j;
				}

				itin.wday_init = (itin.wday_init/24 + 1) % 7;
				itin.wday_peak_init = (itin.wday_peak_init/24 + 1) % 7;
				itin.wday_peak_fin = (itin.wday_peak_fin/24 + 1) %7;
				itin.wday_fin = (itin.wday_fin/24 + 1) % 7;
				itin.range_init = itin.range_init % 24;
				itin.range_peak_init = itin.range_peak_init % 24;
				itin.range_peak_fin = itin.range_peak_fin % 24;
				itin.range_fin = itin.range_fin % 24;
				writer[0]->emit(&node,&itin);
			}
		}
	}
}

// Export itineraries
int MACRO_mobmx_itin_itinerary_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_UInt node;
	MRData_Itinerary itin;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		node.parse(kvSet.kvs[i].key);
		itin.parse(kvSet.kvs[i].value);
		// NODE|POI_SOURCE|POI_TARGET|WDAYPEAK_INIT|HOURPEAK_INIT|WDAYPEAK_FINISH|HOURPEAK_FINISH|WDAY_INIT|HOUR_INIT|WDAY_FINISH|HOUR_FINISH
		total += fprintf(file,"%lu|%lu|%lu|", node.value, itin.source, itin.target);
		total += fprintf(file,"%d|%d|%d|%d|", itin.wday_peak_init, itin.range_peak_init, itin.wday_peak_fin, itin.range_peak_fin);
		total += fprintf(file,"%d|%d|%d|%d\n",itin.wday_init, itin.range_init, itin.wday_fin, itin.range_fin);
	}
	return total;
}
int MACRO_mobmx_itin_itinerary_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}
