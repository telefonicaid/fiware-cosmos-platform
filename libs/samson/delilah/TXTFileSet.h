#ifndef _H_TXT_FILE_SET
#define _H_TXT_FILE_SET

namespace samson {

	
	class Delilah;
	class Buffer;
	
    class DataSource
    {
        
    public:
        
		virtual bool isFinish()=0;
		virtual void fill( engine::Buffer *b )=0;
        virtual size_t getTotalSize()=0;
        
    };
    
    
    class BufferDataSource : public DataSource
    {
        char *data;
        size_t size;
        size_t pos;
        
    public:
        
        BufferDataSource( char *_data , size_t _size )
        {
            data = _data;
            size = _size;
            pos = 0;
        }
        
        ~BufferDataSource()
        {
            if( data )
                delete data;
        }
        
		bool isFinish()
        {
            return (pos>=size);
        }
		
        void fill( engine::Buffer *b )
        {
            size_t available_size = b->getAvailableWrite();
            
            if( available_size > (size-pos) )
            {
                b->write( &data[pos] , size-pos );
                pos = size;
                return;
            }
            else
            {
                // Search the last return 
                size_t pos2 = pos + available_size;
                
                while( (pos2 > pos) && (data[pos2] != '\n') )
                    pos2--;
                
                
                b->write( &data[pos] , pos2 - pos );
                pos = pos2;
            }
            
        }
        
        size_t getTotalSize()
        {
            return size;
        }
        
        
    };
    
	
	class TXTFileSet : public DataSource
	{
		
		std::vector<std::string> fileNames;		// List of files to read
		std::ifstream inputStream;				// Stream of the file we are reading	
		
		int file;
		bool finish;
		
		char previousBuffer[ 10000 ];			// Part of the previous buffer waiting for the next read
		size_t previousBufferSize;
		
		std::vector<std::string> failedFiles;		// List of files that could not be uploaded		
		
	public:
		
		TXTFileSet( std::vector<std::string> &_fileNames )
		{
			fileNames = _fileNames;
			
			previousBufferSize = 0;
			
			finish = false;
			file = 0;	
			
			openNextFile();
		}
		
		void openNextFile()
		{
			while ( !inputStream.is_open() ) 
			{
				
				if( file >= (int)fileNames.size() )
				{
					finish = true;
					return;
				}
				
				inputStream.open( fileNames[file].c_str() );
				if( !inputStream.is_open() )
					failedFiles.push_back( fileNames[file] );	// Add to the list of failed files
				file++;
				
			}
		}
		
		
		bool isFinish()
		{
			return finish;
		}
		
		// Read as much as possible breaking in lines
		void fill( engine::Buffer *b );
		
		std::vector<std::string> getFailedFiles()
		{
			return failedFiles;
		}
		
        
        std::string getStatus()
        {
            std::ostringstream output;
            output << "TXTFile: ";
            if( finish )
                output << " finished";
            else
                output << " running: " << fileNames.size() << " pending files and " << failedFiles.size() << " failed files";
            return  output.str();
        }
        
        
        size_t getTotalSize()
        {
            size_t totalSize = 0;
            
            // Compute the total size for all the files
            for ( size_t i =  0 ; i < fileNames.size() ; i++)
                totalSize += au::Format::sizeOfFile( fileNames[i] );

            return totalSize;
        }
        
		
	};
	
	
}

#endif
