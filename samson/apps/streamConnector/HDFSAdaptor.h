
#ifndef _H_STREAM_CONNECTOR_HDFS_ADAPTOR
#define _H_STREAM_CONNECTOR_HDFS_ADAPTOR

#include "engine/BufferContainer.h"

#include "SingleConnectionAdaptor.h" // parent class SingleConnectionAdaptor

namespace stream_connector {
    
    
    
    
    class HDFSAdaptor : public SingleConnectionAdaptor
    {
        std::string host_;
        std::string directory_;
        
    public:
        
        HDFSAdaptor( Channel * channel , ConnectionType type , const std::string& host , const std::string& directory )
        : SingleConnectionAdaptor( channel  , type , au::str("HDFS(%s:%s)" , host.c_str() , directory.c_str() ))
        {
            // Keep host and firectory to establish connection latter
            host_ = host;
            directory_ = directory;
        }
        
        virtual void start_connection()
        {
            // Method to start the connection
            // This method is called once
            // Run background threads if necessary ( using au::ThreadManager please )
            
            LM_W(("start_connection in HDFS connections is still not defined"));
        }
        
        virtual void review_connection()
        {
            // This method is periodically called if you have to do any periodic review
            // This method has to return quickly ( no heavy operations are permitted )
            LM_W(("review in HDFS: Pending %s to be sent " , au::str(getBufferedSize(),"B").c_str() ));
        
            
            // How to extract data to be sent ( this has not to be done here )
            while ( true )
            {
                engine::BufferContainer buffer_container;
                getNextBufferToSent( &buffer_container );
                engine::Buffer* buffer = buffer_container.getBuffer();
                
                if( buffer )
                {
                    LM_W(("Sending buffer of %s" , au::str( buffer->getSize() , "B" ).c_str() ));
                }
                else
                    break;
                
            }
            
            
        }
        
        
        // Virtual methods of Connection
        virtual std::string getStatus()
        {
            return "Inform about internal state here!";
        }
        
        virtual void stop_connection()
        {
            // Stop all background threads ( and make sure they do not start again ; ))
            // Alert: Once this method return, streamConnector can remove this object
            LM_W(("stop_connection in HDFS connection is still not defined"));
        }
        
    };
    
}

#endif

