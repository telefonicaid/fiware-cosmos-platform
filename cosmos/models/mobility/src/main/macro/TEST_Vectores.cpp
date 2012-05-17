#include "TEST_Module.h"

#define mob_conf_max_comms_bts 70000
#define mob_conf_max_bts_area 4.2
//#define mob_conf_max_bts_area 10.83515
#define mob_conf_home_labelgroup_id 3
#define mob_conf_min_dist_second_home 50165.21	//Mexico
//#define mob_conf_min_dist_second_home 49342.85 	//Chile
////////////////////////////////////////////////////////////////////
/////////////////////  GLOBAL INFORMATION  /////////////////////////
////////////////////////////////////////////////////////////////////
MRData_ClusterSet clientClusters;
MRData_ClusterSet clientbtsClusters;
MRData_ClusterSet btsClusters;
/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// PARSERS ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
#define GET_CLUST_INIT_MX pos=0; pos_field=0; char *offset; int pos_cell=0; int length_cell=0; int cont=0; int length_line=strlen(line);
#define GET_CLUST_NEXT_FIELD pos_field=pos; while( line[pos] != '|' and line[pos] != '\0' ) pos++; line[pos]='\0';  pos++;
#define GET_CLUST_GET_INT( _int ) _int = atoi(&line[pos_field]);
#define GET_CLUST_GET_DOUBLE( _double  ) _double = atof(&line[pos_field]);

////////
// Parse clusters
////////
void MACRO_mobmx_parse_client_clusters::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_UInt clusterId;
	MRData_Cluster clusterInfo;

	MRData_Double coord;

	// Initialize global client clusters
	clientClusters.clusterSetLength(0);

	for (std::vector<char*>::iterator iter = lines.begin(); iter < lines.end() ; iter++)
	{
		clusterInfo.coords.comsSetLength(0);
		MR_PARSER_INIT(*iter);		
		GET_CLUST_INIT_MX
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_INT(clusterInfo.label)
		clusterId.value = clusterInfo.label;
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_INT(clusterInfo.labelgroup)
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_DOUBLE(clusterInfo.mean)
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_DOUBLE(clusterInfo.distance)
		for(int j=0; j<96; j++)
		{
			GET_CLUST_NEXT_FIELD
			GET_CLUST_GET_DOUBLE(coord.value)
			clusterInfo.coords.comsAdd()->copyFrom(&coord);
		}
		clientClusters.clusterAdd()->copyFrom(&clusterInfo);
		writer->emit(&clusterId,&clusterInfo);
	}
}
void MACRO_mobmx_parse_clientbts_clusters::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_UInt clusterId;
	MRData_Cluster clusterInfo;

	MRData_Double coord;

	// Initialize global client clusters
	clientClusters.clusterSetLength(0);

	for (std::vector<char*>::iterator iter = lines.begin(); iter < lines.end() ; iter++)
	{
		clusterInfo.coords.comsSetLength(0);
		MR_PARSER_INIT(*iter);		
		GET_CLUST_INIT_MX
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_INT(clusterInfo.label)
		clusterId.value = clusterInfo.label;
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_INT(clusterInfo.labelgroup)
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_DOUBLE(clusterInfo.mean)
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_DOUBLE(clusterInfo.distance)
		for(int j=0; j<96; j++)
		{
			GET_CLUST_NEXT_FIELD
			GET_CLUST_GET_DOUBLE(coord.value)
			clusterInfo.coords.comsAdd()->copyFrom(&coord);
		}
		clientbtsClusters.clusterAdd()->copyFrom(&clusterInfo);
		writer->emit(&clusterId,&clusterInfo);
	}
}
void MACRO_mobmx_parse_bts_clusters::parseLines( std::vector<char*> lines , bool eof, KVWriterInterface *writer )
{
	// Input: txt
	// Outputs
	MRData_UInt clusterId;
	MRData_Cluster clusterInfo;

	MRData_Double coord;

	// Initialize global bts clusters
	btsClusters.clusterSetLength(0);

	for (std::vector<char*>::iterator iter = lines.begin(); iter < lines.end() ; iter++)
	{
		clusterInfo.coords.comsSetLength(0);
		MR_PARSER_INIT(*iter);		
		GET_CLUST_INIT_MX
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_INT(clusterInfo.label)
		clusterId.value = clusterInfo.label;
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_INT(clusterInfo.labelgroup)
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_DOUBLE(clusterInfo.mean)
		GET_CLUST_NEXT_FIELD
		GET_CLUST_GET_DOUBLE(clusterInfo.distance)
		for(int j=0; j<96; j++)
		{
			GET_CLUST_NEXT_FIELD
			GET_CLUST_GET_DOUBLE(coord.value)
			clusterInfo.coords.comsAdd()->copyFrom(&coord);
		}
		btsClusters.clusterAdd()->copyFrom(&clusterInfo);
		writer->emit(&clusterId,&clusterInfo);
	}
}

