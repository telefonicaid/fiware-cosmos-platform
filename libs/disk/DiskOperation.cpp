#include "DiskOperation.h"			// Own interface
#include "DiskManagerDelegate.h"	// DiskManagerDelegate


namespace ss {
	
	
	DiskOperation::DiskOperation( size_t id )
	{
		_id = id;
		
		// By default
		buffer = NULL;
		read_buffer = NULL;
		
	}
	
	size_t DiskOperation::idGet()
	{
		return _id;
	}
	
	std::string DiskOperation::directoryPath( std::string path )
	{
		size_t pos = path.find_last_of("/");
		
		if ( pos == std::string::npos)
			return ".";
		else
			return path.substr( 0 , pos );
	}
	
	bool DiskOperation::setDevice( )
	{
		// Make sure we only use one device
		st_dev = 0;
		return true;
		
		
		struct ::stat info;
		stat(fileName.c_str(), &info);
		
		if( S_ISREG(info.st_mode) )
		{
			st_dev = info.st_dev;	// Get the devide of the inode 
			return true;
		}
		
		// File currently does not exist ( it wil be writed )
		if( type != DiskOperation::write )
			return false;
		
		// Get the path of the directory
		std::string directory = directoryPath( fileName );
		stat(directory.c_str(), &info);
		
		if( S_ISDIR(info.st_mode) )
		{
			st_dev = info.st_dev;	// Get the devide of the inode 
			st_dev = 0;// Force all files to be process as the same inode
			return true;
		}
		
		// Something wrong...
		return false;
	}
	
	
	std::string DiskOperation::getDescription()
	{
		std::ostringstream o;
		
		switch (type) {
			case write:
				o << "Write file: '" << fileName << "' Size:" << au::Format::string(size,"B");
				break;
			case read:
				o << "Read file: '" << fileName << "' Size:" << au::Format::string(size,"B") << " ["<< size << "B] Offset:" << offset;
				break;
			case remove:
				o << "Remove file: '" << fileName << "'";
				break;
		}
		
		
		return o.str();		
	}

	
	void DiskOperation::setError( std::string message )
	{
		
		std::ostringstream o;
		o << message << " ( " << getDescription() << " )";
		error.set( o.str() );
	}

	
}
