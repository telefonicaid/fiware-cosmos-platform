
#include <memory>

#include "au/TemporalBuffer.h"
#include "samson/zoo/common.h"
#include "ConnectionWatcherInterface.h"
#include "Connection.h" // Own interface

namespace samson { namespace zoo {
  
  Connection::Connection( const std::string& host
                         , const std::string& user
                         , const std::string& password )
  : token_("zoo::Connection")
  {
    handler_ = NULL;
    Connect( host , user , password );
  }
  
  Connection::Connection() : token_("zoo::Connection")
  {
    handler_ = NULL;
  }
  
  void Connection::RegisterWatcher( ConnectionWatcherInterface* w )
  {
    au::TokenTaker tt(&token_);
    connection_watchers_.insert( w );
  }
  
  void Connection::UnregisterWatcher( ConnectionWatcherInterface* w )
  {
    au::TokenTaker tt(&token_);
    connection_watchers_.erase( w );
  }
  
  bool Connection::IsWatcherRegistered( ConnectionWatcherInterface* w )
  {
    au::TokenTaker tt(&token_);
    return ( connection_watchers_.find( w ) != connection_watchers_.end() );
  }
  
  Connection::~Connection()
  {
    // Close connection if it was still open
    Close();
  }
  
  // Remove nodes
  int Connection::Remove( const std::string&path , int version )
  {
    au::TokenTaker tt(&token_);
    
    // Just make sure we are connected
    int rc = WaitUntilConnected(1000);
    if( rc )
      return rc;
    
    return zoo_delete(handler_, path.c_str(), version );
  }
  
  
  int Connection::Set( const std::string& path , const char* value , int value_len , int version  )
  {
    au::TokenTaker tt(&token_);
    
    // Just make sure we are connected
    int rc = WaitUntilConnected(1000);
    if( rc )
      return rc;
    
    // Create a node
    return zoo_set(handler_, path.c_str() , value , value_len,  version );
  }
  
  int Connection::Set( const std::string& path , ::google::protobuf::Message* value , int version  )
  {
    int size = value->ByteSize();
    
    // Temporal buffer to get data
    au::TemporalBuffer buffer( size );
    
    if( !value->SerializeToArray( buffer.data(), size) )
    {
      return ZC_ERROR_GPB;
    }
    
    int rc = Set(path , buffer.data() , size , version );
    return rc;
  }
  
  int Connection::Get( const std::string& path
                      , ConnectionWatcherInterface* watcher
                      , char* buffer
                      , int *buffer_len
                      , struct Stat* stat   )
  {
    au::TokenTaker tt(&token_);
    
    // Get without the buffer is just an exists call
    if( buffer == NULL )
      return Exists( path , watcher , stat );
    
    // Make sure this watcher is registered as active
    RegisterWatcher(watcher);
    
    
    return zoo_wget( handler_
                    , path.c_str()
                    ,static_watcher
                    ,get_new_watcher_caller(watcher)
                    ,buffer
                    ,buffer_len
                    , stat);
    
  }
  
  
  int Connection::Get( const std::string& path
                      , ConnectionWatcherInterface* watcher
                      , std::string& value
                      , struct Stat* stat   )
  {
    char line[1024];
    int length = sizeof(line)/sizeof(char) - 1;
    int rc = Get( path , watcher , line , &length , stat );
    
    if( rc )
      return rc;
    
    line[length]='\0';
    value = line;
    return rc;
  }
  
  int Connection::Get(  const std::string& path
                      , ConnectionWatcherInterface* watcher
                      , ::google::protobuf::Message* value
                      , struct Stat* stat
                      , int buffer_size )
  {
    au::TemporalBuffer buffer( buffer_size );
    int rc = Get( path ,watcher , buffer.data() , &buffer_size , stat );
    if( rc )
      return rc;
    
    bool r = value->ParseFromArray( buffer.data(), buffer_size );
    
    if( r )
      return 0; // OK
    else
      return ZC_ERROR_GPB;
  }
  
  int Connection::Get( const std::string& path , std::string& value  )
  {
    char line[1024];
    int length = sizeof(line)/sizeof(char) - 1;
    int rc = Get( path , line , &length );
    
    if( rc )
      return rc;
    
    line[length]='\0';
    value = line;
    return rc;
  }
  
  
  int Connection::Get( const std::string& path , char* buffer , int *buflen , struct Stat* stat  )
  {
    au::TokenTaker tt(&token_);
    
    // We are interested in getting stat(
    return zoo_get(handler_, path.c_str() , 0, buffer, buflen, stat );
  }
  
