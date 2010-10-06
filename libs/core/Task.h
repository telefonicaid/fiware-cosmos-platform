#ifndef SAMSON_TASKS_H
#define SAMSON_TASKS_H

#include <algorithm>

#include "TaskBase.h"            /* TaskBase                        */
#include "TaskManager.h"
#include "SSManager.h"
#include "KVReader.h"
#include "KVSet.h"
#include "KVQueue.h"
#include "ModulesManager.h"
#include "KVSetBuffer.h"
#include "SSLogger.h"
#include "KVManager.h"
#include "KVQueueWriter.h"
#include "KVWriter.h"            /* KVWriter                                 */
#include "Operation.h"           /* Operation                                */



/* **************************************************************************** 
*
* A Task is initiated at command line
*/

namespace ss {
	
	
#define TASK_TYPE_UNKNOWN			0
#define TASK_TYPE_SHOW				1
#define TASK_TYPE_GENERATOR			2
#define TASK_TYPE_SCRIPT			3
#define TASK_TYPE_MAP_REDUCE		4
#define TASK_TYPE_SUB_MAP_REDUCE	5
#define TASK_TYPE_SPREAD			6

	class Task : public TaskBase
	{
		std::string _command;						//!< Main command sentence
		int taskType;								//!< Type of operation

		std::stringstream errOutput;				//!< Error output
		bool error;									//!< Flag to detect errors in definition and execution
		
		// Common properties used by many operations
		std::string operation_name;
		std::vector<std::string> input_queues;		//!< input queues
		std::vector<std::string> output_queues;		//!< output queues
		std::vector<KVFormat> input_queues_format;	//!< input queues format
		std::vector<KVFormat> output_queues_format;	//!< output queues format
		
		Operation *operation;						//!< Current opertion
		std::vector<KVSetVector*> groups;			//!< Input data
		std::vector<KVWriter *> writers;			//!< Output data

		// Only for scripts
		int scriptLineCounter;
		std::map <std::string,std::string> replacements;	// strings to substituted during script execution
		std::vector<std::string> tmp_queues;				// tmp queues that should be removed at the end of the script
		
		// Only for sub operations
		int hash;
		bool remove_originals;	
		
	public:
		
		Task( std::string command , size_t id, Task * parentTask );

		/**
		 Function where you ask for any KVSet necessary
		 */
		
		void prepareKVSets();
		
		std::string getCommand()
		{
			return _command;
		}

		bool isError()
		{
			return error;
		}
		
		std::string getErrorMessage()
		{
			return errOutput.str();
		}
		
	private:
		
		bool runShow( );
		bool runGenerator();
		bool runMapReduce();
		bool runSubMapReduce();
		bool runScript();
		bool runSpread( );
		
	
	public:		
		bool run()
		{
			
			switch (taskType) {
				case TASK_TYPE_SHOW:
					return runShow();
					break;
				case TASK_TYPE_MAP_REDUCE:
					return runMapReduce();
					break;
				case TASK_TYPE_GENERATOR:
					return runGenerator();
					break;
				case TASK_TYPE_SUB_MAP_REDUCE:
					return runSubMapReduce();
					break;
				case TASK_TYPE_SCRIPT:
					return runScript();
					break;
				case TASK_TYPE_SPREAD:
					return runSpread();
					break;
				default:
					break;
			}
			

			assert(false);
			return true;
			
		}
		
		
		/**
		 Auxiliar function to get all the input and output queues for the selected operation
		 Return false, it there was an error
		 */
		