////////////////////////////
// VECTOR COMMS IN A NODE //
////////////////////////////
void MACRO_mobmx_vector_filt_clients::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs/outputs
	MRData_UInt node;
	MRData_CdrMx cdr;
	
	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			node.parse(inputs[0].kvs[j].key);
			cdr.parse(inputs[0].kvs[j].value);
			writer[0]->emit(&node,&cdr);
		}
	}
}
void MACRO_mobmx_vector_spread_nodedayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt node;
	MRData_CdrMx cdr;
	//Outputs
	MRData_Node_Bts noddayhour;
	MRData_Void mrvoid;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		cdr.parse(inputs[0].kvs[i].value);
		noddayhour.phone = node.value;
		noddayhour.bts = 0;
		// Group 0: MON-TUE, Group 1: FRI, Group 2: SAT, Group 3: SUN
		int date = cdr.date.week_day;
		if(date == 0) {	noddayhour.wday = 3;}
		else if(date == 5)	{	noddayhour.wday = 1;}
		else if(date == 6)	{	noddayhour.wday = 2;}
		else	{	noddayhour.wday = 0;}
		noddayhour.range = cdr.time.hour;
		writer[0]->emit(&noddayhour,&mrvoid);
	}
}
void MACRO_mobmx_vector_get_ncoms_nodedayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Node_Bts noddayhour;
	//Outputs
	MRData_Node_Bts nodbtsday;
	MRData_TwoInt hour_comms;	// num1: hour	num2:ncoms

	noddayhour.parse(inputs[0].kvs[0].key);
	nodbtsday.copyFrom(&noddayhour);
	nodbtsday.range = 0;
	hour_comms.num1 = noddayhour.range;
	hour_comms.num2 = inputs[0].num_kvs;

	writer[0]->emit(&nodbtsday,&hour_comms);
}
void MACRO_mobmx_vector_create_node_dayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Node_Bts inkey;
	MRData_TwoInt hour_comms;
	//Outputs
	MRData_TwoInt nodbts;	// num1: node	num2: bts
	MRData_DailyVector clusvector;
 
	inkey.parse(inputs[0].kvs[0].key);
	clusvector.hoursSetLength(0);

	for(int i=0; i<24; i++)
	{
		bool added = false;
		
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			hour_comms.parse(inputs[0].kvs[j].value);
			if(hour_comms.num1 == i) 
			{
				added = true;
				break;
			}
		}
		if(!added)
		{
			hour_comms.num2 = 0;
		}
		hour_comms.num1 = inkey.wday;
		clusvector.hoursAdd()->copyFrom(&hour_comms);
	}
	nodbts.num1 = inkey.phone;
	nodbts.num2 = inkey.bts;
	writer[0]->emit(&nodbts,&clusvector);
}
void MACRO_mobmx_vector_fuse_node_daygroup::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodebts;	// num1: node	num2: bts
	MRData_DailyVector invector;
	//Outputs
	MRData_Node_Bts output;
	MRData_ClusterVector clusvector;

	clusvector.comsSetLength(0);
	nodebts.parse(inputs[0].kvs[0].key);

	for(int group=0; group<4; group++)
	{
		bool added = false;
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			invector.parse(inputs[0].kvs[j].value);
			if(invector.hours[j].num1 == group)
			{
				for(int k=0; k < invector.hours_length; k++)
				{
					MRData_Double hours;
					hours.value = (double) invector.hours[k].num2;
					clusvector.comsAdd()->copyFrom(&hours);
				}
				added = true;
				break;
			}
		}
		if(!added)
		{
			MRData_Double hours;
			hours.value = 0;
			for(int k=0; k<24; k++)
			{	clusvector.comsAdd()->copyFrom(&hours);}
		}
	}
    
	output.phone = nodebts.num1;
	output.bts = nodebts.num2;
	output.wday = 0;
	output.range = 0;
	writer[0]->emit(&output,&clusvector);
}

