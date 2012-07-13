
#include "NodeIdentifier.h" // Own interfafe


namespace samson {

    NodeIdentifier::NodeIdentifier()
    {
        node_type = UnknownNode;
        id = (size_t)-1;
    }
    
    NodeIdentifier::NodeIdentifier( network::NodeIdentifier pb_node_identifier  )
    {
        
        switch ( pb_node_identifier.node_type() ) 
        {
            case network::NodeIdentifier_NodeType_Delilah:
                node_type = DelilahNode;
                break;
            case network::NodeIdentifier_NodeType_Worker:
                node_type = WorkerNode;
                break;
            case network::NodeIdentifier_NodeType_Unknown:
                node_type = UnknownNode;
                break;
                
            default:
                node_type = UnknownNode;
                break;
                
        }
        
        id = pb_node_identifier.id();
    }
    
    NodeIdentifier::NodeIdentifier ( ClusterNodeType _node_type , size_t _id )
    {
        node_type = _node_type;
        id = _id;
    }

    
    void NodeIdentifier::fill( network::NodeIdentifier* pb_node_identifier )
    {
        switch ( node_type ) 
        {
            case DelilahNode:
                pb_node_identifier->set_node_type( network::NodeIdentifier_NodeType_Delilah );
                break;
            case WorkerNode:
                pb_node_identifier->set_node_type( network::NodeIdentifier_NodeType_Worker );
                break;
            case UnknownNode:
                pb_node_identifier->set_node_type( network::NodeIdentifier_NodeType_Unknown );
                break;
        }            
        pb_node_identifier->set_id(id);
    }
    
    
    bool NodeIdentifier::operator==(const NodeIdentifier&  other)
    {
        if ( node_type != other.node_type )
            return false;
        if ( id != other.id )
            return false;
        
        return true;
    }
    
    std::string NodeIdentifier::str()
    {
        if( id == (size_t) -1 )
            return au::str("%s:Unknown" , ClusterNodeType2str( node_type ) );
        
        return au::str("%s:%lu" , ClusterNodeType2str( node_type ) , id );
    }
    
    std::string NodeIdentifier::getCodeName()
    {
        if( node_type == DelilahNode )
        {
            std::string code_id = au::code64_str(id);
            return au::str("%s_%s" , ClusterNodeType2str( node_type ) ,  code_id.c_str() );
        }
        else
            return au::str("%s_%lu" , ClusterNodeType2str( node_type ) , id );
        
    }
    
    bool NodeIdentifier::isDelilahOrUnknown()
    {
        switch (node_type) 
        {
            case DelilahNode: 
            case UnknownNode: 
                return true;
            case WorkerNode: 
                return false;
        }
        
        LM_X(1, ("Unknown error"));
        return false;
    }

}