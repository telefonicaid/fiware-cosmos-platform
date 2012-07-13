#include "TEST_Module.h"
#include "VoronoiDiagramGenerator.h"
#include "time.h"
#include <set>

////////////////////////////////////////////////////////////////////
//////////////////// MOBILITY PRODUCT //////////////////////////////
////////////////////////////////////////////////////////////////////
#define mob_conf_min_number_total_calls 200
#define mob_conf_max_number_total_calls 5000
#define mob_conf_min_number_calls_bts 12
#define mob_conf_min_perc_rep_bts 5

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// PARSERS ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#define GET_CDRS_INIT_MX( num, max_cell) pos=0; pos_field=0; num=0; char *offset; int pos_cell=0; int length_cell=0; int cont=0; int length_line=strlen(line); char aux[max_cell + 2]; aux[0]='0'; aux[1]='x'; 
#define GET_CDRS_NEXT_FIELD_MX  pos_field=pos; while( line[pos] != '|' and line[pos] != '\0' ) pos++; line[pos]='\0';  pos++;
#define GET_CDRS_GET_NUMBER_MX( num ) num = atoll( &line[pos_field] );
#define GET_CDRS_GET_CLIENT_MX( num, max_client ) num=0; length_cell = pos-pos_field-1; pos_cell=pos_field; if(length_cell > max_client) pos_cell=pos - max_client -1; for(cont=0;cont<length_cell;cont++) if(isdigit(line[cont]) == 0 ) exit; num = atoll( &line[pos_cell] ); cont=0;
#define GET_CDRS_GET_CELL_MX(num, max_cell) if(num == 0) { pos_cell = 0; length_cell = pos-pos_field-1; if(length_cell > max_cell){      pos_cell=length_cell-max_cell;}strcpy(aux, line + pos_field + pos_cell); num = strtol(aux,NULL,16); }
#define GET_CDRS_GET_DURATION_MX( num ) num = atoll( &line[pos_field] );
#define GET_CDRS_GET_FOREIGN_MX( num )  num=0; if( (strcmp(&line[pos_field], "RMITERR") == 0) or (strcmp(&line[pos_field], "RMITRAR") == 0)  or (strcmp(&line[pos_field], "RMNACIO") == 0) ) num=1; if ( (strcmp(&line[pos_field], "RMITNCL") == 0) or (strcmp(&line[pos_field], "RMMUNDI") == 0) ) num=2;
#define GET_CDRS_GET_POSITION_MX( num ) num = atof(&line[pos_field]);

#define DISTINT 15000.0

////////
// Parse cdrs
////////
void MACRO_mobmx_parse_cdrs::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input txt
	// Outputs
	MRData_UInt nodeId;
	MRData_CdrMx cdr;

  //LINE --> "33F430521676F4|2221436242|33F430521676F4|0442224173253|2|01/01/2010|02:00:01|2891|RMITERR"
  //Note: multihtread save implementation
	int length_max_cell = 8;
	int length_max_client = 10;

	for (std::vector<char*>::iterator iter = lines.begin() ; iter < lines.end() ; iter++)
	{
		bool cell2 = false;
		int duration = 0;

		MR_PARSER_INIT(*iter);
		GET_CDRS_INIT_MX( cdr.cell, length_max_cell) 

		GET_CDRS_NEXT_FIELD_MX      
		//Begin Cell field. we are left with the 8 least significant hexadecimal
		GET_CDRS_GET_CELL_MX(cdr.cell, length_max_cell)
		if(cdr.cell == 0) {	cell2 = true;}

		GET_CDRS_NEXT_FIELD_MX                     
		//Phone1
		GET_CDRS_GET_CLIENT_MX(cdr.phone, length_max_client)

		GET_CDRS_NEXT_FIELD_MX      
		//End Cell field. If begin Cell is null then we are left with the 8 least significant hexadecimal
		GET_CDRS_GET_CELL_MX(cdr.cell, length_max_cell)

		GET_CDRS_NEXT_FIELD_MX                     
		//Phone2
		GET_CDRS_NEXT_FIELD_MX      
		//Adrress field

		GET_CDRS_NEXT_FIELD_MX                     
		//Date field
		cdr.date.day    = CHAR_TO_INT( line[pos_field+0] ) *10 + CHAR_TO_INT( line[pos_field+1] );
		cdr.date.month  = CHAR_TO_INT( line[pos_field+3] ) *10 + CHAR_TO_INT( line[pos_field+4] );
		cdr.date.year   = CHAR_TO_INT( line[pos_field+8] ) *10 + CHAR_TO_INT( line[pos_field+9] );
		GET_CDRS_NEXT_FIELD_MX      
		//Time field
		cdr.time.hour     = CHAR_TO_INT( line[pos_field+0] ) *10 + CHAR_TO_INT( line[pos_field+1] );
		cdr.time.minute   = CHAR_TO_INT( line[pos_field+3] ) *10 + CHAR_TO_INT( line[pos_field+4] );
		cdr.time.seconds  = CHAR_TO_INT( line[pos_field+6] ) *10 + CHAR_TO_INT( line[pos_field+7] );

		GET_CDRS_NEXT_FIELD_MX                     
		//Duration field
/*		if(cell2)
		{
			GET_CDRS_GET_DURATION_MX(duration)
		}*/

		//GET_CDRS_NEXT_FIELD_MX
		//GET_CDRS_GET_FOREIGN_MX(cdr.foreign)
		
		// CALCULATE DAY OF WEEK (Sunday: 0, Monday: 1, ... , Saturday: 6)
		// assume year > 1900
		// tested for 1901 to 2099 (seems to work from 1800 on too)
		int _year, _month, _day;
		int ix, tx, vx;	
		_year = cdr.date.year + 100; // i.e. (year+2000-1900)
		_month = cdr.date.month;
		_day = cdr.date.day;

		if( (_month == 2) || (_month == 6) ) vx = 0;
		else if( _month == 8 ) vx = 4;
		else if( _month == 10 ) vx = 8;
		if( (_month == 9) || (_month == 12) ) vx = 12;
		if( (_month == 3) || (_month == 11) ) vx = 16;
		if( (_month == 1) || (_month == 5) ) vx = 20;
		if( (_month == 4) || (_month == 7) ) vx = 24;

		ix = ((_year - 21) % 28) + vx + (_month > 2);
		tx = (ix + (ix / 4)) % 7 + _day;
		cdr.date.week_day = (tx + 1)%7;

		nodeId.value = cdr.phone;
		writer->emit(&nodeId,&cdr);
	}
}