/////////////////////////////////////
// VECTOR COMMS IN A BTS IN A NODE //
/////////////////////////////////////
void MACRO_mobmx_vector_spread_nodbts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node; 
	MRData_NodeMx_Counter counter;
	// Outputs
	MRData_Node_Bts nodebts;
	MRData_UInt comms;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		counter.parse(inputs[0].kvs[i].value);

		nodebts.phone = node.value;
		for(int j=0; j<counter.bts_length; j++)
		{
			nodebts.bts = counter.bts[j].bts;
			int group;
			if(counter.bts[j].wday == 0)		{	group = 3;}
			else if(counter.bts[j].wday == 5)	{	group = 1;}
			else if(counter.bts[j].wday == 6)	{	group = 2;}
			else								{	group = 0;}
			nodebts.wday = group;
			nodebts.range = counter.bts[j].range;
			comms.value = counter.bts[j].count;
			writer[0]->emit(&nodebts,&comms);
		}
	}
}
void MACRO_mobmx_vector_sum_groupcomms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Node_Bts innodebts;
	MRData_UInt comms;
	//Outputs
	MRData_TwoInt node_bts;	//	num1: node	num2: bts
	MRData_Bts_Counter counter;

	int ncoms = 0;

	innodebts.parse(inputs[0].kvs[0].key);
	node_bts.num1 = innodebts.phone;
	node_bts.num2 = innodebts.bts;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		comms.parse(inputs[0].kvs[i].value);
		ncoms += comms.value;
	}
	counter.bts = innodebts.bts;
	counter.wday = innodebts.wday;
	counter.range = innodebts.range;
	counter.count = ncoms;

	writer[0]->emit(&node_bts,&counter);
}

void MACRO_mobmx_vector_filt_clientbts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt node_bts;	//	Num1: node	Num2: bts
	MRData_Bts_Counter counter;
	// Outputs
	MRData_Node_Bts nodbts;
	MRData_TwoInt hour_coms; // Num1: hour	Num2: coms

	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			node_bts.parse(inputs[0].kvs[j].key);
			counter.parse(inputs[0].kvs[j].value);
			nodbts.phone = node_bts.num1;
			nodbts.bts = node_bts.num2;
			nodbts.wday = counter.wday;
			nodbts.range = 0;
			hour_coms.num1 = counter.range;
			hour_coms.num2 = counter.count;
			writer[0]->emit(&nodbts,&hour_coms);
		}
	}
}


///////////////////////////
// VECTOR COMMS IN A BTS //
///////////////////////////

// Filter bts by ncomms and max area
void MACRO_mobmx_vector_filter_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_UInt bts;
	MRData_TwoInt group_hour;	//num1: group	Num2: hour
	// Outputs
	MRData_Bts_Counter counter;
	MRData_Void mrvoid;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		bts.parse(inputs[0].kvs[i].key);
		group_hour.parse(inputs[0].kvs[i].value);
		counter.bts = bts.value;
		counter.wday = group_hour.num1;
		counter.range = group_hour.num2;
		counter.count = 0;
		writer[0]->emit(&counter,&mrvoid);
	}
}
// Sum of coms by bts
void MACRO_mobmx_vector_sum_coms_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Bts_Counter counter;
	//Outputs
	MRData_Node_Bts nodbts;
	MRData_TwoInt hour_comms;	//num1: hour	num2: comms

	counter.parse(inputs[0].kvs[0].key);
	nodbts.phone = 0;
	nodbts.bts = counter.bts;
	nodbts.wday = counter.wday;
	nodbts.range = 0;
	hour_comms.num1 = counter.range;
	hour_comms.num2 = inputs[0].num_kvs;

	writer[0]->emit(&nodbts,&hour_comms);
}

////////////////////////////////////////
///////////// DISTANCES ////////////////
////////////////////////////////////////

