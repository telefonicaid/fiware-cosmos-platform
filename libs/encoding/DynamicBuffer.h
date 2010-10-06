

#pragma once
#include "Common.h"


/** Generic interface of a synamic buffer */

namespace ss {

class DynamicBuffer
{
public:
	/** Function to write new sata to the buffer */
	virtual size_t write( char *data , size_t length)=0;	
	
	/** Function to serialize a varInt */
	virtual size_t writeVarInt( size_t value )=0;
	
	/** Function to serialize a google protocol buffer */
	virtual size_t writeMessage( const SSMessage &message )=0;
	
	/** Get the accumulated size */
	virtual size_t getLength()=0;
	
	/** Function to serialize the content of this buffer to another one */
	virtual size_t serializeTo( char *data )=0;
};

}