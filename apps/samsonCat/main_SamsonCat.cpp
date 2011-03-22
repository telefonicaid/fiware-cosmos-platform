
#include <iostream>
#include "CommandLine.h"			// au::CommandLine
#include "coding.h"					// ss::FormatHeader
#include "samson/KVFormat.h"		// ss::KVFormat
#include "ModulesManager.h"			// ss::ModulesManager
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "SamsonSetup.h"			// ss::SamsonSetup

int logFd = -1;

//char* progName = (char*) "samsonCat";


int main(int argc, const char *argv[])
{
	au::CommandLine cmdLine;
	cmdLine.set_flag_boolean("header");
	cmdLine.set_flag_boolean("debug");
	cmdLine.set_flag_int("limit" , KVFILE_NUM_HASHGROUPS);
	cmdLine.set_flag_string("working",SAMSON_DEFAULT_WORKING_DIRECTORY);
	cmdLine.parse(argc , argv);

	ss::SamsonSetup::load( cmdLine.get_flag_string("working") );
	
	bool debug = cmdLine.get_flag_bool("debug");
	int limit = cmdLine.get_flag_int("limit");
	
	
	if( cmdLine.get_num_arguments() < 2 ) 
	{
		std::cerr << "Usage: samsonCat file [-header]\n";
		exit(0);
	}

	std::string fileName = cmdLine.get_argument(1);
	FILE *file = fopen( fileName.c_str() , "r" );
	
	if( !file )
	{
		std::cerr << "File not found " << cmdLine.get_argument(1) << "\n";
		exit(0);
	}
	
	ss::KVHeader header;
	int          nb;
	nb = fread(&header, 1, sizeof(ss::KVHeader), file);

	if (nb != sizeof(ss::KVHeader))
		printf("WARNING: read only %d bytes (wanted to read %ld)\n", nb, (long int) sizeof(ss::KVHeader));

		

	if (!header.check())
	{
		std::cout << "Error: Wrong magic number\n";
		exit(0);
	}
	
	
	// Check the lenght of the file
	struct stat filestatus;
	stat( fileName.c_str() , &filestatus );
	
	
	size_t expected_size =   (size_t)( sizeof(ss::KVHeader) + (sizeof(ss::KVInfo)*KVFILE_NUM_HASHGROUPS) + header.info.size ) ;
	if( (size_t)filestatus.st_size != expected_size)
	{
		std::cerr << "Worng file length\n";
		std::cerr << "Expected:";
		std::cerr << " Header: " << sizeof(ss::KVHeader);
		std::cerr << " + Info: " << (sizeof(ss::KVInfo)*KVFILE_NUM_HASHGROUPS);
		std::cerr << " + Data: " <<  header.info.size;
		std::cerr << " = " << expected_size << "\n";
		std::cerr << "File size: " << filestatus.st_size << " bytes\n";
	}
	else
	{
		if (debug)
			std::cerr << "File with rigth size according to header\n";
	}
		
		
	
	
	
	ss::KVFormat format = header.getFormat(); 
	if( cmdLine.get_flag_bool("header") )
		std::cout << "Format: " << format.str() << "\n";

	ss::ModulesManager modulesManager;

	ss::Data *keyData = modulesManager.getData(format.keyFormat);
	ss::Data *valueData = modulesManager.getData(format.valueFormat);
	
	if(!keyData )
	{
		std::cerr << "Data format " << format.keyFormat << " not supported\n";
		exit(0);
	}
	
	if(!valueData )
	{
		std::cerr << "Data format " << format.valueFormat << " not supported\n";
		exit(0);
	}
	
	ss::KVInfo*  info = (ss::KVInfo*) malloc(  sizeof(ss::KVInfo)*(KVFILE_NUM_HASHGROUPS));
	int          total_size =  sizeof(ss::KVInfo) * (KVFILE_NUM_HASHGROUPS);
	nb = fread(info, 1, total_size, file);
	if (nb != total_size)
		printf("WARNING: read only %d bytes (wanted to read %d)\n", nb, total_size);


	if (cmdLine.get_flag_bool("header"))
	{
		std::cout << "Total: " << header.info.str() << std::endl;
		
	}
	
	// Compute dispersion
	size_t max_size = 0;
	size_t average = 0;
	size_t squeare_average = 0;
	for ( int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++ )
	{
		size_t _size = info[i].size;
		if(  _size > max_size )
			max_size = _size;
		
		average += _size;
		squeare_average += _size*_size;	
	}
	average /= KVFILE_NUM_HASHGROUPS;
	squeare_average /= KVFILE_NUM_HASHGROUPS;
	
	double d = sqrt( (double) squeare_average - (double) average*average ) / (double) average;

	
	std::cout << "Max size per hash-group " << au::Format::string( max_size ) << "\n";
	std::cout << "Average size per hash-group " << au::Format::string( average ) << "\n" ; 
	std::cout << "Dispersion in size per hash-group " << d << "\n" ; 
	
	if( cmdLine.get_flag_bool("header") )
		exit(0);
	
	
	ss::DataInstance *key = keyData->getInstance();
	ss::DataInstance *value = valueData->getInstance();
	
	for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ;i++)
	{
		if( i == limit)
			break;
		
		if( debug )
		{
			std::cerr << "Reading hash-group " << i << " -> " << info[i].str() << "\n";
		}
		
		
		size_t size = info[i].size;
		size_t kvs = info[i].kvs; 

		if( size > 0)
		{
			char *data = (char*)malloc( size );

			int n = fread(data, size, 1, file);
			if ( n != 1)
			{
				std::cerr << "Not possible to read file. Wrong format\n";
				exit(0);
			}
			
			
			size_t offset = 0 ;
			for (size_t i = 0 ; i < kvs ; i++)
			{
				offset += key->parse(data+offset);
				offset += value->parse(data+offset);
				
				std::cout << key->str() << " " << value->str() << std::endl;
			}
			
			if( offset != size)
				LM_X(1 , ("Wrong file format"));
			
			free( data );
		}
		
		
	}
	
	fclose(file);
	
	
	
	
}