// Minimun euclidean distance - Client
void MACRO_mobmx_cluster_client_get_min_distance::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Node_Bts nodbts;
	MRData_ClusterVector clusVector;
	// Ouputs
	MRData_UInt node;
	MRData_Cluster cluster;
	// Intermediate
	double mindist, dist;
	double com, ccom;
	int clusId = -1;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodbts.parse(inputs[0].kvs[i].key);
		clusVector.parse(inputs[0].kvs[i].value);
		cluster.coords.copyFrom(&clusVector);
		mindist = 1000;

		// Calculate min dist - Euclidean distance
		for(int nClus=0; nClus<clientClusters.cluster_length; nClus++)
		{
			dist = 0;
			for(int nComs=0; nComs<clusVector.coms_length; nComs++)
			{
				ccom = clientClusters.cluster[nClus].coords.coms[nComs].value;
				com = clusVector.coms[nComs].value;
				dist += ((ccom - com) * (ccom - com));
			}
			if(dist < mindist)
			{
				mindist = dist;
				clusId = nClus;
			}
		}
		mindist = sqrt(mindist);

		node.value = nodbts.phone;
		
		// FILTER BY DISTANCE
		cluster.label = clientClusters.cluster[clusId].label;
		cluster.labelgroup = clientClusters.cluster[clusId].labelgroup;
		if(mindist > clientClusters.cluster[clusId].distance) // High to cluster level
		{
			cluster.confident = 0;
		}
		else
		{
			cluster.confident = 1;
		}
		cluster.distance = mindist;
		cluster.mean = 0;
		
		writer[0]->emit(&node,&cluster);
	}
}

// Minimum pearson distance
void MACRO_mobmx_cluster_clientbts_get_min_distance::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Node_Bts nodbts;
	MRData_ClusterVector clusVector;
	// Outputs
	MRData_UInt node;
	MRData_Poi poi;
	MRData_TwoInt node_bts;
	MRData_Cluster cluster;
	// Intermediate
	double dist, mindist;
	double sumcom, sumccom, csumcom, csumccom, sumcomxccom;
	double com, ccom;
	int clusId = -1;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodbts.parse(inputs[0].kvs[i].key);
		clusVector.parse(inputs[0].kvs[i].value);
		node_bts.num1 = nodbts.phone;
		node_bts.num2 = nodbts.bts;
		cluster.coords.copyFrom(&clusVector);
		mindist = 1000;

		// Calculate min dist - Pearson distance
		for(int nClus=0; nClus<clientbtsClusters.cluster_length; nClus++)
		{
			sumcom = sumccom = csumcom = csumccom = sumcomxccom = 0;
			for(int nComs=0; nComs<clusVector.coms_length; nComs++)
			{
				com = clusVector.coms[nComs].value;
				ccom = clientbtsClusters.cluster[nClus].coords.coms[nComs].value;
				sumcom += com;				// S1
				sumccom += ccom;			// S2
				csumcom += com * com;		// SC1
				csumccom += ccom * ccom;	// SC2
				sumcomxccom += com * ccom;	// S12
			}
			
			int nElem = clusVector.coms_length;
			dist = 1.0 - (((nElem*sumcomxccom)-(sumcom*sumccom))/(sqrt((nElem*csumcom)-(sumcom*sumcom))*sqrt((nElem*csumccom)-(sumccom*sumccom))));

			if(dist < mindist)
			{
				mindist = dist;
				clusId = nClus;
			}
		}
		poi.node = node.value = nodbts.phone;
		poi.bts = nodbts.bts;
		
		//FILTER BY DISTANCE
		cluster.label = clientbtsClusters.cluster[clusId].label;
		cluster.labelgroup = clientbtsClusters.cluster[clusId].labelgroup;
		poi.labelnodebts = clientbtsClusters.cluster[clusId].label;
		poi.labelgroupnodebts = clientbtsClusters.cluster[clusId].labelgroup;

		if(mindist > clientbtsClusters.cluster[clusId].distance)
		{
			cluster.confident = 0;
			poi.confidentnodebts = 0;
		}
		else
		{
			cluster.confident = 1;
			poi.confidentnodebts = 1;
		}
		cluster.mean = 0;
		poi.id = 0;
		poi.distancenodebts = cluster.distance = mindist;
		poi.labelnode = poi.labelgroupnode = poi.confidentnode = poi.distancenode = 0;
		poi.labelbts = poi.labelgroupbts = poi.confidentbts = poi.distancebts = 0;
		poi.inoutWeek = poi.inoutWend = -1;
		writer[0]->emit(&node,&poi);
		writer[1]->emit(&node_bts,&cluster);
	}
}

