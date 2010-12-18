
#include "Queue.h"		// Own interface
#include "QueueFile.h"	

namespace ss
{
	void Queue::addFile( int worker, std::string _fileName , KVInfo info )
	{
		// Upodate global info
		_info.append( info );
		
		// Insert file in the local list
		files.push_back( new QueueFile( _fileName , worker , info) ); 	
		
		// Thread save version of the number of files
		_num_files++;
	}

	void Queue::rename( std::string name )
	{
		_name = name;
	}
	
	void Queue::clear()
	{
		_info.init();
		files.clear();
	}
	
	void Queue::copyFileFrom( Queue *q )
	{
		// Copy files form the other queue
		std::list< QueueFile* >::iterator f;

		for (f = q->files.begin() ; f != q->files.end() ; f++)
			files.push_back( *f );

		_info.append( q->_info );
		
	}

	void Queue::insertFilesIn( network::FileList *fileList )
	{
		std::list< QueueFile* >::iterator f;
		for (f = files.begin() ; f != files.end() ; f++)
		{
			network::File * file = fileList->add_file();
			file->set_name( (*f)->fileName );
			file->set_worker( (*f)->worker );

			network::KVInfo *info = file->mutable_info();

			info->set_size( (*f)->info.size );
			info->set_kvs( (*f)->info.kvs );
		}
	}

}