  int Connection::Get( const std::string& path , engine::BufferPointer buffer )
  {
    // Just make sure we are connected
    int rc = WaitUntilConnected(1000);
    if( rc )
      return rc;
    
    int buffer_len = buffer->getMaxSize();
    rc = Get( path , buffer->getData() , &buffer_len , NULL );
    if( !rc )
      buffer->setSize(buffer_len); // Set the real size
    return rc;
  }
  
  int Connection::Get( const std::string& path , ::google::protobuf::Message* value , int buffer_size  )
  {
    au::TemporalBuffer buffer( buffer_size );
    int rc = Get( path , buffer.data() , &buffer_size );
    if( rc )
      return rc;
    
    bool r = value->ParseFromArray( buffer.data(), buffer_size );
    if( r )
      return 0; // OK
    else
      return ZC_ERROR_GPB;
  }
  
  
  int Connection::Exists( const std::string& path , struct Stat* stat  )
  {
    au::TokenTaker tt(&token_);
    // We are interested in getting stat(
    return zoo_exists( handler_ , path.c_str() , 0 , stat );
  }
  
  int Connection::Exists( const std::string& path , ConnectionWatcherInterface* watcher , struct Stat* stat  )
  {
    au::TokenTaker tt(&token_);
    
    RegisterWatcher(watcher);
    
    // We are interested in getting stat(
    return zoo_wexists( handler_
                       , path.c_str()
                       , static_watcher
                       , get_new_watcher_caller(watcher)
                       , stat );
  }
  
  
  int Connection::WaitUntilConnected( int milliseconds )
  {
    
    if( !handler_ )
      return ZC_ERROR_NO_CONNECTION; // It will never became connected if no connection has been stablished
    
    au::Cronometer cronomter;
    while( true )
    {
      
      if( cronomter.seconds() > ((double) milliseconds / 1000.0) )
        return ZC_ERROR_CONNECTION_TIMEOUT;
      
      if( IsConnected() )
        return 0; // Already connected
      
      usleep( 100000 );
    }
  }
  
  int Connection::GetChildrens( const std::string& path, String_vector *vector )
  {
    au::TokenTaker tt(&token_);
    int rc = WaitUntilConnected(1000);
    if( rc )
      return rc;
    return zoo_get_children( handler_ , path.c_str() , 0 , vector);
  }
  
  int Connection::GetChildrens( const std::string& path , au::StringVector& childrens)
  {
    
    String_vector vector;
    init_vector( &vector );
    int rc = GetChildrens(path, &vector);
    
    if( rc )
    {
      // Some error
      free_vector(&vector);
      return rc;
    }
    
    for ( int i = 0 ; i < vector.count ; i++ )
      childrens.push_back( vector.data[i] );
    free_vector(&vector);
    
    return rc;
  }
  
  // Static C function to give watchers
  void Connection::static_watcher(zhandle_t *zzh,
                                  int type,
                                  int state,
                                  const char *path,
                                  void *watcherCtx )
  {
    
    
    if( (type == ZOO_CREATED_EVENT )
       || (type == ZOO_DELETED_EVENT )
       || (type == ZOO_CHANGED_EVENT )
       || (type == ZOO_CHILD_EVENT ) )
    {
      //ConnectionWatcherInterfaceCaller* caller = (ConnectionWatcherInterfaceCaller*)watcherCtx;
      ConnectionWatcherInterfaceCaller* caller = static_cast<ConnectionWatcherInterfaceCaller*>( watcherCtx );
      
      // Check if this watcher is in the list of active watchers
      if( caller->connection->IsWatcherRegistered( caller->connection_wacher ) )
        caller->connection_wacher->watcher(caller->connection, type, state, path);
      
      free( caller );
      return;
    }
    
    if( type == ZOO_SESSION_EVENT )
    {
      LM_W(("Zoo connection is probably disconnected"));
      return;
    }

    if( type == ZOO_NOTWATCHING_EVENT )
    {
      LM_W(("ZOO_NOTWATCHING_EVENT received. Not handeled"));
      return;
    }
    
    LM_W(("Unknown event received for a zoo::Connection %d" , type));
  }
  
  int Connection::Create( const std::string& path , int flags , const char* value , int value_len )
  {
    std::string intern_path = path; // Intern name to call the other interface
    return Create( intern_path , flags , value , value_len );
  }
  
  int Connection::Create( std::string& path , int flags , engine::BufferPointer buffer )
  {
    if( buffer != NULL || (buffer->getSize()==0) )
      return Create( path , flags , NULL , 0 );
    else
      return Create( path , flags , buffer->getData() , buffer->getSize() );
  }
  
