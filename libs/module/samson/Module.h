#ifndef SAMSON_MODULE_H
#define SAMSON_MODULE_H

#include <string>
#include <map>

#include <samson/Operation.h>           /* Operation                                */
#include <samson/Data.h>                /* Data                                     */



namespace ss
{	
	class Module 
	{
	public:
		
		std::string name;		// Name of this container
		std::string version;	// Version of this module
		std::string author;		// Author of this module (mail included to report bugs)

		std::map<std::string, Operation*>  operations;	// Vector containing operations (map, generate, reduce)
		std::map<std::string, Data*>      datas;		// Vector containing data			
		
		friend class ModulesManager;
			
		void *hndl;	// Pointer for the dlOpen

		Module()
		{
			hndl    = NULL;
		}
		
		Module(std::string _name, std::string _version, std::string _author)
		{
			name    = _name;
			version = _version;
			author  = _author;
			hndl    = NULL;
		}
		
		~Module()
		{
			clearModule();
		}
		
	public:		
		
		Operation* getOperation( std::string name )
		{
			std::map<std::string , Operation*>::iterator i;
			i = operations.find( name );
			if( i == operations.end() )
				return NULL;
			else
				return i->second;
		}
		
		Operation::Type getOperationType( std::string name )
		{
			Operation * o =  getOperation(name);
			if( !o )
				return Operation::unknown;
			return o->getType();
		}

		/**
		 Function to get a new instance
		 */
		
		Data* getData( std::string name )
		{
			std::map<std::string , Data*>::iterator i = datas.find( name );
			if ( i == datas.end() )
				return NULL;
			else
				return i->second;
		}		
		
		bool checkData( std::string name )
		{
			if( name == "txt" )
				return true;	// Spetial case
			
			std::map<std::string , Data*>::iterator i = datas.find( name );
			if ( i == datas.end() )
				return false;
			else
				return true;
		}
		
	public:
		
		/** 
		 Add operation to this module
		 */
		
		void add( Operation *operation )
		{
            Operation * previous_operation  = extractOperation( operation->getName() );
            if ( previous_operation )
                delete previous_operation;
            
			operations.insert( std::pair<std::string , Operation*>( operation->getName() , operation) );
		}
		
		void add( Data* data )
		{
            Data *previous_data = extractData( data->getName() );
            if ( previous_data )
                delete previous_data;
            
			datas.insert( std::pair<std::string , Data*> ( data->getName() , data ) );
		}

        /*
         Remove operations and datas
         */
        
		Operation* extractOperation( std::string name )
		{
			std::map<std::string , Operation*>::iterator iter = operations.find(name);
			
			if( iter == operations.end() )
				return NULL;
			else
			{
				Operation* operation = iter->second;
				operations.erase(iter);
				return operation;
			}
			
		}		
        
		Data* extractData( std::string name )
		{
			std::map<std::string , Data*>::iterator iter = datas.find(name);
			
			if( iter == datas.end() )
				return NULL;
			else
			{
				Data* data = iter->second;
				datas.erase(iter);
				return data;
			}
			
		}		
        
        /*
         Mode operation between modulues
         */
        
		void moveFrom( Module *m )
		{
			datas.insert( m->datas.begin() ,  m->datas.end() );
			operations.insert( m->operations.begin() , m->operations.end());

			// We clear here since otherwise will be deleted twice at the destructor
            // Note that we are not removing objects
			m->datas.clear();
			m->operations.clear();
			
		}
		
		void clearModule()
		{
			
			// Remove all operations and datas
			
			for ( std::map<std::string, Operation*>::iterator o = operations.begin() ; o != operations.end() ; o++)
				delete o->second;
			
			for ( std::map<std::string, Data*>::iterator d = datas.begin() ; d != datas.end() ; d++)
				delete d->second;
			
			datas.clear();
			operations.clear();
		}
		
		
	};
}

#endif
