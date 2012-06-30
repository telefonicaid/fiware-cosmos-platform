



#include "Channel.h"

#include "StreamConnector.h"
#include "SamsonAdaptor.h" // Own interface


namespace samson {
    namespace connector {
        
        SamsonAdaptor::SamsonAdaptor( 
                               Channel * _channel 
                               , ConnectionType _type 
                               , 
                               std::string _host 
                               , int _port 
                               , std::string _queue )
        : 
        Item( _channel 
             , _type 
             , au::str("SAMSON(%s:%d)" , _host.c_str() , _port) ) 
        {
            // Information for connection
            host = _host;
            port = _port;
            queue = _queue;
            
            // Init SamsonClien connection
            review_item();
            
        }
        
    }
}
