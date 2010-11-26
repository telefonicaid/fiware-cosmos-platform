
#include <iostream>
#include "CommandLine.h"			// au::CommandLine
#include "coding.h"					// ss::FormatHeader
#include "samson/KVFormat.h"		// ss::KVFormat
#include "ModulesManager.h"			// ss::ModulesManager


int logFd = -1;

char* progName = (char*) "samsonCat";


int main(int argc, const char *argv[])
{

	
	au::CommandLine cmdLine;
	cmdLine.set_flag_boolean("header");
	cmdLine.parse(argc , argv);
	
	
	if( cmdLine.get_num_arguments() < 2 ) 
	{
		std::cerr << "Usage: samsonCat file [-header]\n";
		exit(0);
	}

	FILE *file = fopen( cmdLine.get_argument(1).c_str() , "r" );
	
	if( !file )
	{
		std::cerr << "File not found " << cmdLine.get_argument(1) << "\n";
		exit(0);
	}
	
	ss::FileHeader header;
	fread(&header, 1, sizeof(ss::FileHeader), file);

	if( !header.check() )
	{
		std::cout << "Error: Wrong magic number\n";
		exit(0);
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
	
	ss::FileKVInfo *info = (ss::FileKVInfo*) malloc(  sizeof(ss::FileKVInfo)*(KV_NUM_HASHGROUPS+1)  );
	fread(info, 1, sizeof(ss::FileKVInfo)*(KV_NUM_HASHGROUPS+1) , file);


	if( cmdLine.get_flag_bool("header") )
		std::cout << "Total: " << info->str() << std::endl;
	
	char *data = (char*)malloc( info->size );

	int n = fread(data, info->size, 1, file);
	if ( n != 1)
	{
		std::cerr << "Not possible to read file\n";
		exit(0);
	}
	
	if( cmdLine.get_flag_bool("header") )
		exit(0);

	
	ss::DataInstance *key = keyData->getInstance();
	ss::DataInstance *value = valueData->getInstance();
	
	size_t offset = 0 ;
	for (size_t i = 0 ; i < info->kvs ; i++)
	{
		offset += key->parse(data+offset);
		offset += value->parse(data+offset);
		
		std::cout << "Key " << i << " " << key->str() << " " << value->str() << std::endl;
		
	}
	
	
	
	
}
