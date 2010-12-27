

#include "AutomaticOperationManager.h"	// own interface 
#include "AutomaticOperation.h"			// ss::AutomaticOperation


namespace ss
{
	
	AutomaticOperationManager::AutomaticOperationManager()
	{
		counter = 0;
	}
	
	void AutomaticOperationManager::add( AutomaticOperation* ao )
	{
		size_t id = counter;
		operations.insertInMap( id , ao );
		counter++;
	}
	
	void AutomaticOperationManager::remove( size_t id )
	{
		AutomaticOperation *tmp = operations.extractFromMap( id );
		if( tmp )
			delete tmp;
	}
	
	void AutomaticOperationManager::removeAllWithTag( std::string tag )
	{
		std::vector<size_t> ids_to_be_removed;
		
		au::map<size_t , AutomaticOperation >::iterator iter;
		for ( iter = operations.begin() ; iter != operations.end() ; iter++)
			if( iter->second->checkTag(tag) )
				ids_to_be_removed.push_back( iter->first);
		

		for ( size_t i = 0 ; i < ids_to_be_removed.size() ; i++)
		{
			AutomaticOperation *ao = operations.extractFromMap( ids_to_be_removed[i] );
			if( ao )
				delete ao;
		}
		
	}

	
	std::vector<AOInfo> AutomaticOperationManager::getNextAutomaticOperations()
	{
		std::vector<AOInfo>	info;
		
		// Loop to the queues to see pending operations
		au::map<size_t , AutomaticOperation >::iterator iter;
		for ( iter = operations.begin() ; iter != operations.end() ; iter++)
		{
			if( iter->second->thrigger() )
			{
				AOInfo _info;
				
				_info.id = iter->first;					// Get a new id for this job
				_info.command = iter->second->run( );	// Get the command to run and put in "running" mode
				
				info.push_back( _info );
			}
		}
		
		return info;
	}
	
	void AutomaticOperationManager::finishAutomaticOperation( size_t id , bool error , std::string error_message  )
	{
		AutomaticOperation *ao = operations.findInMap( id );
		if( ao )
		{
			ao->finish( error , error_message );
		}
	}

	
	void AutomaticOperationManager::fill( network::AutomaticOperationList *aol , std::string command)
	{
		au::map<size_t , AutomaticOperation >::iterator iter;
		for ( iter = operations.begin() ; iter != operations.end() ; iter++)
		{
			network::AutomaticOperation *ao = aol->add_automatic_operation();
			ao->set_id( iter->first );
			iter->second->fill( ao );
		}
		
		
	}
	
	
}