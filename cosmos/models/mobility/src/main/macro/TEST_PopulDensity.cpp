#include "TEST_Module.h"

void MACRO_mobmx_popden_spread_nodebtsdayhour::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt cellId;
	MRData_CdrMx cdr;
	MRData_CellMx catalogue;
	// Outputs
	MRData_NodBtsDate nodbtsdate;
	MRData_Void mr_void;
	
	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		cellId.parse(inputs[1].kvs[i].key);
		catalogue.parse(inputs[1].kvs[i].value);
		
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			cdr.parse(inputs[0].kvs[j].value);
			nodbtsdate.node = cdr.phone;
			nodbtsdate.bts = catalogue.bts;
			nodbtsdate.date = cdr.date;
			nodbtsdate.hour = cdr.time.hour;
			writer[0]->emit(&nodbtsdate,&mr_void);
		}
	}
}

void MACRO_mobmx_popden_delete_duplicates::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_NodBtsDate nodbtsdate;
	// Outputs
	MRData_UInt node;
	MRData_Node_Bts nodbts;
	
	nodbtsdate.parse(inputs[0].kvs[0].key);
	node.value = nodbts.phone = nodbtsdate.node;
	nodbts.bts = nodbtsdate.bts;
	nodbts.wday = nodbtsdate.date.week_day;
	nodbts.range = nodbtsdate.hour;
	
	writer[0]->emit(&node,&nodbts);
}
// With profile
void MACRO_mobmx_popden_join_nodeinfo_profile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Node_Bts nodbts;
	MRData_UInt profileId;
	// Outputs
	MRData_BtsProfile btsprofile;
	MRData_UInt ncoms;
	
	for(int i=0; i<inputs[1].num_kvs; i++)
	{
		profileId.parse(inputs[1].kvs[i].value);
		for(int j=0; j<inputs[0].num_kvs; j++)
		{
			nodbts.parse(inputs[0].kvs[j].value);
			btsprofile.bts = nodbts.bts;
			btsprofile.profile = profileId.value;
			btsprofile.wday = nodbts.wday;
			btsprofile.hour = nodbts.range;
			ncoms.value = 1;
			writer[0]->emit(&btsprofile,&ncoms);
		}
	}
}
// Without profile
void MACRO_mobmx_popden_join_nodeinfo_withoutprofile::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_Node_Bts nodbts;
	// Outputs
	MRData_BtsProfile btsprofile;
	MRData_UInt ncoms;
	
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		nodbts.parse(inputs[0].kvs[i].value);
		btsprofile.bts = nodbts.bts;
		btsprofile.profile = 0;
		btsprofile.wday = nodbts.wday;
		btsprofile.hour = nodbts.range;
		ncoms.value = 1;
		writer[0]->emit(&btsprofile,&ncoms);
	}
}