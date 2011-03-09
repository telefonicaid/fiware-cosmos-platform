
#include "Queue.h"		// Own interface
#include "QueueFile.h"	
#include "AutomaticOperation.h"
#include "logMsg.h"		// LM_M

namespace ss
{

	
#pragma mark Queue
	
	
	Queue::Queue( std::string name , KVFormat format )
	{
		_name = name;
		_format = format;
		
		monitor.addMainParameter( "name" , _name );
		monitor.addMainParameter( "format" , _format.str() );
		
		_num_files =0 ;
		
		
	}
	
	Queue::~Queue()
	{
	}
	
	void Queue::addFile( int worker, std::string _fileName , KVInfo info )
	{
		// Update global info
		_info.append( info );
		
		// Insert file in the local list
		files.insertInMap( _fileName , new QueueFile( _fileName , worker , info));
		//files.push_back( new QueueFile( _fileName , worker , info) ); 	
		
		// Thread save version of the number of files
		_num_files++;
	}

	bool Queue::removeFile( int worker, std::string _fileName , KVInfo info )
	{
		QueueFile *qf = files.findInMap( _fileName );
		
		if( !qf )
			return false;

		if( ( qf->worker == worker ) && ( info.size == qf->info.size ) && ( info.kvs == qf->info.kvs ) )
		{
			qf = files.extractFromMap( _fileName );
			if( qf )
			{
				_info.remove( info );
			
				// Thread save version of the number of files
				_num_files--;
				
				delete qf;
				return true;
			}
			else
				return false;
		}
		
		return true;
	}
	
	
	void Queue::rename( std::string name )
	{
		_name = name;
	}
	
	void Queue::clear()
	{
		_info.clear();
		files.clear();
	}
	
	void Queue::copyFileFrom( Queue *q )
	{
		// Copy files form the other queue
		files.insert( q->files.begin() , q->files.end() );

		// Append total information about key-value
		_info.append( q->_info );
		
		// Get the number of files
		_num_files = files.size();
		
	}

	network::FullQueue * Queue::getFullQueue()
	{
		network::FullQueue *network_fq = new network::FullQueue();

		// Queue information
		network::Queue* network_q = network_fq->mutable_queue();
		network_q->set_name( _name );
		
		network::KVFormat *network_kvformat = network_q->mutable_format();
		network_kvformat->set_keyformat( _format.keyFormat );
		network_kvformat->set_valueformat( _format.valueFormat );
		
		// File list infromation
		au::map< std::string , QueueFile >::iterator f;
		for (f = files.begin() ; f != files.end() ; f++)
		{
			network::File * file = network_fq->add_file();
			file->set_name( (f->second)->fileName );
			file->set_worker( (f->second)->worker );

			network::KVInfo *info = file->mutable_info();

			info->set_size( (f->second)->info.size );
			info->set_kvs( (f->second)->info.kvs );
		}
		
		return network_fq;
	}
	
	
}