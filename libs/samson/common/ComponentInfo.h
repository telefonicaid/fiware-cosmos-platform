#ifndef COMPONENT_INFO
#define COMPONENT_INFO


namespace samson
{



/* ****************************************************************************
*
* ComponentInfo - 
*/
typedef struct ComponentInfo
{
	int        packets;
	long long  totalBytes;
	bool       connected;
} ComponentInfo;

}

#endif
