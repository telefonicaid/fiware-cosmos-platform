
#include "samson/common/ports.h"
#include "ClusterInformation.h" // Ow interface

namespace samson {


    ClusterNode::ClusterNode( std::string _host , int _port , size_t _id )
    {
        host = _host;            
        port = _port;
        
        id = _id;
    }
    
    ClusterNode::ClusterNode( ClusterNode* other )
    {
        host = other->host;
        id = other->id;
        port = other->port;
    }
    
    void ClusterNode::fill( network::ClusterNode* pb_cluster_node )
    {
        pb_cluster_node->set_ip(host);
        pb_cluster_node->set_port(port);
        pb_cluster_node->set_id(id);
    }
    
    void ClusterNode::getInfo( ::std::ostringstream& output , ::std::string format)
    {
        if (format == "xml")
        {
            au::xml_open(output, "cluster_node");
        
            au::xml_simple(output, "id", id);
            au::xml_simple(output, "host", host);
            au::xml_simple(output, "port", port);
            
            au::xml_close(output, "cluster_node");
        }
        else // json ...
        {
            output << "    \"cluster_node\":\r\n";
            output << "    {\r\n";
            output << "      \"id\"   : \"" << id   << "\",\r\n";
            output << "      \"host\" : \"" << host << "\",\r\n";
            output << "      \"port\" : \"" << port << "\"\r\n";
            output << "    }";
        }
    }
    

    
    std::string ClusterNode::str()
    {
        std::ostringstream output;
        output << au::str("  NODE %s:%d (id %lu)" , host.c_str() , port , id );
        return output.str();
    }
    
    NodeIdentifier ClusterNode::getNodeIdentifier()
    {
        return NodeIdentifier( WorkerNode , id );            
    }
    
    
    
#pragma mark ClusterInformaiton
    
    ClusterInformation::ClusterInformation() : token("ClusterInformation")
    {
        clearClusterInformation();
    }
    
    ClusterInformation::ClusterInformation ( network::ClusterInformation pb_cluster_information ) : token("ClusterInformation")
    {
        clearClusterInformation();
        parse( &pb_cluster_information );            
    }
    
    ClusterInformation::~ClusterInformation()
    {
        LM_T(LmtCleanup, ("In destructor, calling clearClusterInformation"));
        clearClusterInformation();
    }
    
    void ClusterInformation::clearClusterInformation()
    {
        id = 0;
        version = 0;
        au::TokenTaker tt (&token);
        
        LM_T(LmtCleanup, ("nodes.size == %d", nodes.size()));
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
        {
           LM_T(LmtCleanup, ("delete node %d", i));
           delete nodes[i];
        }
        nodes.clear();
    }
    
    
    void ClusterInformation::init_new_cluster( std::string host , int port )
    {
        LM_T(LmtCleanup, ("In init_new_cluster, calling clearClusterInformation"));
        // Clear previous information ( if any )
        clearClusterInformation();
        
        // New random identifier
        id = ( (size_t) (rand()%1000000) ) * 1000000 + ( (size_t) (rand()%1000000) );
        version = 0;
        
        LM_T(LmtCleanup, ("In init_new_cluster, adds single worker:%s", host.c_str()));
        // Add a single worker ( id = 0 )
        au::TokenTaker tt (&token);
        nodes.push_back( new ClusterNode( host , port , 0 ) );
        
    }
    
    size_t ClusterInformation::add_node( std::string host , int port )
    {
        // Update the version
        version++;
        
        au::TokenTaker tt (&token);
        size_t assigned_id = 0;
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
            if( assigned_id <= nodes[i]->id )
                assigned_id = nodes[i]->id+1;
        
        LM_T(LmtCleanup, ("In add_node, adds single worker:%s", host.c_str()));
        nodes.push_back( new ClusterNode( host , port , assigned_id ) );
        
        return assigned_id;
    }
    
    bool ClusterInformation::remove_host( size_t worker_id )
    {
        bool id_found = false;
        
        std::vector<ClusterNode*> _nodes;   
        
        au::TokenTaker tt (&token);
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
        {
            if( nodes[i]->id == worker_id )
            {
                delete nodes[i];
                id_found = true;
            }
            else
                _nodes.push_back( nodes[i] );
        }
        
        if ( ! id_found )
            return false;
        
        // Update the version
        version++;
        
        LM_T(LmtCleanup, ("In remove_host, removes single worker_id:%lu", worker_id));

        // construct the vector without the affected element
        nodes.clear();
        for ( size_t i = 0 ; i < _nodes.size() ; i++ )
            nodes.push_back(_nodes[i]);
        
        return true;
        
    }
    
