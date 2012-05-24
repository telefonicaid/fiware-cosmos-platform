#include "TEST_Module.h"


////////////////////////////////////////
//////////// NORMALIZATION /////////////
////////////////////////////////////////
void MACRO_mobmx_vector_normalized::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_Node_Bts nodbts;
	MRData_ClusterVector vector_in;
	//Outputs
	MRData_ClusterVector vector_norm;
	//Intermediate
	MRData_ClusterVector div;
	MRData_Double elem;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodbts.parse(inputs[0].kvs[i].key);
		vector_in.parse(inputs[0].kvs[i].value);
		
		///// Initialization of values /////
		double sumvalues = 0;
		div.comsSetLength(0);
		vector_norm.comsSetLength(0);

		///// Div on total number of days of the group /////
		for(int j=0; j<vector_in.coms_length; j++)
		{
			if(j < 24) // Mondays, Tuesday, Wednesday and Thursday --> Total: 103 days
			{	
				elem.value = vector_in.coms[j].value / 103.0;
				sumvalues += elem.value;
			}
			else  // Friday, Saturdays or Sundays --> 26
			{
				elem.value = vector_in.coms[j].value / 26.0;
				sumvalues += elem.value;
			}
			div.comsAdd()->copyFrom(&elem);
		}

		///// Normalization of vector /////
		for(int j=0; j<div.coms_length; j++)
		{
			elem.value = div.coms[j].value / sumvalues;
			vector_norm.comsAdd()->copyFrom(&elem);
		}
		writer[0]->emit(&nodbts,&vector_norm);
	}
}
void MACRO_mobmx_poi_normalize_poi_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt node_poi;
	MRData_Cluster cluster;
	// Outputs
	MRData_Cluster cluster_norm;
	// Intermediate
	MRData_Cluster cluster_sum;
	MRData_Cluster cluster_div;
	MRData_Double coms;
	double sum_values;
	
	if(inputs[0].num_kvs == 0)
	{
		return;
	}
	node_poi.parse(inputs[0].kvs[0].key);

	cluster.parse(inputs[0].kvs[0].value);
	cluster_sum.copyFrom(&cluster);
	
	// Sum of vectors of adjacent pois
	for(int i=1; i<inputs[0].num_kvs; i++) // There are adjacent pois
	{
		cluster.parse(inputs[0].kvs[i].value);
		for(int j=0; j<cluster.coords.coms_length; j++)
		{
			cluster_sum.coords.coms[j].value += cluster.coords.coms[j].value;
		}
	}
	// Div on total number of days of the group
	cluster_div.copyFrom(&cluster_sum);
	cluster_div.coords.comsSetLength(0);
	sum_values = 0;
	for(int i=0; i<cluster_sum.coords.coms_length; i++)
	{
		if(i < 24) // Mondays, Tuesday, Wednesday and Thursday --> Total: 103 days
		{
			coms.value = cluster_sum.coords.coms[i].value/103.0;
		}
		else
		{
			coms.value = cluster_sum.coords.coms[i].value/26.0;
		}
		sum_values += coms.value;
		cluster_div.coords.comsAdd()->copyFrom(&coms);
	}
	///// Normalization of vector /////
	cluster_norm.copyFrom(&cluster_div);
	cluster_norm.coords.comsSetLength(0);
	for(int i=0; i<cluster_div.coords.coms_length; i++)
	{
		coms.value = cluster_div.coords.coms[i].value/sum_values;
		cluster_norm.coords.comsAdd()->copyFrom(&coms);
	}
	writer[0]->emit(&node_poi,&cluster_norm);
}