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
        
        void resetToDefaultValue()
        {
            value_available =  false;
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
    
        // Check if a particular property if defined
        bool isParameterDefined( std::string name );
        
        // Come back to default parameters
        void resetToDefaultValues();
        
        std::string str();
        
    };
    
    
	class SamsonSetup : public SetupItemCollection
	{
		
		SamsonSetup( std::string samson_home , std::string samson_working );
        
        std::string _samson_home;           // Home directory for SAMSON system
        std::string _samson_working;        // Working directory for SAMSON system
		
	public:

		static void init( std::string samson_home , std::string samson_working );
		static void destroy( );
		static SamsonSetup *shared();
		
        
        // Get access to parameters
        std::string get( std::string name );
        size_t getUInt64( std::string name );
        int getInt( std::string name );

        // Get names fo files
        std::string setupFileName();                         // Get the Steup file
        std::string sharedMemoryLogFileName();
        std::string controllerLogFileName();
        std::string dataDirectory(  );     
        std::string dataFile( std::string filename );        // Get the name of a particular data file ( batch processing )
        std::string modulesDirectory();
        std::string blocksDirectory();
        std::string blockFileName( size_t id );
        std::string streamManagerLogFileName();
        std::string streamManagerAuxiliarLogFileName();
        
        
        // Create working directories
        void createWorkingDirectories();

	public:
		
		// Check if everything is ok. Exit if not
		void check( au::ErrorManager *error );

		
        void edit();
        int save();
        
	};

}

#endif
