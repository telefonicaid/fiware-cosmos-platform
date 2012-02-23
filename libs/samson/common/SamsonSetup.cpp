#include <stdio.h>
#include <iostream>				// std::cerr
#include <iomanip>              // std::setw

#include <errno.h>

#include "logMsg/logMsg.h"				// LM_X

#include "au/string.h"             // au::Format
#include "au/CommandLine.h"		// au::CommandLine
#include "au/ErrorManager.h"           // au::ErrorManager

#include "status.h"                         // Status codes

#include "samsonVars.h"                     // HOME AND WORKING DIRECTORIES

#include "samson/common/samsonDirectories.h"	// SAMSON_SETUP_FILE
#include "samson/common/SamsonSetup.h"		// Own interface

namespace samson
{
    
    SAMSON_EXTERNAL_VARS;
/*
  extern char  samsonHome[1024];
  extern char  samsonWorking[1024]
*/
    
	Status createDirectory( std::string path )
	{
		if( mkdir(path.c_str()	, 0755) == -1 )
		{
			if( errno != EEXIST )
            {
                LM_W(("Error creating directory %s (%s)", path.c_str() , strerror(errno)  ));
                return Error;
            }
		}
        
        return OK;
	}
        
    
    Status createFullDirectory( std::string path )
    {
        if( path.length() == 0)
            return Error;
        
        std::vector<std::string> components;
        au::split( path , '/' , components );
        
        std::string accumulated_path;
        if( path[0] == '/' )
            accumulated_path += "/";

        
        for ( size_t i = 0 ; i < components.size() ; i++)
        {
            accumulated_path += components[i];
            Status s = createDirectory(accumulated_path);
            if( s != OK )
            {
                LM_W(("Error creating directory %s (%s)" , accumulated_path.c_str()  , status(s) ));
                return s;
            }
            
            accumulated_path += "/";
            
        }
        
        return OK;
    }
    
    std::string cannonical_path( std::string path )
    {
        while( (path.size() > 0) && (path[ path.size()-1 ] == '/' ))
            path.erase(path.size()-1);
        return path;
    }
    
    
#pragma mark SetupItemCollection
    
    SetupItemCollection::~SetupItemCollection()
    {
       items.clearMap();
    }

    void  SetupItemCollection::add( std::string _name , std::string _default_value , std::string _description , SamsonItemType type )
    {
        if( items.findInMap(_name) != NULL )
        {
            LM_W(("Item %s already added to setup... ignoring" , _name.c_str() ));
            return;
        }
        
        items.insertInMap( _name , new SetupItem( _name , _default_value , _description , type ) );
        
    }
    
    
    void SetupItemCollection::load( std::string fileName )
    {
		FILE *file = fopen( fileName.c_str()  ,"r");
		if (!file)
		{
            LM_W(("Warning: Setup file %s not found\n" , fileName.c_str() ));
		}
		else
		{
			
			char line[2000];
			while( fgets(line, sizeof(line), file))
			{
				au::CommandLine c;
				c.parse(line);
				
				if( c.get_num_arguments() == 0 )
					continue;
				
				// Skip comments
				std::string mainCommand = c.get_argument(0);
				if( mainCommand[0] == '#' )
					continue;
				
				if (c.get_num_arguments() >= 2)	
				{
					std::string name = c.get_argument(0);
					std::string value =  c.get_argument(1);
                    
                    SetupItem*item = items.findInMap(name);
                    
                    if( item )
                        item->setValue(value);
                    else
                        LM_W(("Unknown parameter %s found in setup file %s" , name.c_str() , fileName.c_str() ));
                    
				}
			}
            
			fclose(file);
		}        
    }

