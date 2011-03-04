#ifndef _H_LOAD_DATA_MANAGER
#define _H_LOAD_DATA_MANAGER

#include <string>

#include "samson.pb.h"					// ss::network:...
#include "au_map.h"						// au::map
#include "Lock.h"						// au::Lock
#include "samsonDirectories.h"			// SAMSON_DATA_DIRECTORY
#include "SamsonSetup.h"				// ss::SamsonSetup
#include "FileManagerDelegate.h"		// ss::FileManagerDelegate

namespace ss {

	class Buffer;
	class SamsonWorker;
	class LoadDataManager;

	
	class LoadDataManagerItem
	{
		
	public:

		size_t id;								// Identifier inside LoadDataManager
		
		int fromIdentifier;						// Dalilah identifier to send responses		
		Buffer *buffer;							// Buffer allocated for this task		
		LoadDataManager *dataManager;			// Pointer to the data manager		
		
		LoadDataManagerItem( size_t _id, int _fromIdentifier , LoadDataManager *dataManager );		
		virtual ~LoadDataManagerItem(){}
		virtual std::string getStatus( )=0;

	};
	
	
	class UploadItem : public LoadDataManagerItem 
	{

	public:
		
		network::UploadDataFile *upload_data_file;	// Information about the file-request to upload
		size_t sender_id;							// id of the sender
		
		std::string fileName;						// Filename of the new upload
		size_t size;								// Size of the uploaded file
		
		
		UploadItem( size_t _id, int _fromIdentifier , LoadDataManager *dataManager, const network::UploadDataFile &_uploadDataFile , size_t sender_id,  Buffer * buffer );
		~UploadItem();
		
		size_t submitToFileManager();
		void sendResponse( bool error , std::string error_message );
		
		std::string newFileName()
		{
			std::ostringstream fileName;
			fileName << "file_updaload_" << upload_data_file->load_id() << "_" << rand()%10000 << rand()%10000 << rand()%10000 << "." << upload_data_file->file_ext();
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
		
		
		network::DownloadDataFile *download_data_file;	// Information about the request
		size_t sender_id;								// id of the sender
		
		
		DownloadItem(size_t _id, int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadDataFile &_download_data_file, size_t sender_id);		
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

		size_t id;	// Auto-incrising counter to identify elements
		
	public:

		size_t upload_size;			// Simple information about the size of upload
		
		
		LoadDataManager( SamsonWorker *_worker )
		{
			worker = _worker;
			upload_size = 0;
		}
		
		// Add item to upload data
		void addUploadItem( int fromIdentifier, const network::UploadDataFile &uploadDataFile ,size_t sender_id, Buffer * buffer  );
		
		// add item to download data
		void addDownloadItem( int fromIdentifier, const network::DownloadDataFile &downloadDataFile, size_t sender_id );
		
		// Disk Manager notifications
		void notifyFinishReadItem( FileManagerReadItem *item  );	
		void notifyFinishWriteItem( FileManagerWriteItem *item  );	


		// Fill status information	
		void fill( network::WorkerStatus* ws);
		
	};

}

#endif
