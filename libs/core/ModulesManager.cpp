/* ****************************************************************************
*
* FILE                 ModulesManager.cpp - 
*
*/
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <iomanip>
#include <ios>
#include <string>

#include <samson/samsonVersion.h>   /* SAMSON_VERSION                           */
#include "samsonDirectories.h"		/* SAMSON_MODULES_DIRECTORY                 */
#include <samson/Module.h>
#include "CommandLine.h"			/* AUCommandLine                            */
#include "ModulesManager.h"			/* Own interface                            */
#include <samson/Data.h>			/* ss::system::UInt ... */
#include "logMsg.h"



namespace ss
{
	
	/**
	 Utility function to fill data
	 */
	
	
	void fillKVFormat( network::KVFormat* format , KVFormat f )
	{
		format->set_keyformat( f.keyFormat );
		format->set_valueformat( f.valueFormat );
	}
	
	void fillKVInfo( network::KVInfo* info , KVInfo i )
	{
		info->set_size( i.size );
		info->set_kvs( i.kvs );
	}

	ModulesManager::ModulesManager()
	{
		addModules();
	}
	
	void ModulesManager::addModules()
	{
		// Load modules from "~/.samson/modules" && "/etc/samson/modules"
		std::string modules_dir = SAMSON_MODULES_DIRECTORY;
		addModulesFromDirectory( modules_dir );
		//addModulesFromDirectory( "/etc/samson/modules" );
		 
	}

