

#include "samson/network/NetworkInterface.h"

#include "DistributionInformation.h" // Own interface


namespace samson 
{
    size_t DistributionInformation::get_my_worker_id()
    {
        NodeIdentifier node_identifier = network->getMynodeIdentifier();
        if( node_identifier.node_type != WorkerNode )
            LM_X(1, ("Not possible to ask for a worker_id in a non worker element"));
        return node_identifier.id;
    }

}