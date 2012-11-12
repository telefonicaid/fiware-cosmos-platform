/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef _H_SAMSON_NodeIdentifier
#define _H_SAMSON_NodeIdentifier


#include "logMsg/logMsg.h"               // LM_TODO()

#include "au/utils.h"
#include "au/network/FileDescriptor.h"
#include "au/Status.h"

#include "samson/common/samson.pb.h"     // google protocol buffers
#include "samson/common/EnvironmentOperations.h"        // str( network::Message* )

#include "engine/MemoryManager.h"        // MemoryManager
#include "engine/Object.h"
#include "engine/Engine.h"               // engine::Engine
#include "engine/Buffer.h"               // engine::Buffer
#include "Message.h"                     // samson::MessageType 

#include "samson/network/ClusterNodeType.h"

namespace samson
{
    
    class NodeIdentifier
    {
    public:
        
        ClusterNodeType node_type;
        size_t id;
        
        NodeIdentifier();
        NodeIdentifier( network::NodeIdentifier pb_node_identifier  );
        NodeIdentifier ( ClusterNodeType _node_type , size_t _id );
        
        void fill( network::NodeIdentifier* pb_node_identifier );
        
        bool operator==(const NodeIdentifier&  other);
        
        std::string str();
        
        std::string getCodeName();
        
        bool isDelilahOrUnknown();
        
    };
}

#endif