////////
// Parse cells
////////
void MACRO_mobmx_parse_cells::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_UInt cellId;
	MRData_CellMx cellInfo;

	int length_max_cell = 8;
	int length_max_client = 10;

	for (std::vector<char*>::iterator iter = lines.begin() ; iter < lines.end() ; iter++)
	{
		MR_PARSER_INIT(*iter);
		GET_CDRS_INIT_MX(cellInfo.cell,length_max_cell)
		GET_CDRS_NEXT_FIELD_MX
		GET_CDRS_GET_CELL_MX(cellInfo.cell,length_max_cell)
		GET_CDRS_NEXT_FIELD_MX
		GET_CDRS_GET_NUMBER_MX(cellInfo.bts)
		GET_CDRS_NEXT_FIELD_MX
		GET_CDRS_GET_NUMBER_MX(cellInfo.mun)
		GET_CDRS_NEXT_FIELD_MX
		GET_CDRS_GET_NUMBER_MX(cellInfo.sta)
		GET_CDRS_NEXT_FIELD_MX
		GET_CDRS_GET_POSITION_MX(cellInfo.posx)
		GET_CDRS_NEXT_FIELD_MX
		GET_CDRS_GET_POSITION_MX(cellInfo.posy)
		// TODO: Conversión de coordenadas GMT a UTM
		cellId.value = cellInfo.cell;
		writer->emit(&cellId,&cellInfo);
	}
}

////////
// Parse contorno
////////
void MACRO_mobmx_parse_contorno::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_UInt group;
	MRData_Contorno contorno;

	int length_max_cell = 8;
	int length_max_client = 10;

	for (std::vector<char*>::iterator iter = lines.begin() ; iter < lines.end() ; iter++)
	{
		MR_PARSER_INIT(*iter);
		GET_CDRS_GET_POSITION_MX(contorno.posx)
		GET_CDRS_NEXT_FIELD_MX
		GET_CDRS_GET_POSITION_MX(contorno.posy)
		// TODO: Conversión de coordenadas GMT a UTM
		
    group.value = 1; //Usamos un grupo porque queremos que todas las coordenadas se traten en un solo Reduce
		
    writer->emit(&group,&contorno);
	}
}

