/* *******************************************************************************
*
* FILE                     Packet.cpp
*
* DESCRIPTION              Definition of the packet to be exchange in the samson-ecosystem
*
*/
#include "Packet.h"             // Own interface

namespace samson
{
    
    const char* ClusterNodeType2str( ClusterNodeType type )
    {
        switch ( type ) 
        {
            case DelilahNode: return "delilah";
            case WorkerNode:  return "worker";
            case UnknownNode: return "unknown";
        }
        
        LM_X(1, ("Impossible to got here"));
        return "Error";
    }

}