// Minimun euclidean distance - Bts
void MACRO_mobmx_cluster_bts_get_min_distance::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Node_Bts nodbts;
	MRData_ClusterVector clusVector;

	// Ouputs
	MRData_UInt bts;
	MRData_Cluster cluster;
	// Intermediate
	double mindist, dist;
	double com, ccom;
	int clusId = -1;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodbts.parse(inputs[0].kvs[i].key);
		clusVector.parse(inputs[0].kvs[i].value);
		cluster.coords.copyFrom(&clusVector);
		mindist = 1000000;

		// Calculate min dist - Euclidean distance
		for(int nClus=0; nClus<btsClusters.cluster_length; nClus++)
		{
			dist = 0;
			for(int nComs=0; nComs<clusVector.coms_length; nComs++)
			{
				ccom = btsClusters.cluster[nClus].coords.coms[nComs].value;
				com = clusVector.coms[nComs].value;
				dist += ((ccom - com) * (ccom - com));
			}
			if(dist < mindist)
			{
				mindist = dist;
				clusId = nClus;
			}
		}
		mindist = sqrt(mindist);
		
		bts.value = nodbts.bts;
		cluster.label = btsClusters.cluster[clusId].label;
		cluster.labelgroup = btsClusters.cluster[clusId].labelgroup;
		// FILTER BY DISTANCE (only in Chile)
		/*if(mindist > btsClusters.cluster[clusId].mean)
		{
			cluster.confident = 0;
		}
		else
		{*/
			cluster.confident = 1;
		//}

		cluster.distance = mindist;
		cluster.mean = 0;
		writer[0]->emit(&bts,&cluster);
	}
}

// Filter cluster by num of comms and area of bts
void MACRO_mobmx_filter_bts_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt bts;
	MRData_Cluster cluster;
	MRData_Bts bts_info;

	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		bts.parse(inputs[1].kvs[i].key);
		bts_info.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			cluster.parse(inputs[0].kvs[j].value);
			//if(bts_info.area <= mob_conf_max_bts_area && bts_info.comms >= mob_conf_max_comms_bts)  // CHILE
			if(bts_info.comms < mob_conf_max_comms_bts && bts_info.area > mob_conf_max_bts_area) // MEXICO
			{
				//cluster.confident = 1;
				cluster.confident = 0;
			}
			writer[0]->emit(&bts,&cluster);
		}
	}
}