	void ModulesManager::addModulesFromDirectory( std::string dir_name )
	{	
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir_name.c_str())) == NULL) {
			//logError( "Error opening directory for modules " + dir_name );
			return;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			
			std::string path = dir_name + "/" + dirp->d_name;
			
			struct ::stat info;
			stat(path.c_str(), &info);
			
			if( S_ISREG(info.st_mode) )
				addModule( path );
			
		}
		closedir(dp);
		
	}

	
	void ModulesManager::reloadModules()
	{
		lock.lock();
		
		// Remove all modules unloading from memory
		for( std::map<std::string, Module*>::iterator i = modules.begin() ; i != modules.end() ; i++)
		{
			Module *module = i->second;
			if( module->hndl)
				dlclose(module->hndl);
			delete module;
		}
		modules.clear();
		
		
		// Add modules again
		addModules();
		
		lock.unlock();
	}
	
	typedef Module*(*moduleFactory)();
	typedef std::string(*getVersionFunction)();

	void ModulesManager::addModule( std::string path )
	{
		
		void *hndl = dlopen(path.c_str(), RTLD_NOW);
		if(hndl == NULL){
			//logError( "KVWriter " + path );
			return;
		}
		void *mkr = dlsym(hndl, "moduleCreator");		
		void *getVersionPointer = dlsym(hndl, "getSamsonVersion");
		
		if( mkr && getVersionPointer )
		{
			moduleFactory f = (moduleFactory)mkr;
			getVersionFunction fv = (getVersionFunction)getVersionPointer;
			
			Module *module = f();
			std::string platform_version  = fv();
			
			if ( !module )
			{
				LM_E(( "Not possible to load module at path %s (not container found)" , path.c_str()));
				dlclose(hndl);
				return;
			}
			
			if( platform_version == SAMSON_VERSION )
			{
				//SSLogger::log( SSLogger::message, "Loaded module at path" + path );
				module->hndl = hndl;
				addModule( module );
			}
			else
			{
				//SSLogger::log( SSLogger::warning, "Not possible to load module at path" + path + " (wrong API version)" );
				dlclose(hndl);
				delete module;
				
			}
		}
		else
		{
			dlclose(hndl);
			//SSLogger::log( SSLogger::warning, "Not possible to link module at path" + path );
		}
		
		
		
	}

	void ModulesManager::addModule(  Module *container )
	{
		// Copy all the opertion to this top-level module
		modules.insert( std::pair<std::string , Module*>( container->name , container) );
		copyFrom( container	);
	}

	
	Module *ModulesManager::getModule( std::string name )
	{
		
		lock.lock();
		Module * module = _getModule( name ) ;
		lock.unlock();
		
		return module;
	}
	
	
	Module *ModulesManager::_getModule( std::string name )
	{
		
		size_t pos = name.find_last_of('.');

		std::string module_name;
		if (pos == std::string::npos )
			module_name = name;
		else
			module_name = name.substr( 0 , pos );
			
		std::map<std::string, Module*>::iterator i = modules.find( module_name );
		if( i == modules.end() )
			return NULL;
		else
			return i->second;
	}
	
	std::string ModulesManager::getObjectName( std::string name )
	{
		size_t pos = name.find_last_of('.');
		if (pos == std::string::npos )
			return "unknown";

		std::string tmp =  name.substr(pos+1 , name.length() - pos - 1);
		
		return tmp;
		
	}
	
	
	std::string ModulesManager::showModules()
	{
		
		lock.lock();
		std::stringstream o;
		
		o << "========================================================================================================" << std::endl;;
		o << "   MODULES" << std::endl;
		o << "========================================================================================================" << std::endl;;
		o << " Module name           version     Author" << std::endl;
		o << "========================================================================================================" << std::endl;;
		
		for( std::map<std::string, Module*>::iterator i = modules.begin() ; i != modules.end() ; i++)
		{
			Module *module = i->second;

			o << std::left << std::setw(25) << module->name;
			o << std::left << std::setw(10) << module->version; 
			o << std::left << std::setw(20) << module->author; 
			o << std::endl;
		}
		lock.unlock();
		
		return o.str();
	}
	
	
	std::string ModulesManager::showModule(std::string module_name)
	{
		lock.lock();
		
		Module *module = _getModule( module_name );
		if (!module)
			return "Unknown module " + module_name;
		
		std::stringstream o;
		
		o << "========================================================================================================" << std::endl;;
		o << "   MODULE: " << module->name << "   version " << module->version << "    author: " << module->author << std::endl;
		o << "========================================================================================================" << std::endl;;
		o << std::endl;
		
		 std::map<std::string , Operation*>::iterator i;
		 for (i = module->operations.begin() ; i != module->operations.end() ; i++ )
		 {
			 std::string operation_name = i->first;
			 Operation * op = i->second;
			 
			 std::string fullName =  module->name + "." + op->getName() ;
			 
			 o << "\t";
			 o << std::left << std::setw(10) << op->getTypeName() << " ";
			 o << std::left << std::setw(20) << fullName << " ";
			 o << std::left << std::setw(20) << op->helpLine() << " ";
			 o << "\n";
			 
		 }
		 
		 // Print datas supported
		 o << "\n";
		 o << "\tdatas: ";
		 for( std::map<std::string , Data*>::iterator i = module->datas.begin() ; i != module->datas.end() ; i++)
		 o << i->first << " ";
		 o << std::endl;
		 o << std::endl;
		

		lock.unlock();
		return o.str();
	}
	
	std::string ModulesManager::showFind( std::string command)
	{
	
		
		// Find operation
		// help find -map -reduce -script -input base.UInt-base.Uint -output base.UInt base.UInt -numInputs 1 -numOutputs 1
		au::CommandLine _cmdLine;
		_cmdLine.set_flag_boolean("map");
		_cmdLine.set_flag_boolean("reduce");
		_cmdLine.set_flag_string("input", "-");
		_cmdLine.set_flag_string("output", "-");
		
		_cmdLine.parse( command );
		
		
		// Reparse this command
		bool map_only  = _cmdLine.get_flag_bool("map");
		bool reduce_only  = _cmdLine.get_flag_bool("reduce");
		
		if( map_only && reduce_only )
			return "Not possible to set map only and reduce only\n";
		
		// Looking if the input is set in the command line
		std::string _input = _cmdLine.get_flag_string("input");
		KVFormat inputFormat;
		bool input_format;
		if( _input == "-" )
			input_format = false;
		else
		{
			input_format = true;
			inputFormat.setFromString( _input ) ;
		}
		
		// Looking if the output is set in the command line
		std::string _output = _cmdLine.get_flag_string("output");
		KVFormat outputFormat;
		bool output_format;
		if( _output == "-" )
			output_format = false;
			else
			{
				output_format = true;
				outputFormat.setFromString( _input ) ;
			}
	
		std::ostringstream o;

		
		
		for( std::map<std::string, Module*>::iterator i = modules.begin() ; i != modules.end() ; i++)
		{
			Module *module = i->second;
			for (std::map<std::string , Operation*>::iterator j = module->operations.begin() ; j != module->operations.end() ; j++ )
			{
				Operation * op = j->second;
				
				bool show = true;
				
				if (map_only && (op->getType() != Operation::map) )
					show = false;

				if (reduce_only && (op->getType() != Operation::reduce) )
					show = false;
				

				if ( input_format  && !op->containsInputFormat( inputFormat ) )
					show = false;
				
				if ( output_format && !op->containsOutputFormat( inputFormat ) )
					show = false;
				
				
				if( show )
				{
					
					std::string fullName = module->name + "." + op->getName();
					
					o << "\t";
					o << std::left << std::setw(10) << op->getTypeName() << " ";
					o << std::left << std::setw(20) << fullName << " ";
					o << std::left << std::setw(20) << op->helpLine() << " ";
					o << "\n";
				}
				
			}
		}
		
		return o.str();
		
	}
	
	
	// Fill help responses
	void ModulesManager::helpOperations( network::HelpResponse *response, network::Help help  )
	{
		lock.lock();
		
		{
			for (std::map<std::string , Operation*>::iterator j = operations.begin() ; j != operations.end() ; j++ )
			{
				Operation * op = j->second;

				if( !help.has_name() || j->first == help.name() )
				{
				
					network::Operation *o = response->add_operation();
					o->set_name( j->first );
					o->set_help( op->help() );
					
					// Format
					std::vector<KVFormat> input_formats = op->getInputFormats();
					std::vector<KVFormat> output_formats = op->getOutputFormats();
					
					for (size_t i = 0 ; i < input_formats.size() ; i++)
						fillKVFormat( o->add_input() , input_formats[i] );
					
					for (size_t i = 0 ; i < output_formats.size() ; i++)
						fillKVFormat( o->add_output() , output_formats[i] );
				}
				
			}
		}
		
		lock.unlock();
	}
	
	void ModulesManager::helpDatas( network::HelpResponse *response , network::Help help  )
	{
		lock.lock();
		
		{
			for (std::map<std::string , Data*>::iterator j = datas.begin() ; j != datas.end() ; j++ )
			{
				
				if( !help.has_name() || j->first == help.name() )
				{
					Data * data = j->second;
					network::Data *d = response->add_data();
					d->set_name( j->first );
					d->set_help( data->help() );
				}
				
			}
		}
				
		lock.unlock();
	}	
	
}
