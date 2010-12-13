#ifndef _H_CODING
#define _H_CODING

/* ****************************************************************************
*
* FILE                 coding.h - 
*
*/
#include <samson/Data.h>
#include "Format.h"				// au::Format
#include "samson/KVFormat.h"	// ss::KVFormat
#include <assert.h>				// assert(.)
#include "samson.pb.h"			// network:...
#include <string.h>				// std::string

/*
#define KV_BUFFER_SIZE			1024*1024*256									
#define KV_BUFFER_MAX_NUM_KVS	 1024*1024*64									
 */

#define KV_MAX_SIZE					64*1024				// Max size for an individual key-value
#define KV_MAX_FILE_SIZE			1024*1024*1024		// 1 GB max file size
#define KV_NUM_HASHGROUPS			64*1024				// Number of hash-groups

#define NETWORK_KV_HASH_GROUP_VECTOR_SIZE	(sizeof(NetworkKVInfo)*KV_NUM_HASHGROUPS)	// Size of the structure of every network message ) 
#define FILE_KV_HASH_GROUP_VECTOR_SIZE		(sizeof(FileKVInfo)*KV_NUM_HASHGROUPS)		// Size of the structure of every file


#define NETWORK_TOTAL_HEADER_SIZE	(sizeof( NetworkHeader ) + NETWORK_KV_HASH_GROUP_VECTOR_SIZE)	// Total size of the header + info vector
#define FILE_TOTAL_HEADER_SIZE		(sizeof( FileHeader )    + FILE_KV_HASH_GROUP_VECTOR_SIZE)	// Total size of the header + info vector

namespace ss {

	/*
	 
	 This file defines all the formats used to store key-values in files , network-buffers and shared-memory buffers

	 FILE: (KVFile)
	 --------------------------------------------------------------------------------
	 	 
	 SAMSON platforms stores all key-values in a set of files with the following format:
	 
	 [FileHeader][Info for each hash-group][data]
	 
	 Where
	 
	 * FileHeader is a struct with some information about the format and content of the file
	 * Info for each hash-group is a vector of "KV_NUM_HASH_GROUPS" structs of type FileKVInfo
	 * data is the buffer of real data with the key-values cofidied using the rigth serialization

	 Note: In this case, all hash-groups are allways present
	 
	 NETOWRK: (KVNetworkFile)
	 --------------------------------------------------------------------------------

	 When travelling for the network, key-value buffers ( at the output of map, reduce, generator operations) have the following format
	 
	 [NetworkHeader][Info for each hash-group][data]
	 
	 where:
	 
	 * NetworkHeader is a struct with some information about the buffer ( like total size and number of key-values)
	 * Info for each hash-group is a vector of "KV_NUM_HASH_GROUPS" structs of type NetworkKVInfo
	 * data is the buffer of real data with the key-values cofidied using the rigth serialization
	 
	 Note: In this case, all hash-groups are allways present, so the info vector size is constant = KV_NUM_HASH_GROUPS*sizeof( NetworkKVInfo )
	 
	 
	 SHARED MEMORY: (KVSharedFile)
	 --------------------------------------------------------------------------------

	 When a partiuclar operation is executed (map,recuce of parseout), we need to store part or entire KVFile into a shared memory buffers
	 Indeed, if multiple files are needed, we store more than one in the same shared memory buffer
	 
	 [SharedHeader][KVSharedFile1][KVSharedFile2][KVSharedFile3][KVSharedFile4] ....
	 
	 Where SharedHeader informs about the number of files and some additional information
	 Each KVSharedFile has the following format
	 
	 [SharedHeader][Info for each hash-group][data]
	 
	 where:
	 * SharedHeader is a struct with some information about the process ( like total number of shared files and hash-group range)
	 * Info for each hash-group is a vector of "KV_NUM_HASH_GROUPS" structs of type FileKVInfo
	 * data is the buffer of real data with the key-values cofidied using the rigth serialization

	 Note that in this case, not all hash-group ranges have to be present.
	 
	 */
	

	// Unsigned types with different lengths in bits
	typedef size_t uint64; 
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
	
	typedef uint16 ss_hg;		// Hashgroup identifier			(16bits since we have 2^16 hash-groups)
	typedef uint16 ss_kv_size;	// Size for a particular KV
	

	/****************************************************************
	 Template class for the KVInfo structure
	 ****************************************************************/
	
	typedef uint32 hg_net_size;			// Size of a hashgroup			(32bits)
	typedef uint32 hg_net_kvs;			// Num KVs inside a hashgroup	(32bits)
	