// AGGREGATION OF RESULTS
void MACRO_mobmx_cluster_agg_node_cluster::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Poi poi;
	MRData_Cluster nodeclus;
	//Outputs
	MRData_TwoInt nodbts; //num1: node	num2:bts
	MRData_UInt nodlbl;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poi.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			nodeclus.parse(inputs[1].kvs[j].value);
			nodbts.num1 = poi.node;
			nodbts.num2 = poi.bts;
			poi.labelnode = nodeclus.label;
			poi.labelgroupnode = nodeclus.labelgroup;
			poi.confidentnode = nodeclus.confident;
			poi.distancenode = nodeclus.distance;
			writer[0]->emit(&nodbts,&poi);

			if(poi.confidentnode == 1 && poi.confidentnodebts == 0)
			{
				nodlbl.value = poi.labelnode;
				writer[1]->emit(&nodlbl,&poi);
			}
		}
	}
}
/***********************************************/
/********* Majority POIS by node label *********/
/***********************************************/
void MACRO_mobmx_cluster_spread_nodelbl_poilbl::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Poi poiInfo;
	//Outputs
	MRData_TwoInt nodlbl_poilbl; // num1: Node label	num2: Poi label
	MRData_Void mr_void;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poiInfo.parse(inputs[0].kvs[i].value);
		
		if(poiInfo.confidentnode == 1 && poiInfo.confidentnodebts == 1)
		{
			nodlbl_poilbl.num1 = poiInfo.labelnode;
			nodlbl_poilbl.num2 = poiInfo.labelnodebts;
			writer[0]->emit(&nodlbl_poilbl,&mr_void);
		}
	}
}
void MACRO_mobmx_cluster_count_majPoi_by_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt pairLabels; // num1: Node label	num2: Poi label
	// Outputs
	MRData_UInt nodelbl;
	MRData_TwoInt poilbl_count; // num1: poi label	num2: counter

	pairLabels.parse(inputs[0].kvs[0].key);
	nodelbl.value = pairLabels.num1;
	poilbl_count.num1 = pairLabels.num2;
	poilbl_count.num2 = inputs[0].num_kvs;

	writer[0]->emit(&nodelbl,&poilbl_count);
}
void MACRO_mobmx_cluster_get_majPoi_by_node::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt nodelbl;
	MRData_TwoInt poilbl_count; // num1: poi label	num2: counter
	// Outputs
	MRData_UInt poilbl;

	int max, label;
	max = label = 0;

	nodelbl.parse(inputs[0].kvs[0].key);
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poilbl_count.parse(inputs[0].kvs[i].value);
		if(poilbl_count.num2 > max)
		{
			label = poilbl_count.num1;
			max = poilbl_count.num2;
		}
	}
	poilbl.value = label;
	writer[0]->emit(&nodelbl,&poilbl);
}
void MACRO_mobmx_cluster_join_potPoi_label::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt nodbts; // num1: node		num2: bts
	MRData_Poi potPoi;
	MRData_UInt majPoiLbl;
	// Outputs
	MRData_Void mr_void;

	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		majPoiLbl.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			potPoi.parse(inputs[0].kvs[j].value);
			if(majPoiLbl.value == potPoi.labelnodebts)
			{
				nodbts.num1 = potPoi.node;
				nodbts.num2 = potPoi.bts;
				writer[0]->emit(&nodbts,&mr_void);
			}
		}
	}
}
void MACRO_mobmx_cluster_agg_potPoi_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TwoInt nodbts;
	MRData_Poi poi;
	MRData_Cluster cluster;
	//Outputs
	MRData_UInt bts;

	nodbts.parse(inputs[0].kvs[0].key);
	poi.parse(inputs[0].kvs[0].value);
	cluster.parse(inputs[1].kvs[0].value);

	if(inputs[2].num_kvs > 0)	// Exists the potential poi
	{
		poi.confidentnodebts = cluster.confident = 1;
	}
	bts.value = poi.bts;
	writer[0]->emit(&bts,&poi);
	writer[1]->emit(&nodbts,&cluster);
}
void MACRO_mobmx_cluster_agg_bts_cluster::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Poi poi;
	MRData_Cluster btsclus;
	MRData_Bts btsinfo;
	//Output
	MRData_UInt bts;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poi.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			btsclus.parse(inputs[1].kvs[j].value);
			bts.value = poi.bts;		
			poi.labelbts = btsclus.label;
			poi.labelgroupbts = btsclus.labelgroup;
			poi.confidentbts = btsclus.confident;
			poi.distancebts = btsclus.distance;
			writer[0]->emit(&bts,&poi);
		}
	}
}