    std::string SetupItemCollection::getValueForParameter( std::string name )
    {
        SetupItem*item = items.findInMap(name);
        
        if( !item )
            LM_X(1, ("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
        
        return item->getValue();
        
    }
    
    bool SetupItemCollection::isParameterDefined( std::string name )
    {
        return ( items.findInMap(name) != NULL);
    }
    
    
    bool SetupItemCollection::setValueForParameter( std::string name ,std::string value )
    {
        SetupItem*item = items.findInMap(name);
        
        if( !item )
        {
            LM_W(("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
            return false;
        }
        
        return item->setValue( value );
    }
    
    void SetupItemCollection::resetToDefaultValues()
    {
        au::map< std::string , SetupItem >::iterator it_items;
        for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            it_items->second->resetToDefaultValue();
    }
    
    
    std::string SetupItemCollection::str()
    {
        std::ostringstream output;
        output << "Samson setup parameters:\n";
        output << "---------------------------------------------------------------------------------------------------\n";

        output << std::left;
        
        // Heder
        output << "\n";
        output << std::setw(40) << "Parameter" << " ";
        output << std::setw(20) << "Default value" << " ";
        output << std::setw(20) << "Value in setup.txt" << " ";
        output << std::setw(20) << "Help" << " ";
        output << "\n";
        
        output << "---------------------------------------------------------------------------------------------------\n";
        output << "\n";

        
        au::map< std::string , SetupItem >::iterator i;

        std::string previous_concept;
        
        for (i = items.begin() ; i != items.end() ; i++)
        {
            std::string name = i->first;  
            std::string concept = name.substr(0, name.find(".",0 ) );
            if( concept != previous_concept )
                output << "\n";
            previous_concept = concept;
            
            output << std::setw(40) << i->first << " ";
            output << std::setw(20) << i->second->getDefaultValue() << " ";
            output << std::setw(20) << i->second->getSetValue() << " ";
            output << std::setw(20) << i->second->getDescription() << " ";
            output << "\n";
        }
        
        return output.str();
    }
    
    
    
#pragma mark SamsonSetup
    
	static SamsonSetup *samsonSetup = NULL;
	
	SamsonSetup *SamsonSetup::shared()
	{
		if( !samsonSetup)
			LM_X(1,("Please, init SamsonSetup with SamsonSetup::init()"));
        
		return samsonSetup;
	}
	
    void SamsonSetup::destroy()
    {
        if( !samsonSetup )
            LM_X(1, ("SamsonSetup not initialized at destructor"));
        
        LM_V(("Destroying SamsonSetup"));
        delete samsonSetup;
        samsonSetup=NULL;
    }
                        
    
	void SamsonSetup::init( std::string samson_home , std::string samson_working )
	{
        if( samsonSetup )
        {
            LM_W(("Init SamsonSetup twice... ignoring"));
            return;
        }

        LM_V(("Init SamsonSetup"));
        
		samsonSetup = new SamsonSetup( samson_home , samson_working );
	}

	
	SamsonSetup::SamsonSetup( std::string samson_home , std::string samson_working )
	{
        // Init the setup system
        LM_TODO(("Add the possibility to set particular directories for this..."));
        
        char *env_samson_working = getenv("SAMSON_WORKING");
        char *env_samson_home = getenv("SAMSON_HOME");
        
        if ( samson_working == "" )
		{
            if( env_samson_working )
                samson_working = env_samson_working;
            else
                samson_working = SAMSON_WORKING_DEFAULT;
		}
    
        if ( samson_home == "" )
		{
            if( env_samson_home )
                samson_home = env_samson_home;
            else
                samson_home =  SAMSON_HOME_DEFAULT;
		}
        
        //Basic directories
        _samson_home = cannonical_path( samson_home );
        _samson_working = cannonical_path( samson_working );
        
        // General Platform parameters
        add( "general.memory" , "10000000000" , "Global available memory " , SetupItem_uint64 );                                          
        add( "general.num_processess" , "16" , "Number of cores" , SetupItem_uint64);                                                     

        add( "general.shared_memory_size_per_buffer" , "268435456" , "Size of the shared memory segments", SetupItem_uint64 );
        
        add( "general.update_status_period", "2" , "Period for the automatic update from workers to all delilah" , SetupItem_uint64);        

        // Isolation Process
		add( "isolated.timeout" , "300" , "Timeout for all 3rd partty operations" , SetupItem_uint64);

        // Worker
        add("worker.period_check_finish_tasks", "5", "Period to review finished tasks in samsonWorker, to be clean from memory", SetupItem_uint64 );        
        
        // Upload & Download operations
		add( "load.buffer_size" , "67108864" , "Size of the data block for load operations" , SetupItem_uint64);                         
        
        // Delilah Client
        add("delilah.automatic_update_period" , "2" , "Period for the automatic update of information from the samson cluster" , SetupItem_uint64);      
        
        // Stream processing
        add("stream.max_operation_input_size" , "400000000" , "Maximum input data ( in bytes ) to run an automatic stream processing task", SetupItem_uint64);
     
        // load setup file
        load( setupFileName() );
	}

    std::string SamsonSetup::setupFileName()
    {
        return _samson_working + "/etc/setup.txt";
    }
    
    std::string SamsonSetup::dataDirectory( )
    {
        return _samson_working + "/data";
    }
    
    std::string SamsonSetup::dataFile( std::string filename )
    {
        return _samson_working + "/data/" + filename;
    }
    
    std::string SamsonSetup::sharedMemoryLogFileName()
    {
        return _samson_working + "/log/shared_memory_ids.data";
    }
    
    std::string SamsonSetup::modulesDirectory()
    {
        return _samson_home + "/modules";
    }
    
    std::string SamsonSetup::blocksDirectory()
    {
        return _samson_working + "/blocks";
    }
    
    std::string SamsonSetup::blockFileName( size_t worker_id , size_t id )
    {
        return _samson_working + "/blocks/" + au::str("%lu_%lu", worker_id , id);
    }
    
    bool isNumber( std::string txt )
    {
        for (size_t i = 0 ; i < txt.length() ; i++ )
            if( !au::isCharInRange( txt[i] , 48 , 57 ) )
                return false;
        return  true;
    }
    
    bool SamsonSetup::blockIdFromFileName( std::string fileName , size_t *worker_id , size_t *block_id )
    {
        std::string path = _samson_working + "/blocks/";
        
        if( fileName.substr(0,path.size()) != path )
            return false;
        
        // Take the rest of the name
        std::string res_path = fileName.substr( path.size() );
        
        std::vector<std::string> components = au::split(res_path, '_');

        if( components.size() != 2 )
            return false;
        
        if( !isNumber( components[0] ) )
            return false;

        if( !isNumber( components[0] ) )
            return false;
        
        *worker_id = atoll( components[0].c_str() );
        *block_id  = atoll( components[1].c_str() );
        
        return true;
    }
    
    std::string SamsonSetup::streamManagerLogFileName()
    {
        return _samson_working + "/log/log_stream_state.txt";
    }
    
    std::string SamsonSetup::streamManagerAuxiliarLogFileName()
    {
        return _samson_working + "/log/log_stream_state.txt";
    }
    
    std::string SamsonSetup::clusterInformationFileName()
    {
        return _samson_working + "/log/log_cluster_information.txt";
    }
    
    std::string SamsonSetup::get( std::string name )
    {
        return getValueForParameter( name );
    }

    std::string SamsonSetup::get_default( std::string name )
    {
        SetupItem*item = items.findInMap(name);
        
        if( !item )
            return "";
        
        return item->getDefaultValue();
    }
    
    
    size_t SamsonSetup::getUInt64( std::string name )
    {
        std::string value = getValueForParameter( name );
        return atoll( value.c_str() );
    }

    int SamsonSetup::getInt( std::string name )
    {
        std::string value = getValueForParameter( name );
        return atoi( value.c_str() );
    }
    
	
    
    void SamsonSetup::createWorkingDirectories()
    {
        if( createFullDirectory( _samson_working ) != OK )
            LM_X(1,("Error creating directory %s" , _samson_working.c_str() ));
        if( createFullDirectory( _samson_working + "/data" )!= OK )
            LM_X(1,("Error creating directory %s" , _samson_working.c_str() ));
        if( createFullDirectory( _samson_working + "/log" )!= OK )
            LM_X(1,("Error creating directory %s" , _samson_working.c_str() ));
        if( createFullDirectory( _samson_working + "/blocks" )!= OK )
            LM_X(1,("Error creating directory %s" , _samson_working.c_str() ));
        if( createFullDirectory( _samson_working + "/etc" )!= OK )
            LM_X(1,("Error creating directory %s" , _samson_working.c_str() ));
        if( createFullDirectory( _samson_working + "/config" )!= OK )
            LM_X(1,("Error creating directory %s" , _samson_working.c_str() ));
    }
    
    void SamsonSetup::edit()
    {
        // Edit all the inputs
        
        // previous concept
        std::string concept = "Z";
        
        au::map< std::string , SetupItem >::iterator i;
        for ( i = items.begin() ; i != items.end() ; i++)
        {
            
            std::string tmp_concept = i->second->getConcept();
            
            if( tmp_concept != concept )
            {
                std::cout << "\n";
                std::cout << "----------------------------------------------------------------------------------------\n";
                std::cout << "Seting up parameters for " << tmp_concept << "\n";
                std::cout << "----------------------------------------------------------------------------------------\n";
                std::cout << "\n";
                concept = tmp_concept;
            }

            std::cout << "\n";
            std::cout << "Parameter       : " << i->first << "\n";
            std::cout << "Definition      : " << i->second->getDescription()<< "\n";
            std::cout << "Default value   : " << i->second->getDefaultValue()<< "\n";;
            std::cout << "SetupFile value : " << i->second->getSetValue()<< "\n";;
            std::cout << "Current value   : " << i->second->getValue()<< "\n";;
            std::cout << "\n";
            std::cout << "Press (enter) to use current value, (d) to use default value or a new value: ";
            char line[1024];
            char *l = fgets(line, 1024, stdin);

			if( l == NULL )
			   LM_X(1,("Invalid entry..."));
            // Remove the "\n";
            line[ strlen(line)-1 ] = '\0'; 
            
            if( strcmp(line, "") == 0 )
                std::cout << "Using current value " << i->second->getValue() << "\n";
            else if ( strcmp(line, "d") == 0 )
            {
                std::cout << "Using deafult value " << i->second->getDefaultValue() << "\n";
                i->second->setValue( i->second->getDefaultValue() );
            } else
            {
                std::string value = line;
                
                if( !i->second->check_valid( value ) )
                {
                    std::cout << "Value '%s' is not valid for " << i->first << "\n";
                    i--;
                }
                else
                {
                    std::cout << "Using new value " << line << "\n";
                    i->second->setValue( line );
                }
            }
        }
    }
    
    int SamsonSetup::save()
    {
        // Recover the name of setup file
        std::string setupFile = setupFileName();
        
        FILE *file = fopen( setupFile.c_str() , "w" );
        if( !file )
        {
            LM_W(("Impossible to open setup file %s" , setupFile.c_str() ));
            return 1;
        }

        fprintf(file, "# ----------------------------------------------------------------------------------------\n");
        fprintf(file, "# SAMSON SETUP\n");
        fprintf(file, "# ----------------------------------------------------------------------------------------\n");
        fprintf(file, "# File auto-generated with samsonConfig tool.\n");
        fprintf(file, "# You can edit manually or use samsonConfig to regenetare it\n\n\n");
        

        
        au::map< std::string , SetupItem >::iterator i;

        // First concept
        std::string concept = "Z";
        
        for ( i = items.begin() ; i != items.end() ; i++)
        {
            
            std::string tmp_concept = i->second->getConcept();
            
            if( tmp_concept != concept )
            {
                fprintf(file, "# ------------------------------------------------------------------------ \n" );
                fprintf(file, "# SECTION: %s\n" , tmp_concept.c_str());
                fprintf(file, "# ------------------------------------------------------------------------ \n\n" );
                concept = tmp_concept;
            }
            
            fprintf(file, "%-40s\t%-20s # %s\n" , i->first.c_str() , i->second->getValue().c_str() , i->second->getDescription().c_str()  );
        }
        
        fclose( file );
        
        return 0;
    }
    
    void SamsonSetup::clearCustumValues()
    {
        au::map< std::string , SetupItem >::iterator it_items;
        for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            it_items->second->clearCustumValue();
        
    }

    
    std::vector<std::string> SamsonSetup::getItemNames()
    {
        return items.getKeysVector();
    }

    
}