////////
// Get mobility info from Gasset parser
////////
void MACRO_mobmx_get_mob_from_cdrs::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_GSTCdr gst_cdr;
	// Outputs
	MRData_CdrMx mob_cdr;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		gst_cdr.parse(inputs[0].kvs[i].value);
		mob_cdr.phone = node.value;
		mob_cdr.date = gst_cdr.date;
		mob_cdr.date.week_day = (mob_cdr.date.week_day + 1)%7;
		mob_cdr.time = gst_cdr.time;
		mob_cdr.cell = gst_cdr.cellId;
		writer[0]->emit(&node,&mob_cdr);
	}
}
void MACRO_mobmx_get_mob_from_cells::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt cell;
	MRData_GSTCell gst_cell;
	//Outputs
	MRData_CellMx mob_cell;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		cell.parse(inputs[0].kvs[i].key);
		gst_cell.parse(inputs[0].kvs[i].value);
		mob_cell.cell = gst_cell.cellId;
		mob_cell.bts = gst_cell.btsId;
		mob_cell.mun = gst_cell.lacId;
		mob_cell.sta = gst_cell.stateId;
		mob_cell.posx = gst_cell.posx;
		mob_cell.posy = gst_cell.posy;
		writer[0]->emit(&cell,&mob_cell);
	}
}
////////
// Filter cdrs with no cell info
////////
void MACRO_mobmx_filter_cellnoinfo::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt nodeId;
	MRData_CdrMx cdr;
	//Outputs
	MRData_UInt cellId;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodeId.parse(inputs[0].kvs[i].key);
		cdr.parse(inputs[0].kvs[i].value);
		cellId.value = cdr.cell;
		if(cdr.cell != 0)
		{
			writer[0]->emit(&cellId,&cdr);
		}
		else
		{
			writer[1]->emit(&nodeId,&cdr);
		}
	}
}
////////
// Get sample
////////
void MACRO_mobmx_get_sample_10000::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs 
	MRData_UInt nodeId;
	MRData_CdrMx cdr;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodeId.parse(inputs[0].kvs[i].key);
		cdr.parse(inputs[0].kvs[i].value);
	//	int nod = (nodeId.value % 100000)/100; 
	//	if(nod == 925)
		int nod = (nodeId.value % 100000)/1000; 
		if(nod == 92)
		{
			nodeId.value = cdr.phone;
			writer[0]->emit(&nodeId,&cdr);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////// GET REPRESENTATIVE BTS //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////////
// Substitute cell by bts
////////
void MACRO_mobmx_join_bts_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt cellId;
	MRData_CdrMx cdr;
	MRData_CellMx cell;
	// Outputs
	MRData_Node_Bts node_bts;
	MRData_Void mr_void;
	MRData_UInt bts;
	MRData_TwoInt dayrange;	//num1: bts		num2: range
	MRData_TelMonth telmonth;

	if(inputs[1].num_kvs == 0) // Cell is not in the catalogue
	{
		for(int i=0; i<inputs[0].num_kvs; i++)
		{
			cdr.parse(inputs[0].kvs[i].value);
			cellId.value = cdr.phone;
			writer[2]->emit(&cellId,&cdr);	
		}
	}
	else
	{
		for(int i=0; i<inputs[1].num_kvs; i++)
		{
			cellId.parse(inputs[1].kvs[i].key);
			cell.parse(inputs[1].kvs[i].value);
			for(int j=0; j<inputs[0].num_kvs; j++)
			{
				cdr.parse(inputs[0].kvs[j].value);
				///////////// POINTS OF INTEREST /////////////
				node_bts.phone = cdr.phone;
				bts.value = node_bts.bts = cell.bts;
				node_bts.wday = cdr.date.week_day;
				if(node_bts.wday == 0)		// Group 3: Sunday
				{
					dayrange.num1 = 3;
				}
				else if(node_bts.wday == 5)	// Group 1: Friday
				{
					dayrange.num1 = 1;
				}
				else if(node_bts.wday == 6)	// Group 2: Saturday
				{
					dayrange.num1 = 2;
				}
				else						// Group 0: Monday-Thursday
				{
					dayrange.num1 = 0;
				}
				dayrange.num2 = node_bts.range = cdr.time.hour;
				writer[0]->emit(&node_bts,&mr_void);
				writer[1]->emit(&bts,&dayrange);

				////////// INDIVIDUAL VARIABLES /////////
				telmonth.phone = cdr.phone;
				telmonth.month = cdr.date.month;
				int hour = cdr.time.hour;
				if( (node_bts.wday == 0 || node_bts.wday == 6)||(node_bts.wday == 5 && hour >= 18)) // Weekend
				{
					telmonth.workingday = 0;
				}
				else // Working day
				{
					telmonth.workingday = 1;
				}
				writer[3]->emit(&telmonth,&cell);
			}
		}
	}
}
////////
// Count number of comms by node, bts, hour and day
////////
void MACRO_mobmx_node_bts_counter::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Input
	MRData_Node_Bts nodeBts;
	//Outputs
	MRData_UInt nodeId;
	MRData_Bts_Counter count;

	nodeBts.parse(inputs[0].kvs[0].key);
	nodeId.value = nodeBts.phone;
	count.bts = nodeBts.bts;
	count.wday = nodeBts.wday;
	count.range = nodeBts.range;
	count.count = inputs[0].num_kvs;
	writer[0]->emit(&nodeId,&count);
}
////////
// Array of counters by node: num of comms by day and hour
////////
void MACRO_mobmx_node_mobInfo::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt nodeId;
	MRData_Bts_Counter btsCount;
	//Outputs
	MRData_NodeMx_Counter nodeCount;

	nodeId.parse(inputs[0].kvs[0].key);
	nodeCount.btsSetLength(inputs[0].num_kvs);
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		btsCount.parse(inputs[0].kvs[i].value);
		nodeCount.bts[i] = btsCount;
	}
	writer[0]->emit(&nodeId,&nodeCount);
}
////////
// Spread array of counters by node and bts
////////
void MACRO_mobmx_repbts_spread_nodebts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt nodeId;
	MRData_NodeMx_Counter counter;
	// Outputs
	MRData_Node_Bts_Day outputkey;
	MRData_UInt ncalls;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodeId.parse(inputs[0].kvs[i].key);
		counter.parse(inputs[0].kvs[i].value);
		for(int j=0; j<counter.bts_length; j++)
		{
			outputkey.node = nodeId.value;
			outputkey.bts = counter.bts[j].bts;
			outputkey.workday = 0;
			outputkey.count = 0; // It is not use in this step

			ncalls.value = counter.bts[j].count;
			writer[0]->emit(&outputkey,&ncalls);
		}
	}
}
////////
// Sum of num comms by bts of a client
////////
void MACRO_mobmx_repbts_aggbybts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Node_Bts_Day inkey;
	MRData_UInt ncalls;
	// Outputs
	MRData_UInt node;
	MRData_Node_Bts_Day output;

	int num_calls = 0;

    inkey.parse(inputs[0].kvs[0].key);
	node.value = inkey.node;
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		ncalls.parse(inputs[0].kvs[i].value);
		num_calls += ncalls.value;
		
	}
	output.node = inkey.node;
	output.bts = inkey.bts;
	output.workday = inkey.workday;
	output.count = num_calls;
	writer[0]->emit(&node,&output);
}
////////
// Filter clients by total number of comms
////////
void MACRO_mobmx_repbts_filter_num_comms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_Node_Bts_Day input;
	// Outputs
	MRData_UInt ncomms;

	int num_comms_info, num_comms_noinfo, num_comms_nobts;
	num_comms_info = num_comms_noinfo = 0;

	if(inputs[0].num_kvs == 0) // There is not any cdr with cell info --> No output
	{	return;}
	node.parse(inputs[0].kvs[0].key);

	// Num of communications with bts info
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		input.parse(inputs[0].kvs[i].value);
		num_comms_info += input.count;
	}

	// Num of communications without bts info
	num_comms_noinfo = inputs[1].num_kvs;
	num_comms_nobts = inputs[2].num_kvs;
	
	// Filter by total num of communications
	if((num_comms_info + num_comms_noinfo + num_comms_nobts) < mob_conf_min_number_total_calls)
	{
		ncomms.value = num_comms_info + num_comms_noinfo + num_comms_nobts;
	}
	else if((num_comms_info + num_comms_noinfo + num_comms_nobts) > mob_conf_max_number_total_calls)
	{
		ncomms.value = num_comms_info + num_comms_noinfo + num_comms_nobts;
	}
	else
	{
		ncomms.value = num_comms_info;
		writer[0]->emit(&node,&ncomms);
	}
}
////////
// Calculate percentage by bts
////////
void MACRO_mobmx_repbts_join_dist_comms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt node;
	MRData_Node_Bts_Day nodbtsday;
	MRData_UInt ncomms;
	//Outputs
	MRData_Bts_Counter output;

	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		node.parse(inputs[1].kvs[i].key);
		ncomms.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			nodbtsday.parse(inputs[0].kvs[j].value);
			output.bts = nodbtsday.bts;
			output.wday = 0;
			output.range = nodbtsday.count;  // ATENTION: Field range is used for storing number of comms
			output.count = (nodbtsday.count*100)/ncomms.value;
			writer[0]->emit(&node,&output);
		}
	}
}
////////
// Filter bts and extract representative ones
////////
void MACRO_mobmx_repbts_get_representative_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt node;
	MRData_Bts_Counter counter;
	//Outputs
	MRData_TwoInt nodebts;
	MRData_Bts_Counter output;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		counter.parse(inputs[0].kvs[i].value);
		// counter.count --> percentage of comm of rep bts
		// counter.range --> number of comm of rep bts
		if( counter.count >= mob_conf_min_perc_rep_bts	&&
			counter.range >= mob_conf_min_number_calls_bts)
		{
			nodebts.num1 = node.value;
			nodebts.num2 = counter.bts;
			output.bts = counter.bts;
			output.count = counter.count;
			output.wday = output.range = 0;
			writer[0]->emit(&nodebts,&output);
		}
	}
}

