#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <map>				// std::map
#include <string>			// std::string
#include <iostream>			// std::cout
#include <stdlib.h>         // atoll
#include <sys/stat.h>		// mkdir

#include "au/map.h"         // au::map
#include "au/Environment.h" // au::Environment

#define SAMSON_CONTROLLER_DEFAULT_PORT		1234
#define SAMSON_WORKER_DEFAULT_PORT			1235
#define SAMSON_DEFAULT_WORKING_DIRECTORY	"/opt/samson"

namespace au {
    class ErrorManager;
}

namespace samson {
	
    
    class SetupItem
    {
        std::string name;
        std::string default_value;

        std::string value;
        bool value_available;

        std::string description;
        
    public:
        
        SetupItem( std::string _name , std::string _default_value , std::string _description )
        {
            name = _name;
            default_value = _default_value;
            value_available =  false;
            
            description = _description;
        }

        void setValue( std::string _value )
        {

            value = _value;
            value_available = true;
        }
        
        std::string getValue()
        {
            if( value_available )
                return value;
            else
                return default_value;
        }

        std::string getDefaultValue()
        {
            return default_value;
        }
        
        std::string getSetValue()
        {
            if( value_available )
                return value;
            else
                return "";
        }
        
        std::string getDescription()
        {
            return description;
        }
        
        std::string getConcept()
        {
            return name.substr(0, name.find(".",0 ) );
        }
        
        
    };
    
    
    class SetupItemCollection
    {
        
    protected:
        
        au::map< std::string , SetupItem > items;
        
    public:
      
        // Add a new parameter to consider
        void  add( std::string _name , std::string _default_value , std::string _description );

        // Load a particular file to include all setup parameters
        void load( std::string file );
        
        // get the value of a parameter
        std::string getValueForParameter( std::string name );
    
        // Set manualy a particular parameter ( special case )
        void setValueForParameter( std::string name ,std::string value );
    
        std::string str();
        
    };
    
    
	class SamsonSetup : public SetupItemCollection
	{
		
		SamsonSetup( );
		
	public:

		static void init( );
		static SamsonSetup *shared();

        // Set the main working directory ( for worker, controller )
        void setWorkingDirectory( std::string workingDirectory );
        
		// Directories
		std::string baseDirectory;
		std::string controllerDirectory;
		std::string dataDirectory;
		std::string blocksDirectory;
		std::string modulesDirectory;
		std::string setupDirectory;
		std::string setupFile;
		std::string configDirectory;
        
        static std::string dataFile( std::string filename );
		
        
        // Get access to parameters
        
        static std::string get( std::string name );
        static size_t getUInt64( std::string name );
        static int getInt( std::string name );
        
	public:
		
		// Check if everything is ok. Exit if not
		void check( au::ErrorManager *error );

		
        void edit();
        void save();
        
	};

}

#endif
