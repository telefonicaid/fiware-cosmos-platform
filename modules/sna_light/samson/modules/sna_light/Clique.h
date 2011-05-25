
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_sna_light_Clique
#define _H_SAMSON_sna_light_Clique


#include <samson/modules/sna_light/Clique_base.h>


namespace samson{
namespace sna_light{


	class Clique : public Clique_base
	{

	public:
		uint fistElementInCommonWithOtherClique(  Clique *clique )
		{
			int pos1 = 0;
			int pos2 = 0;

			while ( (pos1 < nodes_length ) && (pos2 < clique->nodes_length ) )
			{
				if (nodes[pos1] == clique->nodes[pos2])
					return nodes[pos1].value;

				if (nodes[pos1].value < clique->nodes[pos2].value)
					pos1++;
				else
					pos2++;

			}

			return -1;

		}

		bool containtsNode( uint id )
		{
			//Assumed clique is sorted
			for (int i = 0; i < nodes_length ; i++)
			{
				if( nodes[i] == id)
					return true;
				if( nodes[i] > id)
					return false;
			}

			return false;

		}

		bool isEqual( Clique * clique)
		{
			if( nodes_length != clique->nodes_length)
				return false;

			for (int i = 0  ; i < nodes_length ; i++)
				if( nodes[i] != clique->nodes[i])
					return false;

			return true;
		}

#ifdef PROBLEMAS
		int compare( Clique * clique)
		{
			if( nodes_length < clique->nodes_length)
				return -1;
			if( nodes_length > clique->nodes_length)
				return 1;

			//Same length
			for (int i = 0 ;  i < nodes_length ; i++)
			{
				if( nodes[i] < clique->nodes[i] )
					return -1;
				if( nodes[i] > clique->nodes[i] )
					return 1;

			}

			return 0;
		}
#endif

		int cliqueCompare( Clique * clique)
		{
			if( nodes_length < clique->nodes_length)
				return -1;
			if( nodes_length > clique->nodes_length)
				return 1;

			//Same length
			for (int i = 0 ;  i < nodes_length ; i++)
			{
				if( nodes[i] < clique->nodes[i] )
					return -1;
				if( nodes[i] > clique->nodes[i] )
					return 1;

			}

			return 0;
		}

		bool isContained(Clique *longest )
		{
			if (longest->nodes_length <= nodes_length)
				return false;

			int pos_shortest = 0 ;
			int pos_longest = 0;

			while( ( pos_shortest < nodes_length ) && ( pos_longest < longest->nodes_length ) )
			{
				if ( nodes[pos_shortest].value < longest->nodes[pos_longest].value )
					return false;	//Not included because this element is not present in the longest

				if ( nodes[pos_shortest].value > longest->nodes[pos_longest].value )
					pos_longest++;
				else
				{
					//Same element
					pos_longest++;
					pos_shortest++;
				}
			}

			// If all the elements of the shortest are considered, it is included in the large clique
			if( pos_shortest == nodes_length )
				return true;
			else
				return false;
		}


		int numberOfNodesInCommon( Clique *clique )
		{
			int pos_clique1 = 0;
			int pos_clique2 = 0;

			int common_elements = 0;

			while( ( pos_clique1 < nodes_length ) && ( pos_clique2 < clique->nodes_length ) )
			{
				if ( nodes[pos_clique1].value < clique->nodes[pos_clique2].value )
					pos_clique1++;
				else if ( nodes[pos_clique1].value > clique->nodes[pos_clique2].value )
					pos_clique2++;
				else
				{
					//Same element
					common_elements++;

					pos_clique1++;
					pos_clique2++;
				}
			}

			return common_elements;

		}

		double factorOFNodesInCommon( Clique *clique)
		{
			int pos_clique1 = 0;
			int pos_clique2 = 0;

			int common_elements = 0;

			while( ( pos_clique1 < nodes_length ) && ( pos_clique2 < clique->nodes_length ) )
			{
				if ( nodes[pos_clique1].value < clique->nodes[pos_clique2].value )
					pos_clique1++;
				else if ( nodes[pos_clique1].value > clique->nodes[pos_clique2].value )
					pos_clique2++;
				else
				{
					//Same element
					common_elements++;

					pos_clique1++;
					pos_clique2++;
				}
			}

			return (double) common_elements / (double) std::min( nodes_length , clique->nodes_length );

		}

#ifdef nodesAdd
		void nodesAdd(samson::system::UInt _value){
			nodesSetLength( nodes_length + 1 );
			nodes[nodes_length-1] = _value;
		}
#endif /* de nodesAdd */

	};


} // end of namespace samson
} // end of namespace sna_light

#endif
