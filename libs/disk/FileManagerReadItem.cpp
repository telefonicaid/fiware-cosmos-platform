

#include "FileManagerReadItem.h"


namespace ss {

	size_t FileManagerReadItem::sizeOfFile( std::string fileName )
	{
		struct ::stat info;
		if( stat(fileName.c_str(), &info) == 0)
			return info.st_size;
		else
		{
			assert( false );
			return 0;
		}
	}


	FileManagerReadItem::FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  char *_buffer , FileManagerDelegate *_delegate )
	: FileManagerItem( _delegate , FileManagerItem::read , _size )
	{
		buffer = _buffer;
		
		fileName = _fileName;
		offset = _offset;
		size = _size;
	}


	// Replace the delegate ( before sending to File Manager )
	void FileManagerReadItem::setDelegate( FileManagerDelegate *_delegate )
	{
		delegate = _delegate;
	}

	void FileManagerReadItem::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "Read Item File: " << fileName << " Offset: " << offset << " Size: " << au::Format::string( size , "bytes") << "\n";
	}
}