////////////////////////////////////
////////// Second homes ////////////
////////////////////////////////////
void MACRO_mobmx_poi_cell_to_bts::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_CellMx info;
	// Outputs
	MRData_UInt bts;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		info.parse(inputs[0].kvs[i].value);
		bts.value = info.bts;
		writer[0]->emit(&bts,&info);
	}
}
void MACRO_mobmx_poi_join_pois_btscoord::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Poi poi;
	MRData_CellMx btsinfo;
	// Outputs
	MRData_UInt node;
	MRData_TwoInt node_bts;	//Num1: node	Num2: bts
	MRData_PoiPos poipos;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poi.parse(inputs[0].kvs[i].value);
		for(int j=0; j<1; j++)
		{
			btsinfo.parse(inputs[1].kvs[j].value);
			poipos.node = node.value = poi.node;
			poipos.bts = poi.bts;

			node_bts.num1 = node.value;
			node_bts.num2 = poipos.bts;
			writer[1]->emit(&node_bts,&poi);

			if(poi.confidentnodebts == 0)
			{
				poipos.label = 0;
			}
			else
			{
                poipos.label = poi.labelgroupnodebts;
			}
			poipos.posx = btsinfo.posx;
			poipos.posy = btsinfo.posy;
			poipos.inoutWeek = poi.inoutWeek;
			poipos.inoutWend = poi.inoutWend;
			// BORRAR
			poipos.radiusWeek = poipos.distCMWeek = poipos.radiusWend = poipos.distCMWend = -1;
			////
			writer[0]->emit(&node,&poipos);
			
		}
	}
}
void MACRO_mobmx_poi_join_pois_vi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt node;
	MRData_PoiPos poipos;
	MRData_Mob_VI_MobVars indvars;
	// Outputs
	MRData_TwoInt node_bts; // Num1: node	Num2: bts
	MRData_TwoInt ioweek_iowend; //Num1: in/out week	Num2: in/out wend

	double distx, disty, dist;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node.parse(inputs[0].kvs[i].key);
		poipos.parse(inputs[0].kvs[i].value);

		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			indvars.parse(inputs[1].kvs[j].value);
			for(int n=0; n<indvars.vars_length; n++)
			{
				distx = poipos.posx - indvars.vars[n].masscenter_utmx;
				disty = poipos.posy - indvars.vars[n].masscenter_utmy;
				dist = sqrt(distx * distx + disty * disty);

				// Individual variables Monday - Friday
				if(indvars.vars[n].workingday == 1)
				{
					poipos.inoutWeek = dist <= indvars.vars[n].radius ? 1 : 0;
					poipos.radiusWeek = indvars.vars[n].radius;
					poipos.distCMWeek = dist;
				}
				// Individual variables Saturday - Sunday
				if(indvars.vars[n].workingday == 0)
				{
					poipos.inoutWend = dist <= indvars.vars[n].radius ? 1 : 0;
					poipos.radiusWend = indvars.vars[n].radius;
					poipos.distCMWend = dist;
				}
			}
			writer[0]->emit(&node,&poipos);
			node_bts.num1 = poipos.node;
			node_bts.num2 = poipos.bts;
			ioweek_iowend.num1 = poipos.inoutWeek;
			ioweek_iowend.num2 = poipos.inoutWend;
			writer[1]->emit(&node_bts,&ioweek_iowend);
		}
	}
}
void MACRO_mobmx_poi_get_pairs_sechome_pois::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs 
	MRData_PoiPos poi_in;
	MRData_PoiPos poi_out;
	//Outputs
	MRData_TwoInt pairbts;	//Num1: bts_in	Num2: bts_out
	MRData_UInt node;

	double distx, disty, dist;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		poi_in.parse(inputs[0].kvs[i].value);
		if(poi_in.inoutWeek == 1 && poi_in.label == mob_conf_home_labelgroup_id)	// In the Monday-Friday radius
		{
			for(int j=0; j<inputs[0].num_kvs; j++)
			{
				poi_out.parse(inputs[0].kvs[j].value);
				if(poi_out.inoutWeek == 0 && poi_out.label == mob_conf_home_labelgroup_id)	// Out of Monday-Friday radius
				{
					node.value = poi_in.node;
					pairbts.num1 = poi_in.bts;
					pairbts.num2 = poi_out.bts;
					distx = poi_in.posx - poi_out.posx;
					disty = poi_in.posy - poi_out.posy;
					dist = sqrt(distx*distx + disty*disty);
					if(dist >= mob_conf_min_dist_second_home)
					{
						writer[0]->emit(&pairbts,&node);
					}
				}
			}
		}
	}
}
void MACRO_mobmx_poi_filter_sechome_adjacent::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt pairbts;	//Num1: bts_in	Num2: bts_out
	MRData_UInt node;
	// Outputs
	MRData_TwoInt node_bts; //Num1: node	Num2: bts
	MRData_Void mr_void;

	if(inputs[1].num_kvs == 0)	// The pair of bts are not adjacents
	{
		for(int i=0; i<inputs[0].num_kvs; i++)
		{
			pairbts.parse(inputs[0].kvs[i].key);
			node.parse(inputs[0].kvs[i].value);
			node_bts.num1 = node.value;
			node_bts.num2 = pairbts.num2;
			writer[0]->emit(&node_bts,&mr_void);
		}
	}
} 
void MACRO_mobmx_poi_delete_sechome_duplicate::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Input/Output
	MRData_TwoInt nod_bts;
	MRData_Void mr_void;

	nod_bts.parse(inputs[0].kvs[0].key);
	writer[0]->emit(&nod_bts,&mr_void);
}
void MACRO_mobmx_poi_join_sechome_results::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt node_bts;
	MRData_Poi poi;
	MRData_TwoInt ioweek_iowend;
	
	node_bts.parse(inputs[0].kvs[0].key);
	poi.parse(inputs[0].kvs[0].value);
	ioweek_iowend.parse(inputs[1].kvs[0].value);
	poi.inoutWeek = ioweek_iowend.num1;
	poi.inoutWend = ioweek_iowend.num2;
	if(inputs[2].num_kvs != 0)	// It is a secondary home
	{
		poi.labelnodebts = poi.labelgroupnodebts = 100;
	}
	writer[0]->emit(&node_bts,&poi);
}

