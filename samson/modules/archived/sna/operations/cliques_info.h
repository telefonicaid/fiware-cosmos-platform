/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_cliques_info
#define _H_SAMSON_sna_cliques_info


#include <samson/module/samson.h>
#include "AUIntStatistics.h"
#include <iostream>
#include <sstream>


namespace samson{
namespace sna{


	class cliques_info : public samson::ParserOut
	{
		AUIntStatistics total;
		AUIntStatistics sizeClique;



	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			samson::sna::Clique clique;


			for (size_t i  =0 ;  i< inputs[0].num_kvs ; i++)
			{
				clique.parse( inputs[0].kvs[i]->key);

				total.add(1);
				sizeClique.add(clique.nodes_length);
			}
		}

#ifdef RUNFINISH

		void runFinish(TXTWriter *writer)
		{
			std::ostringstream output;

			output.str(""); //Clear content
			output << "\n\nDistribution of sizes of the cliques:\n";
			output << "-----------------------------------------\n";
			sizeClique.printDistribution(output);
			output << "-----------------------------------------\n";

			output << "General information:\n";
			output << "-------------------------\n";
			output << "Size of the clique: mean " << sizeClique.mean << " std-dev:" << sqrt( sizeClique.var ) << "\n";
			double average_connections = (double) ( total.count - sizeClique.count) / (double) sizeClique.count;
			output << "Average number of connections: " << average_connections << std::endl;

			writer->emit(output.str());
		}
#endif /* de RUNFINISH */


	};


} // end of namespace samson
} // end of namespace sna

#endif