void MACRO_mobmx_cells_spread_GMT_to_UTM::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
  /*//Inputs
  MRData_UInt cellId;
  MRData_CellMx cellInfo;
  //Outputs
  MRData_UInt groupId;
  
  for(int i=0; i<inputs[0].num_kvs; i++)
  {
    cellInfo.parse(inputs[0].kvs[i].value);
    
    groupId.value = 1;
    
    writer[0]->emit(&groupId, &cellInfo);
  }*/
}

void MACRO_mobmx_get_husos_cells_GMT_to_UTM::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
  /*//Inputs
  MRData_UInt  group;
  MRData_CellMx cellInfo;
  //Outputs
  MRData_UInt groupId;
  MRData_Huso husoInfo;

  double med_lon_gra_dec = 0;
  
  for(int i=0; i<inputs[0].num_kvs; i++)
  {
    cellInfo.parse(inputs[0].kvs[i].value);
    
    med_lon_gra_dec += cellInfo.posx;
  }
  
  med_lon_gra_dec /= inputs[0].num_kvs;
  
  husoInfo.id = (int) (med_lon_gra_dec / 6 + 31);
  husoInfo.merid_gra = (double) husoInfo.id * 6 - 183;
  husoInfo.merid_rad = husoInfo.merid_gra * M_PI / 180;
  
  groupId.value = 1;
  
  writer[0]->emit(&groupId, &husoInfo);*/
}

void MACRO_mobmx_calc_cells_GMT_to_UTM::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
 /* //Inputs
  MRData_UInt groupId;
  MRData_CellMx cellInfo;
  MRData_Huso husoInfo;
  //Outputs
  MRData_UInt cellId;
  MRData_CellMxUTM outCellInfo;
  
  //Variables
  double lon_rad; 
  double lat_rad;
  double delta_lambda;
  double cos_cua_lat_rad;
  double A;
  double Xi;
  double Eta;
  double Ni;
  double Zeta;
  double A1; 
  double A2;
  double J2;
  double J4;
  double J6;
  double Alfa;
  double Beta;
  double Gamma;
  double B_fi;
  
  MRData_Elipsoide eli;
 
  
  //Rellenamos los datos del elipsoide
  eli.a = 6378137.0; //Valor fijo para elipsoides del tipo WGS 84
  eli.b = 6356752.31424518;  //Valor fijo para elipsoides del tipo WGS 84
  eli.e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.a;
  eli.seg_e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.b;
  eli.seg_e_cua = eli.seg_e * eli.seg_e;
  eli.c = (eli.a * eli.a) / eli.b;
  eli.apl = (eli.a - eli.b) / eli.a;
  
  
  // Recibe las celdas de un grupo determinado
	for(int i=0; i<inputs[0].num_kvs; i++)
  {
    cellInfo.parse(inputs[0].kvs[i].value);
    
    //Calcula las coordenadas en radianes
    lon_rad = cellInfo.posx * M_PI / 180;
    lat_rad = cellInfo.posy * M_PI / 180;
    
    // Bucle innecesario, en este caso solo recibiremos grupo 1
    for(int j=0; j<inputs[1].num_kvs; j++)
	  {
      husoInfo.parse(inputs[1].kvs[j].value);
      
      delta_lambda = lon_rad - husoInfo.merid_rad;
      cos_cua_lat_rad = pow(cos(lat_rad), 2);
      A = cos(lat_rad) * sin(delta_lambda);
      Xi = 0.5 * log((1 + A) / (1 - A));
      Eta = atan(tan(lat_rad) / cos(delta_lambda)) - lat_rad;
      Ni = eli.c / sqrt(1 + eli.seg_e_cua * cos_cua_lat_rad) * 0.9996;
      Zeta = 0.5 * eli.seg_e_cua * Xi * Xi * cos_cua_lat_rad;
      A1 = sin(2 * lat_rad);
      A2 = A1 * cos_cua_lat_rad;
      J2 = lat_rad + 0.5 * A1;
      J4 = (3 * J2 + A2) / 4;
      J6 = (5 * J4 + A2 * cos_cua_lat_rad) / 3;
      Alfa = 0.75 * eli.seg_e_cua;
      Beta = 5 * Alfa * Alfa / 3;
      Gamma = 35 * Alfa * Alfa * Alfa / 27;
      B_fi = 0.9996 * eli.c * (lat_rad - Alfa * J2 + Beta * J4 - Gamma * J6);
      
      //Rellenamos los valores de la salida
      cellId.value = cellInfo.cell;
      outCellInfo.cell = cellInfo.cell;
	    outCellInfo.bts = cellInfo.bts;
	    outCellInfo.mun = cellInfo.mun;
	    outCellInfo.sta = cellInfo.sta;
	    outCellInfo.posx = Xi * Ni * (1 + Zeta / 3) + 500000;
      outCellInfo.posy = Eta * Ni * (1 + Zeta) + B_fi;

      
      if (cellInfo.posy < 0) 
      {
        outCellInfo.huso.id = -(husoInfo.id);
        (outCellInfo.posy) += 10000000;
      }
      else 
      {
        outCellInfo.huso.id = husoInfo.id;
      }
      
      outCellInfo.huso.merid_gra = husoInfo.merid_gra;
      outCellInfo.huso.merid_rad = husoInfo.merid_rad;
      
      writer[0]->emit(&cellId,&outCellInfo);
    }
  }*/
}

