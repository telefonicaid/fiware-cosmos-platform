
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_cluster_agg_bts_cluster
#define _H_SAMSON_mob2_cluster_agg_bts_cluster


#include <samson/module/samson.h>
#include <samson/modules/mob2/Bts.h>
#include <samson/modules/mob2/Cluster.h>
#include <samson/modules/mob2/Poi.h>
#include <samson/modules/mob2/TwoInt.h>
#include <samson/modules/system/UInt.h>



namespace samson{
namespace mob2{


class cluster_agg_bts_cluster : public samson::Reduce
{
	//Inputs
	Poi poi;
	Cluster btsclus;
	Bts btsinfo;
	//Output
	TwoInt node_bts; //num1: node    num2:bts

	//Configurable thresholds
	int mob_conf_max_comms_bts;
	double mob_conf_max_bts_area;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt mob2.Poi
	input: system.UInt mob2.Cluster
	output: mob2.TwoInt mob2.Poi

	extendedHelp: 		Aggregate to results info about bts cluster
	Input 0: POI with clientbts cluster info and client cluster info
	Input 1: Info about bts cluster

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
		mob_conf_max_comms_bts = 36000;
		mob_conf_max_bts_area = 4.0;
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			poi.parse(inputs[0].kvs[i]->value);
			for(uint64_t j=0; j<inputs[1].num_kvs; j++)
			{
				btsclus.parse(inputs[1].kvs[j]->value);
				node_bts.num1.value = poi.node.value;
				node_bts.num2.value = poi.bts.value;
				poi.labelbts.value = btsclus.label.value;
				poi.labelgroupbts.value = btsclus.labelgroup.value;
				poi.confidentbts.value = btsclus.confident.value;
				poi.distancebts.value = btsclus.distance.value;
				writer->emit(0, &node_bts,&poi);
			}
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif
