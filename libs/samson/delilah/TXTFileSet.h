#ifndef _H_TXT_FILE_SET
#define _H_TXT_FILE_SET

namespace samson {

	
	class Delilah;
	class Buffer;
	
	
	class TXTFileSet
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
                            
        
		
	};
	
	
}

#endif