void MACRO_mobmx_calc_cells_UTM_to_GMT::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
  /*//Inputs
  MRData_UInt cellId;
  MRData_CellMxUTM cellInfo;
  //Outputs
  MRData_CellMx outCellInfo;
  
  double lat_rad;
  double fi_prima;
  double cos_cua_fi_prima;
  double Ni;
  double a;
  double A1; 
  double A2;
  double J2;
  double J4;
  double J6;
  double Alfa;
  double Beta;
  double Gamma;
  double B_fi;
  double b;
  double Zeta;
  double Xi;
  double Eta;
  double sen_h_Xi;
  double delta_lambda;
  double Tau;
  int huso_id;
  
  MRData_Elipsoide eli;
 
  
  //Rellenamos los datos del elipsoide
  eli.a = 6378137.0; //Valor fijo para elipsoides del tipo WGS 84
  eli.b = 6356752.31424518;  //Valor fijo para elipsoides del tipo WGS 84
  eli.e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.a;
  eli.seg_e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.b;
  eli.seg_e_cua = eli.seg_e * eli.seg_e;
  eli.c = (eli.a * eli.a) / eli.b;
  eli.apl = (eli.a - eli.b) / eli.a;
  
	for(int i=0; i<inputs[0].num_kvs; i++)
  {
    cellId.parse(inputs[0].kvs[i].key);
    cellInfo.parse(inputs[0].kvs[i].value);
    
    cellInfo.posx -= 500000;
    
    if (cellInfo.huso.id < 0)
    {
      (cellInfo.posy) -= 10000000;
    }
    
    //Obtenemos el huso
    huso_id = cellInfo.huso.id;
    cellInfo.huso.id = abs(huso_id);
    
    //Convertimos las coordenadas
    fi_prima = cellInfo.posy / (6366197.724 * 0.9996);
    cos_cua_fi_prima = pow(cos(fi_prima), 2);
    Ni = eli.c / sqrt(1 + eli.seg_e_cua * cos_cua_fi_prima) * 0.9996;
    a = cellInfo.posx / Ni;
    A1 = sin(2 * fi_prima);
    A2 = A1 * cos_cua_fi_prima;
    J2 = fi_prima + 0.5 * A1;
    J4 = (3 * J2 + A2) / 4;
    J6 = (5 * J4 + A2 * cos_cua_fi_prima) / 3;
    Alfa = 0.75 * eli.seg_e_cua;
    Beta = 5 * Alfa * Alfa / 3;    
    Gamma = 35 * Alfa * Alfa * Alfa / 27;
    B_fi = 0.9996 * eli.c * (fi_prima - Alfa * J2 + Beta * J4 - Gamma * J6);
    b = (cellInfo.posy - B_fi) / Ni;
    Zeta = 0.5 * eli.seg_e_cua * a * a * cos_cua_fi_prima;
    Xi = a * (1 - Zeta / 3);
    Eta = b * (1 - Zeta) + fi_prima;
    sen_h_Xi = 0.5 * (exp(Xi) - exp(-Xi));
    delta_lambda = atan(sen_h_Xi / cos(Eta));
    Tau = atan(cos(delta_lambda) * tan(Eta));
    
    //Rellenamos la estructura de salida
    outCellInfo.cell = cellInfo.cell;
	  outCellInfo.bts = cellInfo.bts;
	  outCellInfo.mun = cellInfo.mun;
	  outCellInfo.sta = cellInfo.sta;
	 
	  outCellInfo.posx = delta_lambda * 180 / M_PI + cellInfo.huso.merid_gra;
    lat_rad = fi_prima + (1 + eli.seg_e_cua * cos_cua_fi_prima - 1.5 * eli.seg_e_cua * sin(fi_prima) * cos(fi_prima) * (Tau - fi_prima)) * (Tau - fi_prima);
    outCellInfo.posy = lat_rad * 180 / M_PI;
  
    writer[0]->emit(&cellId,&outCellInfo);
  }*/
}

