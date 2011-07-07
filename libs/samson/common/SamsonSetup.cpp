#include <stdio.h>
#include <iostream>				// std::cerr
#include <iomanip>              // std::setw

#include <errno.h>

#include "logMsg/logMsg.h"				// LM_X

#include "au/Format.h"             // au::Format
#include "au/CommandLine.h"		// au::CommandLine
#include "au/ErrorManager.h"           // au::ErrorManager

#include "samson/common/samsonDirectories.h"	// SAMSON_SETUP_FILE
#include "samson/common/SamsonSetup.h"		// Own interface

namespace samson
{
    
	void createDirectory( std::string path )
	{
		if( mkdir(path.c_str()	, 0755) == -1 )
		{
			if( errno != EEXIST )
				LM_X(1,("Error creating directory %s", path.c_str()));
		}
	}
    
    
#pragma mark SetupItemCollection
    
    void  SetupItemCollection::add( std::string _name , std::string _default_value , std::string _description )
    {
        if( items.findInMap(_name) != NULL )
        {
            LM_W(("Item %s already added to setup... ignoring" , _name.c_str() ));
            return;
        }
        
        items.insertInMap( _name , new SetupItem( _name , _default_value , _description) );
        
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
                        LM_W(("Parameter %s found in setup file %s not included since it is not defined." , name.c_str() , fileName.c_str() ));
                    
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
    
    void SetupItemCollection::setValueForParameter( std::string name ,std::string value )
    {
        SetupItem*item = items.findInMap(name);
        
        if( !item )
            LM_X(1, ("Parameter %s not defined in the setup. This is not acceptable", name.c_str()));
        
        item->setValue( value );
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
        output << std::setw(20) << "Value at setup.txt" << " ";
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
	
    void destroy_samson_setup()
    {
        if( samsonSetup )
        {
            delete samsonSetup;
            samsonSetup = NULL;
        }
        
    }
    
	void SamsonSetup::init()
	{
        if( samsonSetup )
        {
            LM_W(("Init SamsonSetup twice... ignoring"));
            return;
        }
        
		samsonSetup = new SamsonSetup( );
        atexit(destroy_samson_setup);
        
	}
	
    void SamsonSetup::setWorkingDirectory( std::string workingDirectory )
	{

		baseDirectory = workingDirectory;

		controllerDirectory = workingDirectory + "/controller";
		dataDirectory		= workingDirectory + "/data";
        blocksDirectory     = workingDirectory + "/blocks";
		modulesDirectory	= workingDirectory + "/modules";
		setupDirectory		= workingDirectory + "/etc";
		setupFile			= setupDirectory   + "/setup.txt";
		configDirectory			= workingDirectory + "/config";
				
		// Create directories if necessary
		createDirectory( workingDirectory );
		createDirectory( controllerDirectory );
		createDirectory( dataDirectory );
		createDirectory( blocksDirectory );
        createDirectory( modulesDirectory );        
		createDirectory( setupDirectory );			
		createDirectory( configDirectory );			
		
		// Load values from file ( if exist )
		load( setupFile );
        
        // Check everything looks ok
        au::ErrorManager errorManager;
        check(&errorManager);
        
        if( errorManager.isActivated() ) 
        {
            LM_X(1, ("Error checking setup: %s" , errorManager.getMessage().c_str() ));
        }
	}
	
	SamsonSetup::SamsonSetup( )
	{
        
        add( "general.memory" , "2147483648" , "Global available memory " );                                    // Memory 2G
        add( "general.num_processess" , "2" , "Number of cores" );                                              // Number of cores 2
        add( "general.max_file_size" , "104857600" , "Max size for generated files" );                          // Max file size 100 Mb
        add( "general.shared_memory_size_per_buffer" , "67108864" , "Size of the shared memory segments" );            // Shared memory suze 64Mb

		add( "general.max_parallel_outputs" , "2" , "Max number of parallel outputs");
        
		add( "isolated.timeout" , "300" , "Timeout for all 3rd partty operations" );                            // Max time isolated

		add( "load.buffer_size" , "67108864" , "Size of the data block for load operations" );                  // Load in blocks of 64 Mbytes
        
        add("delilah.automatic_update_period" , "2" , "Period for the automatic update of information from the samson cluster" );                          // Seconds to update delilah
        
        
        add("worker.update_files_period" ,"5" , "Period for the automatic update of files for each worker" );
        add("worker.update_status_period", "3" , "Period for the automatic update from workers to controller" );        
        
        
        add("controller.max_worker_disconnected_time", "120" , "Maximum acceptable time for a worker to be disconnected. All task will be killed is larger disconnected-time is observed" );
	}

    std::string SamsonSetup::get( std::string name )
    {
        if( !samsonSetup )
            LM_X(1, ("Please init SamsonSetup with SamsonSetup::init()"));

        return samsonSetup->getValueForParameter( name );
    }
    
    size_t SamsonSetup::getUInt64( std::string name )
    {
        if( !samsonSetup )
            LM_X(1, ("Please init SamsonSetup with SamsonSetup::init()"));
     
        std::string value = samsonSetup->getValueForParameter( name );
        
        return atoll( value.c_str() );
    }

    int SamsonSetup::getInt( std::string name )
    {
        if( !samsonSetup )
            LM_X(1, ("Please init SamsonSetup with SamsonSetup::init()"));
        
        std::string value = samsonSetup->getValueForParameter( name );
        
        return atoi( value.c_str() );
    }
    
	
    
    void SamsonSetup::check( au::ErrorManager *error )
	{
        
        size_t memory = getUInt64("general.memory");
        int num_processes = getInt("general.num_processess");
        size_t shared_memory_size_per_buffer = getUInt64("general.shared_memory_size_per_buffer");
        size_t max_file_size = getUInt64("general.max_file_size");
        
        int num_paralell_outputs = getInt("general.max_parallel_outputs");
        
		int max_num_paralell_outputs =  ( memory - num_processes*shared_memory_size_per_buffer ) / ( 2 * max_file_size);
		if( num_paralell_outputs > max_num_paralell_outputs )
		{
            char line[1024];
            sprintf(line, "Num of maximum paralell outputs is too high to the current memory setup. Review num_paralell_outputs in setup.txt file. Current value %d Max value %d (memory(%lu) - num_processes(%d)*shared_memory_size_per_buffer(%lu) ) / (2*max_file_size(%lu))", num_paralell_outputs , max_num_paralell_outputs, memory, num_processes, shared_memory_size_per_buffer, max_file_size  );
            
            error->set( line );
		}
		
		if ( num_paralell_outputs < 2 )
            error->set(  au::Format::string("Num  of paralell outputs is lower than 2. Please review property 'general.max_parallel_outputs'" ) );
        
	}

    std::string SamsonSetup::dataFile( std::string filename )
    {
        return samsonSetup->dataDirectory + "/" + filename;
    }
	
    
    void SamsonSetup::edit()
    {
        // Edit all the inputs
        
        // First concept
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
            fgets(line, 1024, stdin);
            
            // Remove the "\n";
            line[ strlen(line)-1 ] = '\0'; 
            
            if( strcmp(line, "") == 0 )
            {
                std::cout << "Using current value " << i->second->getValue() << "\n";
            } 
            else if ( strcmp(line, "d") == 0 )
            {
                std::cout << "Using deafult value " << i->second->getDefaultValue() << "\n";
                i->second->setValue( i->second->getDefaultValue() );
            } else
            {
                std::cout << "Using new value " << line << "\n";
                i->second->setValue( line );
            }
            
        }
        
    }
    
    void SamsonSetup::save()
    {
        FILE *file = fopen( setupFile.c_str() , "w" );
        if( !file )
        {
            LM_W(("Impossible to open setup file %s" , setupFile.c_str() ));
            return;
        }

        fprintf(file, "----------------------------------------------------------------------------------------\n");
        fprintf(file, "SAMSON SETUP\n");
        fprintf(file, "----------------------------------------------------------------------------------------\n");
        fprintf(file, "File auto-generated with samsonConfig tool.\n");
        fprintf(file, "You can edit manually or use samsonConfig to regenetare it\n\n\n");
        

        
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
    }
    
    
}