    // Fill to be included in a message
    void ClusterInformation::fill( network::ClusterInformation* pb_cluster_information )
    {
        au::TokenTaker tt(&token); // Mutex protection
        
        pb_cluster_information->set_id(id);
        pb_cluster_information->set_version(version);
        
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
        {
            network::ClusterNode * pb_cluster_node = pb_cluster_information->add_nodes();
            nodes[i]->fill( pb_cluster_node );
        }
    }
    
    void ClusterInformation::parse( network::ClusterInformation* pb_cluster_information )
    {
        au::TokenTaker tt(&token); // Mutex protection
        
        id = pb_cluster_information->id();
        version = pb_cluster_information->version();
        
        for (int i = 0 ; i < pb_cluster_information->nodes_size() ; i++ )
        {
            std::string ip = pb_cluster_information->nodes(i).ip();
            int port = pb_cluster_information->nodes(i).port();
            size_t id = pb_cluster_information->nodes(i).id();
            
            nodes.push_back( new ClusterNode( ip , port , id ) );
        }
        
    }
    
    size_t ClusterInformation::getId()
    {
        return id;
    }
    size_t ClusterInformation::getVersion()
    {
        return version;
    }
    
    size_t ClusterInformation::getNumNodes()
    {
        au::TokenTaker tt (&token);
        return nodes.size();
    }
    
    size_t ClusterInformation::getAssignedId()
    {
        return assigned_id;
    }
    
    void ClusterInformation::setAssignedId( size_t _assigned_id )
    {
        assigned_id = _assigned_id;
    }
    
    void ClusterInformation::update( ClusterInformation* new_cluster_information )
    {

        if ( id != 0 )
        {
            if( id != new_cluster_information->id )
                LM_X(1, ("SAMSON Protocol error"));
            
            if( version >= new_cluster_information->version )
                LM_X(1, ("SAMSON Protocol error"));
            
        }
        
        // Clear previous information
        clearClusterInformation();
        
        id = new_cluster_information->id;
        version = new_cluster_information->version;
        
        au::TokenTaker tt(&token); // Mutex protection
        for ( size_t i = 0 ; i < new_cluster_information->nodes.size() ; i++ )
            nodes.push_back( new ClusterNode( new_cluster_information->nodes[i] )) ;
        
    }
    
    void ClusterInformation::remove_file()
    {
        std::string fileName =  SamsonSetup::shared()->clusterInformationFileName();
        remove( fileName.c_str() );
    }
    
    // Save contents to file
    void ClusterInformation::save(  au::ErrorManager * error )
    {
        std::string fileName =  SamsonSetup::shared()->clusterInformationFileName();
        
        au::TokenTaker tt(&token); // Mutex protection
        
        FILE* file = fopen( fileName.c_str() , "w" );
        if ( !file )
        {
            error->set( au::str("Not possible to open file %s" , fileName.c_str() ) );
            return;
        }
        
        std::ostringstream output;
        output << "cluster " << id << "\n";
        output << "version " << version << "\n";
        output << "assigned_id " << assigned_id << "\n";
        
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
            output << "node " << nodes[i]->host << " " << nodes[i]->port << " " << nodes[i]->id << "\n";
        
        std::string content = output.str();
        
        if( fwrite( content.c_str() , content.length(), 1,  file ) != 1 )
            error->set( au::str( "Error writing %lu bytes to file %s" , content.length() , fileName.c_str() ) );
        
        fclose(file);
        
    }
    
    // Load content to file
    void ClusterInformation::load( au::ErrorManager * error )
    {
        // Clear previous information if any
        clearClusterInformation();
        
        std::string fileName =  SamsonSetup::shared()->clusterInformationFileName();
        
        // Read file...
        FILE * file = fopen( fileName.c_str() , "r");
        if( !file )
        {
            error->set( au::str("Not possible to open file %s" , fileName.c_str() ) );
            return;
        }
        
        char line[1024];
        
        while( fgets(line, sizeof(line), file) )
        {
            au::CommandLine cmdLine;
            cmdLine.parse( line );
            
            if ( cmdLine.get_num_arguments() < 2 )
                continue;
            
            std::string command = cmdLine.get_argument(0);
            
            if( command == "cluster" )
                id = atoll( cmdLine.get_argument(1).c_str() ); 
            if( command == "version" )
                version = atoll( cmdLine.get_argument(1).c_str() ); 
            if( command == "assigned_id" )
                assigned_id = atoll( cmdLine.get_argument(1).c_str() ); 
            
            if( command == "node" )
            {
                au::TokenTaker tt (&token);
                if( cmdLine.get_num_arguments() >= 4 )
                {
                    std::string host = cmdLine.get_argument(1).c_str();
                    int port = atoi( cmdLine.get_argument(2).c_str() );
                    size_t id = atoll( cmdLine.get_argument(3).c_str() );
                    
                    nodes.push_back( new ClusterNode( host , port , id ) );
                    
                }
            }
        }

        fclose(file);
    }
    