	template <typename T_kvs,typename T_size >
	struct BaseKVInfo
	{
		T_size size;	// Total size
		T_kvs kvs;		// Total number of kvs

		
		BaseKVInfo(T_size _size ,T_kvs _kvs )
		{
			kvs = _kvs;
			size = _size;
		}
		
		BaseKVInfo()
		{
			kvs = 0;
			size = 0;
		}
		
		void init()
		{
			kvs = 0;
			size = 0;
		}
		
		void append( T_kvs _kvs , T_size _size)
		{
			kvs += _kvs;
			size += _size;
		}
		
		void append( BaseKVInfo<T_kvs,T_size> o )
		{
			kvs += o.kvs;
			size += o.size;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o <<  "( " << au::Format::string( kvs , "kvs" ) << " in " << au::Format::string( size ,"bytes" ) << " )";  
			return o.str();
		}
		
	};	
	
	template <typename Info> 
	struct BaseHeader 
	{
		int magic_number;			// Magic number to make sure reception is correct
		char keyFormat[100];		// Format for the key
		char valueFormat[100];		// Format for the value
		Info info;					// Total information in this file
		
		void init( )
		{
			magic_number =  4652783;
		}
		
		void setInfo( Info _info)
		{
			info = _info;
		}
		
		KVFormat getFormat()
		{
			return KVFormat( keyFormat , valueFormat );
		}
		
		void setFormat( KVFormat format )
		{
			snprintf(keyFormat, 100, "%s", format.keyFormat.c_str());
			snprintf(valueFormat, 100, "%s", format.valueFormat.c_str());
		}
		
		bool check()
		{
			return ( magic_number == 4652783);
		}
		
	};
	
	typedef BaseKVInfo<size_t,size_t> KVInfo;	// Old common definition ( to be substituted by File or Network )
	
	
	/****************************************************************
	 Network interface
	 ****************************************************************/
	
	typedef uint32 hg_net_size;			// Size of a hashgroup			(32bits)
	typedef uint32 hg_net_kvs;			// Num KVs inside a hashgroup	(32bits)

	typedef BaseKVInfo<hg_net_kvs,hg_net_size>  NetworkKVInfo;
	typedef BaseHeader<NetworkKVInfo> NetworkHeader;

	/****************************************************************
	 File interface
	 ****************************************************************/
	
	typedef uint32 hg_file_size;		// Size of a hashgroup			(32bits)
	typedef uint32 hg_file_kvs;			// Num KVs inside a hashgroup	(32bits)
	
	typedef BaseKVInfo<hg_file_kvs,hg_file_size>  FileKVInfo;
	typedef BaseHeader<FileKVInfo> FileHeader;
	
	/****************************************************************
	 Shared Memory ( use the file definitions )
	 ****************************************************************/

	struct SharedHeader
	{
		int magic_number;		// Magic number for the Reduce File Header
		
		int input;				// Input channel
		int num_inputs;			// Total number of inputs
		
		FileKVInfo info;		// Total info for this set of hash-groups
		
		int hg_begin;			// Hash group range we are processing
		int hg_end;

		size_t total_size;		// total size of this file including this header , info vector (FileKVInfo) and data
		
		void init(  )
		{
			magic_number =  5972384;
		}
		
		bool check()
		{
			if( magic_number != 5972384)
				return false;
			
			if( input >= num_inputs)
				return false;
			
			return true;
		}
		
		int num_hash_groups()
		{
			return hg_end - hg_begin;
		}
	};

	
	/**
	 A SharedFile from the Process side
	 */
	
	class ProcessSharedFile
	{
	public:
		
		// Pointer to the header
		SharedHeader *header;
		
		// Pointer to the info for each hash-group
		FileKVInfo *info;
		
		// Data pointer to data
		char *data;
		
		// Current hash group
		int hg;
		size_t offset;
		
		ProcessSharedFile()
		{
			hg = 0;
		}
		
		// Set the real pointer to the current data and returns the toal size of this file in the shared memory area
		size_t set( char *_data )
		{
			offset = 0;	// Init the offset of this file
						
			header = (SharedHeader*) _data;
			assert( header->check() );	// Check magic number

			// Get the number of hash group from header
			int num_hash_groups = header->num_hash_groups();
			
			info = (FileKVInfo*) ( _data + sizeof(SharedHeader) );
			data = (_data + sizeof(SharedHeader) + sizeof(FileKVInfo)*num_hash_groups);
			
			size_t total_size = sizeof(SharedHeader) + sizeof(FileKVInfo)*num_hash_groups + header->info.size;
			assert( total_size == header->total_size );
			
			return total_size;
			
		}
		
		size_t getNumKVsInThisHashGroup()
		{
			return info[hg].kvs;
		}
		
	};	

	
	class FileManagerReadItem;
	
	/**
	 A SharedFile from the ProcessAssitant side
	 */
	