		bool setupInputOutput( AUCommandLine& _cmdLine)
		{
			if( (size_t)_cmdLine.get_num_arguments() < ( 1 + operation->inputFormats.size() + operation->outputFormats.size() ) )
			{
				errOutput << "Error in number of arguments." << std::endl;
				error = true;
				return false;
			}
			
			// Get the storage pointers
			int parameter = 1;
			
			for (size_t i = 0 ; i < operation->inputFormats.size() ; i++)
			{
				std::string queue_name = _cmdLine.get_argument(parameter++);
				input_queues.push_back(queue_name);
				
				// Get the format
				input_queues_format.push_back( KVManager::shared()->getKVQueueFormat( queue_name ) );
				
			}
			
			for (size_t i = 0 ; i < operation->outputFormats.size() ; i++)
			{
				std::string queue_name = _cmdLine.get_argument(parameter++);
				output_queues.push_back(queue_name);				
				
				// Get the format
				output_queues_format.push_back( KVManager::shared()->getKVQueueFormat( queue_name ) );
				
			}
			
			
			// Check input formats
			for (size_t i = 0 ; i < operation->inputFormats.size() ; i++)
				if( !operation->inputFormats[i].isEqual( input_queues_format[i] ) )
				{
					errOutput << "Error in the format of " << (i+1) << "-th input queue: " << input_queues[i] << " ("<< input_queues_format[i].str() <<")";
					errOutput << " Format expected by this operation  (" <<  operation->inputFormats[i].str() <<  ")" << std::endl;
					return false;
				}

					
			// Check output formats
			for (size_t i = 0 ; i < operation->outputFormats.size() ; i++)
				if( !operation->outputFormats[i].isEqual( output_queues_format[i] ) )
				{
					errOutput << "Error in the format of " << (i+1) << "-th output queue: " << output_queues[i] << " ("<< output_queues_format[i].str() <<")";
					errOutput << " Format expected by this operation  (" <<  operation->outputFormats[i].str() <<  ")" << std::endl;
					return false;
				}
			
			return true;
		}
		
		void startInputOtput()
		{
			
			// Inputs
			for (int i = 0  ; i < (int)input_queues.size() ; i++)
			{
				KVSetVector *tmp =  KVManager::shared()->getKVSetsForTask(this, input_queues[i] , hash , remove_originals );
				groups.push_back( tmp );
			}
			
			// Writers 
			int num_outputs = output_queues.size();
			for (int i = 0 ; i < num_outputs ; i++)
				writers.push_back(  KVManager::shared()->getKVQueueWriter( this , output_queues[i] ) );
			
		}
	
		
		void finishInputOtput()
		{
			// Close KV writers
			
			for (int i = 0 ; i < (int)output_queues.size() ; i++)
			{
				KVQueueWriter *writer = (KVQueueWriter*)writers[i];
				writer->close();
				delete writer;
			}
			
			writers.clear();
			
			// Delete the KVGroups
			for (size_t i = 0 ; i < groups.size() ; i++)
				delete groups[i];
			
			
		}

		
		
#pragma mark Auxiliar functions for scripts
		
		size_t findPos( std::string &command , std::string &text)
		{
			size_t pos = command.find( text );
			if ( pos == std::string::npos )
				return pos;
			
			
			// Check there is a space or tab before
			if( ( command[pos-1] != ' ') && ( command[pos-1] != '\t') )
				return std::string::npos;
			
			// Check there is a space or tab before ( or the end of the command)
			size_t pos_end = pos + text.length();
			if( ( pos_end < command.length() ) && ( command[pos_end] != ' ') && ( command[pos_end] != '\t') )
				return std::string::npos;
			
			return pos;
		}
		
		void replace_string( std::string& new_command , std::string text , std::string new_text	)
		{
			assert( text.length() > 0);
			assert( new_command.length() > 0);
			
			size_t pos;
			pos = findPos( new_command , text );
			while( pos != std::string::npos )
			{
				new_command.replace( pos , text.length() , new_text );
				pos = findPos( new_command , text );
			}
			
		}
		
		bool quitScript()
		{
			// Remove temporal stuff and return true (always)
			
			// Remove temporal queues
			for (std::vector<std::string>::iterator i = tmp_queues.begin() ; i != tmp_queues.end() ;i++)
			{
				std::string queue_name = *i;
				KVManager::shared()->removeKVQueue( getId() ,  queue_name );
			}
			
			
			return true;
			
		}			

#pragma mark Submit new commands ( as children )

		void runCommand( std::string command );
		
		
#pragma mark Debug information
		
		std::string str();
	};
}

#endif
