

#include "FileManagerReadItem.h"


namespace ss {


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