void MACRO_mobmx_calc_bts_UTM_to_GMT::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
 /* //Inputs
  MRData_UInt btsId;
  MRData_Bts btsInfo;
  
  //Outputs
  MRData_Bts outBtsInfo;
  
  double lat_rad;
  double fi_prima;
  double cos_cua_fi_prima;
  double Ni;
  double a;
  double A1; 
  double A2;
  double J2;
  double J4;
  double J6;
  double Alfa;
  double Beta;
  double Gamma;
  double B_fi;
  double b;
  double Zeta;
  double Xi;
  double Eta;
  double sen_h_Xi;
  double delta_lambda;
  double Tau;
  int huso_id;
  
  MRData_Elipsoide eli;
 
  
  //Rellenamos los datos del elipsoide
  eli.a = 6378137.0; //Valor fijo para elipsoides del tipo WGS 84
  eli.b = 6356752.31424518;  //Valor fijo para elipsoides del tipo WGS 84
  eli.e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.a;
  eli.seg_e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.b;
  eli.seg_e_cua = eli.seg_e * eli.seg_e;
  eli.c = (eli.a * eli.a) / eli.b;
  eli.apl = (eli.a - eli.b) / eli.a;
  
	for(int i=0; i<inputs[0].num_kvs; i++)
  {
    btsId.parse(inputs[0].kvs[i].key);
    btsInfo.parse(inputs[0].kvs[i].value);
    
    outBtsInfo.copyFrom(&btsInfo);
    
    btsInfo.posxUTM -= 500000;
    
    if (btsInfo.huso.id < 0)
    {
      (btsInfo.posyUTM) -= 10000000;
    }
    
    //Obtenemos el huso
    huso_id = btsInfo.huso.id;
    btsInfo.huso.id = abs(huso_id);
    
    //Convertimos las coordenadas
    fi_prima = btsInfo.posyUTM / (6366197.724 * 0.9996);
    cos_cua_fi_prima = pow(cos(fi_prima), 2);
    Ni = eli.c / sqrt(1 + eli.seg_e_cua * cos_cua_fi_prima) * 0.9996;
    a = btsInfo.posxUTM / Ni;
    A1 = sin(2 * fi_prima);
    A2 = A1 * cos_cua_fi_prima;
    J2 = fi_prima + 0.5 * A1;
    J4 = (3 * J2 + A2) / 4;
    J6 = (5 * J4 + A2 * cos_cua_fi_prima) / 3;
    Alfa = 0.75 * eli.seg_e_cua;
    Beta = 5 * Alfa * Alfa / 3;    
    Gamma = 35 * Alfa * Alfa * Alfa / 27;
    B_fi = 0.9996 * eli.c * (fi_prima - Alfa * J2 + Beta * J4 - Gamma * J6);
    b = (btsInfo.posyUTM - B_fi) / Ni;
    Zeta = 0.5 * eli.seg_e_cua * a * a * cos_cua_fi_prima;
    Xi = a * (1 - Zeta / 3);
    Eta = b * (1 - Zeta) + fi_prima;
    sen_h_Xi = 0.5 * (exp(Xi) - exp(-Xi));
    delta_lambda = atan(sen_h_Xi / cos(Eta));
    Tau = atan(cos(delta_lambda) * tan(Eta));
    
	 
	  outBtsInfo.posxGMT = delta_lambda * 180 / M_PI + btsInfo.huso.merid_gra;
    lat_rad = fi_prima + (1 + eli.seg_e_cua * cos_cua_fi_prima - 1.5 * eli.seg_e_cua * sin(fi_prima) * cos(fi_prima) * (Tau - fi_prima)) * (Tau - fi_prima);
    outBtsInfo.posyGMT = lat_rad * 180 / M_PI;
  
    writer[0]->emit(&btsId,&outBtsInfo);
  }*/
}

void MACRO_mobmx_calc_vertices_UTM_to_GMT::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
 /* //Inputs
  MRData_UInt verticeId;
  MRData_Vertices_Voronoi verticeInfo;
  
  //Outputs
  MRData_Vertices_Voronoi outVerticeInfo;
  
  double lat_rad;
  double fi_prima;
  double cos_cua_fi_prima;
  double Ni;
  double a;
  double A1; 
  double A2;
  double J2;
  double J4;
  double J6;
  double Alfa;
  double Beta;
  double Gamma;
  double B_fi;
  double b;
  double Zeta;
  double Xi;
  double Eta;
  double sen_h_Xi;
  double delta_lambda;
  double Tau;
  int huso_id;
  
  MRData_Elipsoide eli;
 
  
  //Rellenamos los datos del elipsoide
  eli.a = 6378137.0; //Valor fijo para elipsoides del tipo WGS 84
  eli.b = 6356752.31424518;  //Valor fijo para elipsoides del tipo WGS 84
  eli.e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.a;
  eli.seg_e = sqrt((eli.a * eli.a) - (eli.b * eli.b)) / eli.b;
  eli.seg_e_cua = eli.seg_e * eli.seg_e;
  eli.c = (eli.a * eli.a) / eli.b;
  eli.apl = (eli.a - eli.b) / eli.a;
  
	for(int i=0; i<inputs[0].num_kvs; i++)
  {
    verticeId.parse(inputs[0].kvs[i].key);
    verticeInfo.parse(inputs[0].kvs[i].value);
    
    outVerticeInfo.copyFrom(&verticeInfo);
    
    verticeInfo.posxUTM -= 500000;
    
    if (verticeInfo.huso.id < 0)
    {
      (verticeInfo.posyUTM) -= 10000000;
    }
    
    //Obtenemos el huso
    huso_id = verticeInfo.huso.id;
    verticeInfo.huso.id = abs(huso_id);
    
    //Convertimos las coordenadas
    fi_prima = verticeInfo.posyUTM / (6366197.724 * 0.9996);
    cos_cua_fi_prima = pow(cos(fi_prima), 2);
    Ni = eli.c / sqrt(1 + eli.seg_e_cua * cos_cua_fi_prima) * 0.9996;
    a = verticeInfo.posxUTM / Ni;
    A1 = sin(2 * fi_prima);
    A2 = A1 * cos_cua_fi_prima;
    J2 = fi_prima + 0.5 * A1;
    J4 = (3 * J2 + A2) / 4;
    J6 = (5 * J4 + A2 * cos_cua_fi_prima) / 3;
    Alfa = 0.75 * eli.seg_e_cua;
    Beta = 5 * Alfa * Alfa / 3;    
    Gamma = 35 * Alfa * Alfa * Alfa / 27;
    B_fi = 0.9996 * eli.c * (fi_prima - Alfa * J2 + Beta * J4 - Gamma * J6);
    b = (verticeInfo.posyUTM - B_fi) / Ni;
    Zeta = 0.5 * eli.seg_e_cua * a * a * cos_cua_fi_prima;
    Xi = a * (1 - Zeta / 3);
    Eta = b * (1 - Zeta) + fi_prima;
    sen_h_Xi = 0.5 * (exp(Xi) - exp(-Xi));
    delta_lambda = atan(sen_h_Xi / cos(Eta));
    Tau = atan(cos(delta_lambda) * tan(Eta));
    
	 
	  outVerticeInfo.posxGMT = delta_lambda * 180 / M_PI + verticeInfo.huso.merid_gra;
    lat_rad = fi_prima + (1 + eli.seg_e_cua * cos_cua_fi_prima - 1.5 * eli.seg_e_cua * sin(fi_prima) * cos(fi_prima) * (Tau - fi_prima)) * (Tau - fi_prima);
    outVerticeInfo.posyGMT = lat_rad * 180 / M_PI;
  
    writer[0]->emit(&verticeId,&outVerticeInfo);
  }*/
}