    std::vector<size_t> ClusterInformation::getWorkerIds()
    {
        std::vector<size_t> ids;
        au::TokenTaker tt (&token);
        
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
            ids.push_back( nodes[i]->id );
        
        return ids;
    }
    
    std::string ClusterInformation::str_identifier()
    {
        return au::str("%lu:%lu" , id , version );
    }
    
    
    std::string ClusterInformation::str()
    {
        std::ostringstream output;
        
        output << "------------------------------------------------\n";
        output << "Cluster Information [ id " << id << " version " << version << " ]\n";
        output << "------------------------------------------------\n";
        au::TokenTaker tt (&token);
        for ( size_t i = 0 ; i < nodes.size() ; i++ )
            output << nodes[i]->str() << "\n";
        output << "------------------------------------------------\n";
        
        return output.str();
        
    }
    
    au::vector<ClusterNode> ClusterInformation::getNodesToConnect( NodeIdentifier node_identifier )
    {
        au::TokenTaker tt (&token);
        au::vector<ClusterNode> _nodes;   // Information for all nodes
        
        
        // Delilah should connect to all workers
        if( node_identifier.node_type == DelilahNode )
            for ( size_t i = 0 ; i < nodes.size() ; i ++)
                _nodes.push_back( new ClusterNode( nodes[i] ) );
        
        // Workers should connect to all the workers latter than itself
        bool self_found = false;
        if( node_identifier.node_type == WorkerNode )
            for ( size_t i = 0 ; i < nodes.size() ; i ++)
            {
                if( nodes[i]->id == assigned_id )
                    self_found = true;
                else if ( self_found )
                    _nodes.push_back( new ClusterNode( nodes[i] ) );
            }
        
        return _nodes;
    }

    au::vector<ClusterNode> ClusterInformation::getNodes( )
    {
        au::TokenTaker tt (&token);
        au::vector<ClusterNode> _nodes;   // Information for all nodes
                
        for ( size_t i = 0 ; i < nodes.size() ; i ++)
            _nodes.push_back( new ClusterNode( nodes[i] ) );
        
        return _nodes;
    }
    
    
    void ClusterInformation::getInfo( ::std::ostringstream& output , ::std::string format)
    {
        au::TokenTaker tt (&token);

        if (format == "xml")
        {
            au::xml_open(output, "cluster_information");
        
            au::xml_simple(output, "id", id);
            au::xml_simple(output, "version", version);
        }
        else
        {
            output << "  \"cluster_information\":\r\n";
            output << "  {\r\n";
            output << "    \"id\"      : " << id << ",\r\n";

            if (nodes.size() == 0)
                output << "    \"version\" : " << version << "\r\n";
            else
                output << "    \"version\" : " << version << ",\r\n";
        }

        for ( size_t i = 0 ; i < nodes.size() ; i++ )
        {
            nodes[i]->getInfo(output, format);
            if (format == "json")
            {
                if (i != (nodes.size() - 1))
                    output << ",";
                output << "\r\n";
            }
        }

        if (format == "xml")
            au::xml_close(output, "cluster_information");
        else
            output << "  }\r\n";
    }
    
    std::string ClusterInformation::hostForWorker( size_t worker_id)
    {
        au::TokenTaker tt (&token);
        for ( size_t i = 0 ; i < nodes.size() ; i ++)
            if( nodes[i]->id == worker_id )
                return nodes[i]->host;
        return "unknown_host";
    }

    unsigned short ClusterInformation::portForWorker( size_t worker_id)
    {
        au::TokenTaker tt(&token);

        for (size_t i = 0; i < nodes.size(); i ++)
        {
            if (nodes[i]->id == worker_id)
                return nodes[i]->port;
        }

        return 0xFFFF;
    }
    
    size_t ClusterInformation::getIdForWorker( std::string host )
    {
        // host or host:port are accepted
        au::TokenTaker tt(&token);

        std::string lookup_host = host;
        int lookup_port = SAMSON_WORKER_PORT;
        
        size_t p = host.find(":");
        if( p != std::string::npos )
        {
            lookup_host = host.substr( 0 , p );
            lookup_port = atoi( host.substr( p + 1 ).c_str() );
        }

        for (size_t i = 0 ; i <nodes.size() ; i++ )
            if( nodes[i]->host == lookup_host )
                if( nodes[i]->port == lookup_port )
                    return nodes[i]->id;

        return -1;
    }

    
}
