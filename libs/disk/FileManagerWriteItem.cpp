

#include "FileManagerWriteItem.h"	//Own interface

namespace ss {
		

	FileManagerWriteItem::FileManagerWriteItem( std::string _fileName , Buffer *_buffer , FileManagerDelegate *_delegate ) 
	: FileManagerItem( _delegate , FileManagerItem::write , _buffer->getSize() )
	{
		fileName = _fileName;
		buffer = _buffer;
	}

	void FileManagerWriteItem::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "Write Item File: " << fileName << " Size: " << au::Format::string( buffer->getSize() , "bytes" ) << "\n";
	}

}
