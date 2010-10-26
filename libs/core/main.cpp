#include <algorithm>           // sort 
#include <iostream>            //
#include <vector>

#include "ModulesManager.h"
#include "samson/Data.h"
#include "KVWriterToBuffer.h"
#include "KVFileSaver.h"
#include "coding.h"
#include <vector>
#include <map>
#include <stdio.h>
#include <sstream>




namespace ss {

	/**
	 Vector of sizes in each Hasg Group
	 */
/*	
	class HGVector
	{
	public:
		ss_hg_info *hg;		// Hash groups information
		
		size_t size;		// Total size
		size_t kvs;			// Total number of kvs
		
		HGVector( std::string fileName )
		{
			hg	= (ss_hg_info*) malloc( KV_HASH_GROUP_VECTOR_SIZE );
			
			FILE *file = fopen( fileName.c_str() , "r" );
			fread( hg, 1 , KV_HASH_GROUP_VECTOR_SIZE , file ); 
			fclose( file );

			// totals
			size = 0;
			kvs = 0;
			for (int i = 0 ;i< KV_NUM_HASHGROUPS ;i++)
			{
				size += hg[i].size;
				kvs += hg[i].kvs;
			}
			
			
		}
		
		HGVector()
		{
			hg	= (ss_hg_info*) malloc( KV_HASH_GROUP_VECTOR_SIZE );
			for (int i = 0 ;  i < KV_NUM_HASHGROUPS ; i++)
			{
				hg[i].size = 0;
				hg[i].kvs = 0;
			}
			size = 0;
			kvs = 0;
			
		}
		
		~HGVector()
		{
			free( hg );
		}
		
		void add( HGVector *v )
		{
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
			{
				hg[i].size += v->hg[i].size;
				hg[i].kvs += v->hg[i].kvs;
			}
			
			size += v->size;
			kvs += v->kvs;
		}
		
	};
	*/
	
	/**
	 Information about a file and its hash-group sizes distribution
	 */
/*	
	class KVFile
	{

	public:
		std::string fileName;		// FileName
		HGVector info;				// Hash Group Size vector
		
		KVFile( std::string _fileName ) : info( _fileName )
		{
			fileName = _fileName;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "FILE: " << fileName << " ";
			for (int i = 0 ; i < 10 ; i++)
				o << info.hg[i].size << " ";
			o << "...\n";
			return o.str();
		}
		
	};
*/	
	/**
	 Information about all the files that form a queue
	 */
/*	
	class KVQueue
	{
		std::vector<KVFile*> files;
		HGVector info;				// Total information of this queue ( size and kvs per hash-group and total)

		friend class KVDataManager;
		
	public:
		
		void add( KVFile *file )
		{
			info.add(&file->info);			// Increase the total counters
			files.push_back( file );
		}
		
	};
	*/
	/**
	 Segment inside th hash-group range
	 */
/*	
	class Segment
	{
	public:
		int hg_from;
		int hg_to;
		
		Segment()
		{
			hg_from = -1;
			hg_to = -1;
		}
		
		Segment( int _hg_from , int _hg_to )
		{
			hg_from = _hg_from;
			hg_to = _hg_to;
		}

		
	};
*/	
	/**
	 Global manager of data in a SAMSON WORKER
	 */
/*	
	class KVDataManager
	{
		// Vector of queues
		std::map< std::string , KVQueue*> queues;
		
		KVQueue* findKVQueue( std::string name)
		{
			std::map< std::string , KVQueue*>::iterator q = queues.find( name );
			if( q == queues.end() )
				return NULL;
			else
				return q->second;
		}
		
	public:
		
		
		std::vector<Segment> getSegmentsForQueues( std::vector<std::string> queues , size_t max_memory_per_segment )
		{
			
			HGVector info;
			for ( std::vector<std::string>::iterator q = queues.begin(); q < queues.end() ; q++)
			{
				KVQueue *queue = findKVQueue( *q );
				info.add(&queue->info);
			}
				
			
			std::vector<Segment> segments;
			
			size_t total_size_for_segment;
			int from = 0;
			for (int i = 0; i < KV_NUM_HASHGROUPS ; i++)
			{
				total_size_for_segment+= info.hg[i].size;
				
				if (total_size_for_segment > max_memory_per_segment)
				{
					// New segment
					segments.push_back( Segment(from , i ) );
					from = i+1;
				}
				
			}
			
			// last segment
			if( from < KV_NUM_HASHGROUPS)
				segments.push_back( Segment(from , (int)KV_NUM_HASHGROUPS ) );
			
			return segments;
		}
		
		

	};
*/
	
	/** 
	 Extract information
	 */
	
/*	
	class KVFileSegment
	{
	public:
		KVFile *file;
		Segment segment;

		// Buffer to load data when necessary
		Buffer *b;
		
		KVFileSegment( KVFile *_file , Segment _segment)
		{
			file = _file;
			segment = _segment;
		}
		
		void load()
		{
			size_t offset = 0;
			size_t size = 0;
			
			for (int i = 0 ; i < segment.hg_from ; i++)
				offset += file->info.hg[i].size;
			for (int i = segment.hg_from ; i < segment.hg_to ; i++)
				size += file->info.hg[i].size; 
			
			b = MemoryManager::shared()->newPrivateBuffer( size);

			FILE *_file = fopen( file->fileName.c_str() , "r" );
			fseek(_file, KV_HASH_GROUP_VECTOR_SIZE + offset, SEEK_SET);
			fread(b->getData(), 1, size, _file);
			fclose(_file);
		}
		
	};
	
	class KVReader
	{
	public:
		KVReader( std::vector<KVFileSegment> segments , Data *dataKey , Data *dataValue )
		{
			
			for (size_t i = 0 ; i < segments.size() ; i++)
			{
				segments[i].load();
				
				// Parse and show data
				
				ss::system::UInt a;
				ss::system::Int32 b;
				
				size_t offset = 0;
				size_t size = segments[i].b->getSize();
				char *buffer = segments[i].b->getData();
				
				while (offset<size)
				{
					offset+= a.parse( buffer + offset );
					offset+= b.parse( buffer + offset );
					
					std::cout << "OFF:" << offset << " -> " << a.str() << "\n";
				}
				
			}
			
		}
		
		
	};

 */

}


int main( int argc , char *argv[] )
{
	/*
	std::cout << "Test\n";
	
	ss::ModulesManager mm;
	mm.checkData("system.UInt");
	ss::Data *data = mm.getData("example.example");
	
	if( data )
		std::cout << data->help();

	
	// Example of a file saver to save everything to disk
	ss::KVFileSaver fileSaver;
	
	
	ss::system::UInt a;
	ss::system::Int32 b;
	ss::KVWriterGeneral output(5,&fileSaver);	// Number of servers
	
	for (size_t i = 0 ; i < 100000 ; i++)
	{
		a = i;
		b = i;
		
		// Emit this key-value to a buffer
		output.emit( &a , &b );
	}
	output.close();

	

	fileSaver.save("/Users/andreu/test_data.bin");
*/	
/*	
	std::cout << "Preparing file to read\n";
	ss::KVFile file("/Users/andreu/test_data.bin");
	std::cout << file.str();
	std::vector<ss::KVFileSegment> segments;
	segments.push_back( ss::KVFileSegment(&file , ss::Segment(0 , 2) ) );
	ss::KVReader r( segments , NULL ,NULL);
*/	
	
}

 