void MACRO_mobmx_bts_spread::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
  /*//Inputs
  MRData_UInt cellId;
  MRData_CellMxUTM cellInfo;
  //Outputs
  MRData_UInt btsId;
  MRData_bts_coords btsInfo;
  
  for(int i=0; i<inputs[0].num_kvs; i++)
  {
    cellId.parse(inputs[0].kvs[i].key);
    cellInfo.parse(inputs[0].kvs[i].value);
    
    btsId.value = cellInfo.bts;
    btsInfo.id = cellInfo.bts;
    btsInfo.posx = cellInfo.posx;
    btsInfo.posy = cellInfo.posy;
    btsInfo.huso = cellInfo.huso;
    
    writer[0]->emit(&btsId, &btsInfo);
  }*/
}

void MACRO_mobmx_bts_norepeat::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
 /* //Inputs & Outputs
  MRData_UInt btsId;
  MRData_bts_coords btsInfo;
  
  //Cojo solo una de las varias repeticiones que puedo recibir
  btsId.parse(inputs[0].kvs[0].key);
  btsInfo.parse(inputs[0].kvs[0].value);
  
  writer[0]->emit(&btsId, &btsInfo);*/
}

void MACRO_mobmx_bts_spread_voronoi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
  /*//Inputs
  MRData_UInt btsId;
  MRData_bts_coords btsInfo;
  //Outputs
  MRData_UInt groupId;

  
  for(int i=0; i<inputs[0].num_kvs; i++)
  {
    btsInfo.parse(inputs[0].kvs[i].value);
    
    groupId.value = 1;
    
    writer[0]->emit(&groupId, &btsInfo);
  }*/
}