  int Connection::Create( std::string& path , int flags , ::google::protobuf::Message* value )
  {
    if( !value->IsInitialized() )
    {
      return ZC_ERROR_GPB_NO_INITIALIZED;
    }
    
    int size = value->ByteSize();
    au::TemporalBuffer buffer( size );
    if( !value->SerializeToArray(buffer.data(), size) )
      return ZC_ERROR_GPB;
    
    int rc = Create(path , flags , buffer.data() , size );
    return rc;
  }
  
  int Connection::Create( const std::string& path , int flags , ::google::protobuf::Message* value )
  {
    int size = value->ByteSize();
    au::TemporalBuffer buffer(size);
    if( !value->SerializeToArray(buffer.data(), size) )
      return ZC_ERROR_GPB;
    
    return Create(path , flags , buffer.data() , size );
  }
  
  int Connection::Create( std::string& path , int flags , const std::string & value )
  {
    return Create( path , flags , value.c_str() , value.length() );
  }
  
  int Connection::Create( std::string& path , int flags  , const char* value , int value_len )
  {
    au::TokenTaker tt(&token_);
    
    // Just make sure we are connected
    int rc = WaitUntilConnected(1000);
    if( rc )
      return rc;
    
    char buffer[512];
    size_t buffer_length = sizeof(buffer)/sizeof(char);
    struct ACL ALL_ACL[] = {{ ZOO_PERM_ALL , ZOO_AUTH_IDS }};
    struct ACL_vector ACL_VECTOR = { 1 , ALL_ACL };
    
    // Create a node
    rc = zoo_create(handler_, path.c_str() , value , value_len , &ACL_VECTOR, flags , buffer, buffer_length-1 );
    if( !rc )
      path = buffer; // Get the new name ( it is different when flag ZOO_SEQUETIAL is used )
    return rc;
  }
  
  
  int Connection::Connect( const std::string& host , const std::string& user , const std::string& password )
  {
    au::TokenTaker tt(&token_);
    
    int rc = Connect( host );
    if( rc )
      return rc;
    
    return AddAuth( user , password );
  }
  
  int Connection::Connect( const std::string& host )
  {
    au::TokenTaker tt(&token_);
    
    // Close, just it case it was connected
    Close();
    
    // Init zookeerp
    handler_ = zookeeper_init(host.c_str(), NULL, 5000, 0, NULL, 0);
    if( handler_ )
      return WaitUntilConnected(1000);
    else
      return ZC_ERROR_CONNECTION_TIMEOUT; // Error, not possible to create handler
  }
  
  int Connection::AddAuth( const std::string& user , const std::string& password )
  {
    au::TokenTaker tt(&token_);
    
    // Just make sure we are connected
    int rc = WaitUntilConnected(1000);
    if( rc )
      return rc;
    
    std::string user_password = user + ":" + password;
    rc = zoo_add_auth( handler_ , "digest" , user_password.c_str() , user_password.length() , 0 , 0 );
    
    // If corect, just wait until connected
    if( rc == 0 )
      rc = WaitUntilConnected(1000);
    
    return rc;
  }
  
  void Connection::Close()
  {
    au::TokenTaker tt(&token_);
    
    if( handler_ )
    {
      zookeeper_close( handler_ );
      handler_ = NULL;
    }
  }
  
  bool Connection::IsConnected()
  {
    au::TokenTaker tt(&token_);
    
    if( !handler_ )
      return false;
    return( zoo_state(handler_) == ZOO_CONNECTED_STATE );
  }
  
  std::string Connection::GetStatusString()
  {
    au::TokenTaker tt(&token_);
    
    // Just make sure we are connected
    int rc = WaitUntilConnected(1000);
    if( rc )
      return "Unconnected";
    
    rc = zoo_state(handler_);
    
    if (rc == ZOO_EXPIRED_SESSION_STATE)
      return au::str("ZOO_EXPIRED_SESSION_STATE (%d)",rc);
    if (rc == ZOO_AUTH_FAILED_STATE)
      return au::str("ZOO_AUTH_FAILED_STATE (%d)",rc);
    if (rc == ZOO_CONNECTING_STATE)
      return au::str("ZOO_CONNECTING_STATE (%d)",rc);
    if (rc == ZOO_ASSOCIATING_STATE)
      return au::str("ZOO_ASSOCIATING_STATE (%d)",rc);
    if (rc == ZOO_CONNECTED_STATE)
      return "Connected";
    
    return au::str("Unknown (%d)" , rc );
  }
  
  Connection::ConnectionWatcherInterfaceCaller* Connection::get_new_watcher_caller( ConnectionWatcherInterface* watcher )
  {
    Connection::ConnectionWatcherInterfaceCaller* caller = \
    (ConnectionWatcherInterfaceCaller*) malloc( sizeof(ConnectionWatcherInterfaceCaller) );
    
    caller->connection = this;
    caller->connection_wacher = watcher;
    
    return caller;
  }
  
  
}}