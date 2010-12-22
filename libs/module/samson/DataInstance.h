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
#include <sstream>				/*std::ostringstream*/
#include <samson/var_int.h>	

namespace ss {
	
	class DataInstance 
	{
	public:
		virtual int          parse(char *data) = 0;						// Parse input buffer (return bytes read)
		virtual int          serialize(char *data)= 0;					// Serialize to a vector (returns bytes writed)
		virtual int          hash(int max_max) = 0;						// Hash function when this is used as key
		virtual std::string  str()= 0;									// Function  to get a debug-like string with the content
		virtual ~DataInstance(){}
	};
	
} // ss namespace

#endif
