

#include "FileManagerReadItem.h"


namespace ss {




	FileManagerReadItem::FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  SimpleBuffer _simpleBuffer , FileManagerDelegate *_delegate )
	: FileManagerItem( _delegate , FileManagerItem::read , _size )
	{
		simpleBuffer = _simpleBuffer;
		buffer = NULL;

		// Make sure we have space in the provided simple buffer to write all we have to read
		assert( simpleBuffer.checkSize(_size) );
		
		fileName = _fileName;
		offset = _offset;
		size = _size;
	}

	FileManagerReadItem::FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  Buffer * _buffer , FileManagerDelegate *_delegate )
	: FileManagerItem( _delegate , FileManagerItem::read , _size )
	 {
	   buffer = _buffer;
	   simpleBuffer = buffer->getSimpleBuffer();

	   // Make sure we have space in the provided simple buffer to write all we have to read                                                                        
	   assert( simpleBuffer.checkSize(_size) );

	   fileName = _fileName;
	   offset = _offset;
	   size = _size;
	   
	 }


	// Replace the delegate ( before sending to File Manager )
	void FileManagerReadItem::setDelegate( FileManagerDelegate *_delegate )
	{
		delegate = _delegate;
	}

}