void MACRO_mobmx_calc_voronoi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
  /*//Inputs
  MRData_UInt groupId;
  MRData_bts_coords btsCoords;
  MRData_Contorno contorno;
  
  //Outputs
  MRData_UInt btsId;
  MRData_Bts btsInfo;
  MRData_Vertices_Voronoi vertices;


  VoronoiDiagramGenerator vdg;
  
  int *idValues;
  double *xValues;
  double *yValues;
  
  double *xValues_loc;
  double *yValues_loc;
  double *xValues_contorno;
  double *yValues_contorno;
  double rango_x;
  double rango_y;
  double posxMin;
  double posxMax;
  double posyMin;
  double posyMax;
  double *p_aux_x;
  double *p_aux_y;
  double *p_aux_x_sig;
  double *p_aux_y_sig;
  double dist;
  double dist_acum;
  double *p_aux_x_sal;
  double *p_aux_y_sal;
  int *p_aux_id;
  int npuntos_int;
  int j;
  int nint;
  int count_ext = 0;
  
  
  //Estructuras de salida Voronoi
  struct pto_origen *listaPuntos_sal;
  struct vertice *listaVertices_sal;

  
  //Relleno los vectores de coordenadas e identificadores
  if ((idValues = (int *) malloc(inputs[0].num_kvs * sizeof(int))) == NULL) 
  {
    perror("No puedo reservar memoria para idValues");
    exit(1);
  }
    
  if ((xValues = (double *) malloc(inputs[0].num_kvs * sizeof(double))) == NULL) 
  {
    perror("No puedo reservar memoria para xValues");
    exit(1);
  }
  
  if ((yValues = (double *) malloc(inputs[0].num_kvs * sizeof(double))) == NULL) 
  {
    perror("No puedo reservar memoria para yValues");
    exit(1);
  }
  
  fprintf(stderr, "He reservado memoria para los 3 arrays\n");
  
  p_aux_id = idValues;
  p_aux_x = xValues;
  p_aux_y = yValues;
  
  for (int i=0; i < inputs[0].num_kvs; i++, p_aux_id++, p_aux_x++, p_aux_y++) 
  {
    btsCoords.parse(inputs[0].kvs[i].value);
    
    //Guardo en los arrays el id y las coordenadas
    *p_aux_id = btsCoords.id;
    *p_aux_x = btsCoords.posx;
    *p_aux_y = btsCoords.posy;
  }
  //FIN Relleno los vectores de coordenadas e identificadores
  
  //Lectura de los datos del contorno
  if ((xValues_loc = (double *) malloc(inputs[1].num_kvs * sizeof(double))) == NULL) 
  {
    perror("No puedo reservar memoria para xValues_loc");
    exit(1);
  }
  if ((yValues_loc = (double *) malloc(inputs[1].num_kvs * sizeof(double))) == NULL) 
  {
    perror("No puedo reservar memoria para yValues_loc");
    exit(1);
  }
  
  p_aux_x = xValues_loc;
  p_aux_y = yValues_loc;
  
  for (int i = 0; (i < inputs[1].num_kvs); i++, p_aux_x++, p_aux_y++)
  {
    contorno.parse(inputs[1].kvs[i].value);
    
    *p_aux_x = contorno.posx;
    *p_aux_y = contorno.posy;
  }
  
  npuntos_int = 0;
  p_aux_x = xValues_loc;
  p_aux_y = yValues_loc;
  
  for (int i = 0; (i < inputs[1].num_kvs); i++, p_aux_x++, p_aux_y++) 
  {
    j = (i + 1) % inputs[1].num_kvs;
    p_aux_x_sig = xValues_loc + j;
    p_aux_y_sig = yValues_loc + j;
    dist = sqrt((*p_aux_x_sig - *p_aux_x) * (*p_aux_x_sig - *p_aux_x) +
                (*p_aux_y_sig - *p_aux_y) * (*p_aux_y_sig - *p_aux_y));
    nint = dist / DISTINT;
    if ((dist - nint * DISTINT) <= DISTMIN) 
    {
      nint--;
    }
    
    npuntos_int += nint;
  }
  
  count_ext = inputs[1].num_kvs + npuntos_int;
  
  if ((xValues_contorno = (double *) malloc((inputs[1].num_kvs + npuntos_int) * sizeof(double))) == NULL) 
  {
    perror("No puedo reservar memoria para xValues_contorno");
    exit(1);
  }
    
  if ((yValues_contorno = (double *) malloc((inputs[1].num_kvs + npuntos_int) * sizeof(double))) == NULL) 
  {
    perror("No puedo reservar memoria para yValues_contorno");
    exit(1);
  }
    
  p_aux_x_sal = xValues_contorno;
  p_aux_y_sal = yValues_contorno;
  npuntos_int = 0;
  p_aux_x = xValues_loc;
  p_aux_y = yValues_loc;
  
  for (int i = 0; (i < inputs[1].num_kvs); i++, p_aux_x++, p_aux_y++) 
  {
    j = (i + 1) % inputs[1].num_kvs;
    p_aux_x_sig = xValues_loc + j;
    p_aux_y_sig = yValues_loc + j;
    dist = sqrt((*p_aux_x_sig - *p_aux_x) * (*p_aux_x_sig - *p_aux_x) +
                (*p_aux_y_sig - *p_aux_y) * (*p_aux_y_sig - *p_aux_y));
       
    *(p_aux_x_sal++) = *p_aux_x;
    *(p_aux_y_sal++) = *p_aux_y;
    
    npuntos_int++;
    
    for (dist_acum = DISTINT; ((dist - dist_acum) > DISTMIN); dist_acum += DISTINT) 
    {
         *(p_aux_x_sal++) = *p_aux_x + dist_acum / dist * (*p_aux_x_sig - *p_aux_x);
         *(p_aux_y_sal++) = *p_aux_y + dist_acum / dist * (*p_aux_y_sig - *p_aux_y);
         npuntos_int++;
    }
  }
    
  free(xValues_loc);
  free(yValues_loc);
  
  //FIN Lectura de los datos del contorno
  
  
  //Unimos coordenadas del contorno y bts
  vdg.junta_ent_cont(&xValues, &yValues, xValues_contorno, yValues_contorno,
                    inputs[0].num_kvs, count_ext);
  
  //Calculamos los limites maximos y minimos          
  vdg.calc_lim(&posxMin, &posxMax, &posyMin, &posyMax, xValues, yValues, inputs[0].num_kvs + count_ext);
  
  vdg.generateVoronoi(xValues,yValues,inputs[0].num_kvs + count_ext,posxMin,posxMax,posyMin,posyMax, DISTMIN);
  
  vdg.postproceso(&listaPuntos_sal, &listaVertices_sal, &vdg, inputs[0].num_kvs, count_ext, posxMin, posxMax, posyMin, posyMax, DISTMIN);
  
  for (int i = 0; (i < inputs[0].num_kvs); i++) 
  {
    btsCoords.parse(inputs[0].kvs[i].value);
    
    btsId.value = idValues[i];
    
    btsInfo.id = btsId.value;
    btsInfo.posxUTM = xValues[i];
	  btsInfo.posyUTM = yValues[i];
	  
	  btsInfo.area = listaPuntos_sal[i].area;
	  btsInfo.comms = 0;
	
	  btsInfo.posxGMT = 0;
	  btsInfo.posyGMT = 0;
	  
	  btsInfo.huso = btsCoords.huso;
	  
	  //Relleno las bts adyacentes
	  btsInfo.adjSetLength( listaPuntos_sal[i].num_adyacentes);
	  
	  for (int j = 0; (j < listaPuntos_sal[i].num_adyacentes); j++)
    {
      btsInfo.adj[j] = idValues[listaPuntos_sal[i].ptos_adyacentes[j]];
    }
	  
	  writer[0]->emit(&btsId, &btsInfo);
    
    for (int j = 0; (j < listaPuntos_sal[i].num_lados); j++) 
    {
      struct lado *p_lado = (listaPuntos_sal[i].poligono) + j;
            
      if (j == 0) 
      {             
        vertices.idBts = idValues[i];
        vertices.idOrden = j;
        vertices.posxUTM = listaVertices_sal[p_lado->indv1_final].coordx;
	      vertices.posyUTM = listaVertices_sal[p_lado->indv1_final].coordy;
	      
	      vertices.posxGMT = 0;
	      vertices.posyGMT = 0;
	      
	      vertices.huso = btsCoords.huso;
	      
	      writer[1]->emit(&btsId, &vertices);
      }
                    
      vertices.idBts = idValues[i];
      vertices.idOrden = j + 1;
      vertices.posxUTM = listaVertices_sal[p_lado->indv2_final].coordx;
	    vertices.posyUTM = listaVertices_sal[p_lado->indv2_final].coordy;
	    
	    vertices.posxGMT = 0;
	    vertices.posyGMT = 0;
              
      writer[1]->emit(&btsId, &vertices);
    }
  }
  
  for (int i = 0; (i < (inputs[0].num_kvs + count_ext)); i++)
  {
    free(listaPuntos_sal[i].poligono);
    free(listaPuntos_sal[i].ptos_adyacentes);
  }
  
  free(listaPuntos_sal);
  free(listaVertices_sal);
  free(idValues);
  free(xValues);
  free(yValues);*/

}