	class ProcessAssistantSharedFile
	{
	public:
		
		FileKVInfo *info;				// Information about key-values ( read directly from file )
		FileKVInfo *cumulative_info;	// Cumulative information
		
		int input;						// Input of this file ( input channel )
		int num_inputs;					// total number of inputs
		std::string fileName;			// fileName of this file
		
		ProcessAssistantSharedFile( int _input  , int _num_inputs ,  std::string _fileName)
		{
			input = _input;
			fileName = _fileName;
			num_inputs = _num_inputs;
			
			info				=  (FileKVInfo*) malloc( sizeof( FileKVInfo) * KV_NUM_HASHGROUPS );
			cumulative_info		=  (FileKVInfo*) malloc( sizeof( FileKVInfo) * KV_NUM_HASHGROUPS );
		}
		
		~ProcessAssistantSharedFile()
		{
			free( info );
			free( cumulative_info );
		}
		
		
		void setup()
		{
			FileKVInfo cumulative;
			cumulative.init();
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
			{
				cumulative.append( info[i] );
				cumulative_info[i] = cumulative;
			}
		}
		
		
		// Get the header of this file for a particular range of hash-groups
		
		SharedHeader getSharedHeader( int hg_begin , int hg_end )
		{
			// Header to be written on shared memory
			SharedHeader header;
			header.init( );
			
			// Input channel information
			header.input = input;				
			header.num_inputs = num_inputs;
			
			// Information about the number of kvs and size
			header.info = getKVInfo( hg_begin , hg_end);	
			
			// Hash group range we are processing
			header.hg_begin = hg_begin;			
			header.hg_end = hg_end;			
			
			// total size of this file including this header , info vector (FileKVInfo) and data
			header.total_size =  sizeof(SharedHeader) + sizeof(FileKVInfo)*(hg_end-hg_begin) + header.info.size ;	
			
			assert( header.check() );
			return header;
		}
		
		
		// Operation to get information when processing a subset of hash-groups ( reduce item operation )
		
		FileKVInfo getKVInfo( int hg_begin , int hg_end )
		{
			FileKVInfo _info;
			if (hg_begin == 0)
			{
				// Spetial case with the first operation
				_info = cumulative_info[hg_end-1];
			}
			else
			{
				_info.size = cumulative_info[hg_end-1].size - cumulative_info[hg_begin-1].size;
				_info.kvs = cumulative_info[hg_end-1].kvs - cumulative_info[hg_begin-1].kvs;
			}
			
			return _info;
		}
		
		size_t getFileOffset( int hg_begin )
		{
			size_t file_offset = FILE_TOTAL_HEADER_SIZE;
			if( hg_begin > 0)
				file_offset += cumulative_info[hg_begin-1].size;
			
			return file_offset;
		}
		
		
	};
	
	
	class ProcessAssistantSharedFileCollection
	{
		
	public:
		
		std::vector<ProcessAssistantSharedFile*> file;
		
		int num_inputs;				// Number of input channels
		int *num_input_files;		// Number of input files per input channels
		int total_num_input_files;	// Total number of files
		
		size_t* size_of_hg;			// Vector with the total size per hash_group ( necessary when organizing reduce )
		
		ProcessAssistantSharedFileCollection( const network::WorkerTask & workerTask  )
		{
			// Get the number of inputs & files per input
			int num_inputs = workerTask.input_size();
			total_num_input_files = 0;
			num_input_files = (int*) malloc( sizeof(int) * num_inputs );
			for( int i = 0 ; i < num_inputs ; i++)
			{
				num_input_files[i] = workerTask.input(i).file_size();
				for (int j = 0 ; j < num_input_files[i] ; j++)
					file.push_back( new ProcessAssistantSharedFile( i , num_inputs ,workerTask.input(i).file(j).name() ) );
				total_num_input_files += num_input_files[i];
			}
			
			
			// Size of each hash-group
			size_of_hg = (size_t*) malloc(KV_NUM_HASHGROUPS *sizeof(size_t));
			
		}
		
		void setup()
		{
			for (int i = 0 ; i < total_num_input_files ; i++)
				file[i]->setup();
			
			// Compute the total size per hash group	
			for (int hg = 0 ; hg < KV_NUM_HASHGROUPS ; hg++)
			{
				size_of_hg[hg] = 0;
				for (int f = 0 ; f < total_num_input_files ; f++)
					size_of_hg[hg] += file[f]->info[hg].size;
			}
		}
		
		~ProcessAssistantSharedFileCollection()
		{
			free( num_input_files);
			free( size_of_hg );
			
			for (size_t i = 0 ; i < file.size() ;i++)
				delete file[i];
			
		}
	};
	
	
}

#endif
