#include "TEST_Module.h"

// WITH PROFILE
void MACRO_mobmx_popden_join_array_profile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_NodeMx_Counter counter;
	MRData_UInt profile;
	// Outputs
	MRData_BtsProfile bts_profile;
	MRData_UInt ncoms;
	
	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		profile.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			counter.parse(inputs[0].kvs[j].value);
			bts_profile.profile = profile.value;
			for(int k=0; k<counter.bts_length; k++)
			{
				bts_profile.bts = counter.bts[k].bts;
				bts_profile.wday = counter.bts[k].wday;
				bts_profile.hour = counter.bts[k].range;
				ncoms.value = counter.bts[k].count;
				writer[0]->emit(&bts_profile,&ncoms);
			}
		}
	}
}
// WITHOUT PROFILE
void MACRO_mobmx_popden_spread_array::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_NodeMx_Counter counter;
	// Outputs
	MRData_BtsProfile bts_profile;
	MRData_UInt ncoms;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		counter.parse(inputs[0].kvs[i].value);
		for(int j=0; j<counter.bts_length; j++)
		{
			bts_profile.bts = counter.bts[j].bts;
			bts_profile.profile = 0;	// No profile
			bts_profile.wday = counter.bts[j].wday;
			bts_profile.hour = counter.bts[j].range;
			ncoms.value = counter.bts[j].count;
			writer[0]->emit(&bts_profile,&ncoms);
		}
	}
}
// COMMON FUNCTIONALITY
void MACRO_mobmx_popden_sum_comms::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_BtsProfile bts_wdayhour;
	MRData_UInt ncoms;
	// Outputs
	MRData_TwoInt bts_profile;
	MRData_Bts_Counter counter;
	
	int ncoms_total = 0;
	
	bts_wdayhour.parse(inputs[0].kvs[0].key);
	bts_profile.num1 = bts_wdayhour.bts;
	bts_profile.num2 = bts_wdayhour.profile;
	counter.bts = 0;
	counter.wday = bts_wdayhour.wday;
	counter.range = bts_wdayhour.hour;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		ncoms.parse(inputs[0].kvs[i].value);
		ncoms_total += ncoms.value;
	}
	counter.count = ncoms_total;
	writer[0]->emit(&bts_profile,&counter);
}

void MACRO_mobmx_popden_create_vector::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_TwoInt bts_profile;
	MRData_Bts_Counter counter;
	// Outputs
	MRData_ClusterVector comsVector;
	
	MRData_Double zero_double;
	int pos;
	double norm;
	
	bts_profile.parse(inputs[0].kvs[0].key);
	
	// Initialize the vector
	comsVector.comsSetLength(0);
	zero_double.value = 0.0;
	for(int i=0; i<168; i++)
	{
		comsVector.comsAdd()->copyFrom(&zero_double);
	}
	// Fill in with values
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		counter.parse(inputs[0].kvs[i].value);
		int wday = counter.wday;
		pos = wday > 0 ? wday-1 : 6;	// Sunday: 0
		pos *= 24;
		pos += counter.range;
		
		// Normalization of values
		// TODO: Meter nº de días por configuración
		if(counter.wday == 4)
		{
			norm = (double)counter.count/25.0; 
		}
		else
		{
			norm = (double)counter.count/26.0; 
		}
		
		comsVector.coms[pos].value = norm;
	}
	writer[0]->emit(&bts_profile,&comsVector);
}

// OUTPUT
int MACRO_mobmx_popden_get_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_TwoInt bts_0;
	MRData_ClusterVector comms_vector;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		bts_0.parse(kvSet.kvs[i].key);
		comms_vector.parse(kvSet.kvs[i].value);
		// BTS|C0|...|C167
		total += fprintf(file,"%lu", bts_0.num1);
		for(int j=0; j<comms_vector.coms_length; j++)
		{
			total += fprintf(file,"|%.4f",comms_vector.coms[j].value);
		}
		total += fprintf(file,"\n");
	}
	
	return total;
}
int MACRO_mobmx_popden_get_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}
int MACRO_mobmx_popden_profile_get_out::parseOut( KVSet kvSet , FILE *file)
{
	//Inputs
	MRData_TwoInt bts_profile;
	MRData_ClusterVector comms_vector;

	size_t total = 0;

	for(int i=0; i<kvSet.num_kvs; i++)
	{
		bts_profile.parse(kvSet.kvs[i].key);
		comms_vector.parse(kvSet.kvs[i].value);
		// BTS|PROFILE|C0|...|C167
		total += fprintf(file,"%lu|%d", bts_profile.num1,bts_profile.num2);
		for(int j=0; j<comms_vector.coms_length; j++)
		{
			total += fprintf(file,"|%.4f",comms_vector.coms[j].value);
		}
		total += fprintf(file,"\n");
	}
	
	return total;
}
int MACRO_mobmx_popden_profile_get_out::parseOutFinish( FILE *file)
{
	return fprintf(file, "");
}