#ifndef SAMSON_KV_SETVECTOR_H
#define SAMSON_KV_SETVECTOR_H

/* ****************************************************************************
*
* FILE            KvSetVector.h - vector of KVSets
*
*
* Set of KVSets that can add and remove easily
*
*/
#include <sys/types.h>           /* size_t                                   */
#include <vector>                /* vector                                   */

#include "KVSet.h"               /* KVSet                                    */



namespace ss
{
	class KVSetVector : public std::vector<KVSet*>
	{
	public:
				
		KVFormat format;
		
		KVSetVector( KVFormat _format )
		{
			format = _format;
		}
	private:
		friend class KVQueue;
		
		KVSetVector(  )
		{
		}
		
		void setFormat( KVFormat _format)
		{
			format = _format;
		}
		
		
	public:		
		KVSetVector* duplicate( )
		{
			KVSetVector* tmp = new KVSetVector( format );
			tmp->insert( tmp->end() , begin() , end() ); 
			
			return tmp;
		}
		
		size_t getNumKVs()
		{
			size_t total = 0 ;
			for (size_t i = 0  ; i  < size() ; i++)
			{
				total += (*this)[i]->getNumKVs();
				//total += at(i)->num_kvs;
			}
			return total;
		}
	};
}


#endif
