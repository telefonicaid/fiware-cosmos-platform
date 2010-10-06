/*

	Module Example module to play with samson (example)

	File: /Users/andreu/devel/svn_trunk_samson/modules/example/Module.h
	NOTE: This file has been generated with the samson_module tool, please do not modify

*/

#ifndef _H_SS_example_Module
#define _H_SS_example_Module


#include <samson/samsonModuleHeader.h>



namespace ss
{
namespace example{

// Datas defined in this module


// Operations defined in this module

#pragma mark CLASS generator : generator generator


class generator : public ss::Generator {

	public:


	//Main function to implement
	void run( std::vector<ss::KVWriter *>& writers );

	std::string help(){
		std::ostringstream o;
		return o.str();
	}


	// Implement this functions:
	//void ss::example::generator::run( std::vector<ss::KVWriter *>& writers ){};
};
#pragma mark CLASS map : map map


class map : public ss::Map {

	public:


	//Main function to implement
	void run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs );

	std::string help(){
		std::ostringstream o;
		o<<"Simple map\n";
		return o.str();
	}


	// Implement this functions:
	//void ss::example::map::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs ){};
};
#pragma mark CLASS r : reduce r


class r : public ss::Reduce {

	public:


	//Main function to implement
	void run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs );

	std::string help(){
		std::ostringstream o;
		return o.str();
	}


	// Implement this functions:
	//void ss::example::r::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs ){};
};

// Module definition

class Module : public ss::Module{

public:
	Module() : ss::Module("example","0.1","Andreu Urruela"){


		//Add datas


		//Add operatons
		{
		ss::Operation * operation = new ss::Operation( "generator" , ss::Operation::generator , au::factory<generator> );
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt" ,"base.UInt") );
		operation->setHelpLine("");
		std::ostringstream o;
		operation->setHelp( o.str() );
		add( operation );
		}
		{
		ss::Operation * operation = new ss::Operation( "map" , ss::Operation::map , au::factory<map> );
		operation->inputFormats.push_back( ss::KVFormat::format("base.UInt" ,"base.UInt") );
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt" ,"base.UInt") );
		operation->setHelpLine("");
		std::ostringstream o;
		o << "Simple map\n";
		operation->setHelp( o.str() );
		add( operation );
		}
		{
		ss::Operation * operation = new ss::Operation( "r" , ss::Operation::reduce , au::factory<r> );
		operation->inputFormats.push_back( ss::KVFormat::format("base.UInt" ,"base.UInt2") );
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt" ,"system.Void") );
		operation->setHelpLine("This is another example of what it is possible ;");
		std::ostringstream o;
		operation->setHelp( o.str() );
		add( operation );
		}
		{
		ss::Operation * operation = new ss::Operation( "s" , ss::Operation::script);
		operation->outputFormats.push_back( ss::KVFormat::format("base.UInt" ,"base.UInt") );
		operation->setHelpLine("This if the first script of the samson platform");
		std::ostringstream o;
		operation->setHelp( o.str() );
		// Code of this operation (usually scripts)
		operation->code.push_back("add_queue tmp base.UInt base.UInt");
		operation->code.push_back("example.generator tmp");
		operation->code.push_back("example.map tmp $1");
		add( operation );
		}
	}

	std::string help(){
		std::ostringstream o;
		return o.str();
	}
};

}


} // end of namespace ss

#endif


// List of functions to be implemented:

//generator
//void ss::example::generator::run( std::vector<ss::KVWriter *>& writers ){};

//map
//void ss::example::map::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs ){};

//r
//void ss::example::r::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs ){};