///////////////////////////////////
// SUM ADJACENT NODE-BTS VECTORS //
///////////////////////////////////
void MACRO_mobmx_poi_spread_nodebts_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Node_Bts nodbts;
	MRData_ClusterVector comms;
	// Outputs
	MRData_TwoInt node_bts;	//Num1: node	Num2: bts

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodbts.parse(inputs[0].kvs[i].key);
		comms.parse(inputs[0].kvs[i].value);
		node_bts.num1 = nodbts.phone;
		node_bts.num2 = nodbts.bts;
		writer[0]->emit(&node_bts,&comms);
	}
}
void MACRO_mobmx_poi_join_poivector_poi::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt node_bts;
	MRData_ClusterVector comms;
	MRData_Poi poi_info;
	// Outputs
	MRData_Cluster poi_comms;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		node_bts.parse(inputs[0].kvs[i].key);
		comms.parse(inputs[0].kvs[i].value);
		for(int j=0; j<inputs[1].num_kvs; j++)
		{
			poi_info.parse(inputs[1].kvs[j].value);
			node_bts.num2 = poi_info.id;
			poi_comms.label = poi_info.labelnodebts;
			poi_comms.labelgroup = poi_info.labelgroupnodebts;
			poi_comms.confident = poi_info.confidentnodebts;
			poi_comms.mean = poi_comms.distance = 0;
			poi_comms.coords.copyFrom(&comms);
			writer[0]->emit(&node_bts,&poi_comms);
		}
	}
}

////////////////////////////////
///////     OUTPUT     /////////
////////////////////////////////

int MACRO_mobmx_vector_pois_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_Poi poi;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		poi.parse(kvSet.kvs[i].value);
		// POIID|NODE|BTS|POILABEL|POIGROUPLABEL|CONFIDENT|INOUT_MONFRI|INOUT_SATSUN|NODELABEL|NODEGROUPLABEL|CONFIDENT|BTSLABEL|BTSGROUPLABEL|CONFIDENT
		poi.labelgroupnodebts = poi.confidentnodebts == 0 ? 0 : poi.labelgroupnodebts;
		poi.labelgroupnode = poi.confidentnode == 0 ? 0 : poi.labelgroupnode;
		poi.labelgroupbts = poi.confidentbts == 0 ? 0 : poi.labelgroupbts;
		total += fprintf(file,"%d|%lu|%lu|",poi.id,poi.node,poi.bts);
		total += fprintf(file,"%d|%d|%d|",poi.labelnodebts, poi.labelgroupnodebts, poi.confidentnodebts);
		total += fprintf(file,"%d|%d|",poi.inoutWeek,poi.inoutWend);
		total += fprintf(file,"%d|%d|%d|",poi.labelnode, poi.labelgroupnode, poi.confidentnode);
		total += fprintf(file,"%d|%d|%d\n",poi.labelbts, poi.labelgroupbts, poi.confidentbts);
	}
	return total;
}
int MACRO_mobmx_vector_pois_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}

// Export vector client or bts
int MACRO_mobmx_vector_oneid_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_UInt id;
	MRData_Cluster cluster;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		id.parse(kvSet.kvs[i].key);
		cluster.parse(kvSet.kvs[i].value);
		// NODE/BTS|LABEL|LABELGROUP|CONFIDENT|C0|...|C95
		total += fprintf(file,"%lu|%d|%d|%d",id.value,cluster.label,cluster.labelgroup,cluster.confident);
		for(int j=0; j<cluster.coords.coms_length; j++)
		{
			total += fprintf(file,"|%f",cluster.coords.coms[j].value);
		}
		total += fprintf(file,"\n");
	}
	return total;
}
int MACRO_mobmx_vector_oneid_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}

// Export vector clientbts
int MACRO_mobmx_vector_nodbts_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_TwoInt node_bts;
	MRData_Cluster cluster;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		node_bts.parse(kvSet.kvs[i].key);
		cluster.parse(kvSet.kvs[i].value);
		// NODE|BTS|LABEL|LABELGROUP|CONFIDENT|C0|...|C95
		total += fprintf(file,"%lu|%lu|%d|%d|%d",node_bts.num1,node_bts.num2,cluster.label,cluster.labelgroup,cluster.confident);
		for(int j=0; j<cluster.coords.coms_length; j++)
		{
			total += fprintf(file,"|%f",cluster.coords.coms[j].value);
		}
		total += fprintf(file,"\n");
	}
	return total;
}
int MACRO_mobmx_vector_nodbts_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}