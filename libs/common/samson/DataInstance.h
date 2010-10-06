#ifndef SAMSON_DATA_INSTANCE_H
#define SAMSON_DATA_INSTANCE_H

/* ****************************************************************************
*
* FILE                     DataInstance.h - 
*
* Data used to parse and serialize content
*
*
*/
#include <string>                /* std::string                              */



namespace ss {
	class DataInstance 
	{
	public:
		virtual int          parse(char *data)                    = 0; // Parse input buffer (return bytes read)
		virtual int          serialize(char *data)                = 0; // Serialize to a vector (returns bytes writed)
		virtual int          getPartition(int max_num_partitions) = 0; // Hash function when this is used as key
		virtual std::string  str()                                = 0; // Function  to get a debug-like string with the content
	};
} // ss namespace

#endif
