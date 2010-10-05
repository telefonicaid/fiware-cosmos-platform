#ifndef SAMSON_KV_QUEUES_MANAGER_H
#define SAMSON_KV_QUEUES_MANAGER_H

#include <map>
#include <string>



namespace ss
{
	
	class KVQueue;
	class KVFormat;
	class KVSet;
	class KVManagerLog;

	class KVQueuesManager
	{
		
		
		std::map<std::string , KVQueue*> queues;	//!< List of queues
		
	public:
		
		void newKVQueue( std::string name , KVFormat format );
		void removeKVQueue( std::string name );
		void addKVSetToQueue( KVSet *set , KVQueue *queue , int hash);
		void addKVSetToKVQueue( KVSet *set , std::string queue_name , int hash );			
		KVQueue *findQueue( std::string& queue_name  );
		std::string str();
		
		KVManagerLog * getFullLog();
		
	};
	
	
}

#endif
