#ifndef _H_REDUCE_INFORMATION
#define _H_REDUCE_INFORMATION

#include "coding.h"			// FileKVInfo

namespace ss
{
	class ReduceInformation
	{
		
	public:
		
		// Place where FileKVInfo data is really stored
		char *data;	
		bool _free;	// Flag to indicate if we allocated space
		
		// Space for the pointer info
		FileKVInfo ***info;
		
		int num_inputs;
		int *num_input_files;
		int total_num_input_files;
		
		ReduceInformation( char *_data,  int _num_inputs , int *_num_input_files , int num_hash_groups )
		{
			// Get the number of inputs
			num_inputs = _num_inputs;
			
			// Copy the input number of files per input
			num_input_files = (int*) malloc( sizeof(int) * num_inputs );
			memcpy(num_input_files, _num_input_files, (num_inputs*sizeof(int)) );
			
			// Compute the total number of files
			total_num_input_files = 0;
			for (int i = 0 ; i< num_inputs ; i++)
				total_num_input_files += num_input_files[i];
			
			if( _data )
			{
				data = _data;
				_free =  false;
				
			}
			else
			{
				data = (char*)malloc( total_num_input_files * num_hash_groups * sizeof( FileKVInfo ) );
				_free = true;
			}
			
			// Allocate space for the vector of information
			info =  (FileKVInfo***) malloc( sizeof( FileKVInfo**) * num_inputs );	// Number of inputs
			
			int file_counter = 0;
			for( int i = 0 ; i < num_inputs ; i++)
			{
				info[i] = (FileKVInfo **) malloc( sizeof(FileKVInfo *) * num_input_files[i] );
				for (int f = 0; f < num_input_files[i] ; f++)
					info[i][f] = (FileKVInfo*) ( data + file_counter*( num_hash_groups * sizeof( FileKVInfo ) ) ); ;
			}
		}
		
		~ReduceInformation()
		{
			if( _free )
				free( data );
			
			for (int i = 0 ; i < num_inputs ; i++)
				free( info[i] );
			free( info );
		}
	};
	
}


#endif
