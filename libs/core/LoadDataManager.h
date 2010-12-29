#ifndef _H_LOAD_DATA_MANAGER
#define _H_LOAD_DATA_MANAGER

#include <string>

#include "samson.pb.h"					// ss::network:...
#include "au_map.h"						// au::map
#include "Lock.h"						// au::Lock
#include "DiskManagerDelegate.h"		// ss::DiskManagerDelegate
#include "Status.h"			// ss::getStatusFromArray(.)
#include "samsonDirectories.h"			// SAMSON_DATA_DIRECTORY
#include "Status.h"				// au::Status


namespace ss {

	class Buffer;
	class SamsonWorker;
	class LoadDataManager;

	
	class LoadDataManagerItem
	{
		
	public:

		int fromIdentifier;						// Dalilah identifier to send responses		
		Buffer *buffer;							// Buffer allocated for this task		
		LoadDataManager *dataManager;			// Pointer to the data manager		
		LoadDataManagerItem( int _fromIdentifier , LoadDataManager *dataManager );		
		virtual ~LoadDataManagerItem(){}
		virtual std::string getStatus( )=0;

	};
	
	
	class UploadItem : public LoadDataManagerItem 
	{

	public:
		
		network::UploadData uploadData;		// Information about the request
		size_t sender_id;					// id of the sender
		
		std::string fileName;				// Filename fo the new upload
		size_t size;						// Size of the uploaded file
		
		UploadItem( int _fromIdentifier , LoadDataManager *dataManager, const network::UploadData &_uploadData , size_t sender_id,  Buffer * buffer );
	
		size_t submitToFileManager();
		void sendResponse( bool error , std::string error_message );
		
		static std::string newFileName()
		{
			std::ostringstream fileName;
			fileName << SAMSON_DATA_DIRECTORY << "file_updaload_" << rand()%10000 << rand()%10000 << rand()%10000;
			return fileName.str();
		}
		
		// Function to get the run-time status of this object
		std::string getStatus( )
		{
			std::ostringstream output;
			output << "<Up:" << au::Format::string(size,"B") << ">";
			return output.str();
		}
		
		
		
	};	
	
	
	/**
	 Item requested to be downloaded
	 */
	
	class DownloadItem : public LoadDataManagerItem 
	{
		
	public:
		
		network::DownloadData downloadData;		// Information about the request
		size_t sender_id;					// id of the sender
		
		
		DownloadItem( int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadData &_downloadData, size_t sender_id);		
		~DownloadItem();

		size_t submitToFileManager();
		void sendResponse( bool error , std::string error_message );

		std::string getStatus( )
		{
			std::ostringstream output;
			output << "<Down:?>";
			return output.str();
		}
		
		
	};
	
	
	class LoadDataManager : public FileManagerDelegate
	{
		friend class UploadItem;
		friend class DownloadItem;
		
		
		SamsonWorker *worker;

		au::Lock lock;
		
		au::map<size_t,UploadItem> uploadItem;		// Items to be uploded from dalilahs
		
		au::map<size_t,DownloadItem> downloadItem;	// Items to be downloaded to dalilahs
		
	public:
		
		LoadDataManager( SamsonWorker *_worker )
		{
			worker = _worker;
		}
		
		// Add item to upload data
		void addUploadItem( int fromIdentifier, const network::UploadData &uploadData ,size_t sender_id, Buffer * buffer  );
		
		// add item to download data
		void addDownloadItem( int fromIdentifier, const network::DownloadData &downloadData, size_t sender_id );
		
		// Disk Manager notifications
		virtual void fileManagerNotifyFinish(size_t id, bool success);


		// Fill status information	
		void fill( network::WorkerStatus* ws);
		
	};

}

#endif
