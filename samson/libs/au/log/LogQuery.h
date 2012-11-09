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

#include "au/log/Log.h"
#include "au/log/LogFilter.h"
#include "au/network/SocketConnection.h"
#include "au/string/Descriptors.h"

namespace au {
  
  /*
   
   Class to accumulate logs based on a provided filter
   
   */
  
  class LogQuery
  {
  public:
    
    LogQuery( const std::string& filter_definition ){
      au::ErrorManager error;// Ignore error
      log_filter_ = LogFilter::Create(filter_definition, error);
    }
    
    ~LogQuery(){
    }
    
    virtual void Process( LogPointer log )
    {
      if( log_filter_ != NULL )
        if ( !log_filter_->Accept(log) )
          return;
      logs_.push_back( log );
    }
    
    virtual void Emit( au::SocketConnection *socket_connection )
    {
      
      for ( size_t i =  0 ; i < logs_.size() ; i++ )
        if( !logs_[i]->Write(socket_connection) )
          return;
    }
    
  private:
    
    bool count_; // Flag to only count
    std::vector< LogPointer> logs_;
    au::LogFilterPointer log_filter_;
    
  };
  
  
}