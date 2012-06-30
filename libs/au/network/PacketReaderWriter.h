



#ifndef _H_AU_NETWORK_PACKET_READER_WRITER
#define _H_AU_NETWORK_PACKET_READER_WRITER

#include "au/mutex/Token.h"
#include "au/Status.h"
#include "au/Cronometer.h"
#include "au/network/FileDescriptor.h"

namespace au { namespace network {
    
    /*
     
     Note:
     
     All clases and interfaces here assume "P"...

     * is a class that derives from au::Object ( to be retained and released )
     * implemented methods au::Status read( FileDescriptor* fd )
     * implemented methods au::Status write( FileDescriptor* fd )
     
     */
    
    // Interface to read
    template <class P> // Class for the packet
    class PacketReaderInteface
    {
    public:
        virtual void process_packet( P* packet )=0;
    };
    
    template <class P> // Class for the packet
    class PacketReader : public au::Thread
    {
        
        au::FileDescriptor * fd_;
        PacketReaderInteface<P> * interface_;
        
    public:        
        
        PacketReader( std::string name , au::FileDescriptor * fd , PacketReaderInteface<P> * interface  )
        : Thread( name )
        {
            fd_ = fd;
            interface_ = interface;

            // Start me as a thread
            start_thread();
        }
        
        virtual ~PacketReader()
        {
            
        }
        virtual void run()
        {
            while( true )
            {
                if( thread_should_quit() )
                    return;
                
                if( fd_->isDisconnected() )
                    return; // End of thread since socket is disconnected
                
                P* packet = new P();
                au::Status s = packet->read(fd_);
                
                if( s != au::OK )
                {
                    // Close connection
                    fd_->close();
                    
                    return; // End of the tread
                }
                
                // Execute this receive method in the interface
                interface_->process_packet(packet);
                
                // Release this object
                packet->release();
            }
        }
        
    };
    
    template <class P> // Class for the packet
    class PacketWriter : public au::Thread
    {
        
        au::FileDescriptor * fd_;
        
        au::Token token; // Token to protect the list
        ObjectList<P> packets_;
        
        // Size accumulated to be sent
        size_t buffered_size;
    
    public:
        
        PacketWriter( std::string name , au::FileDescriptor * fd )
        : Thread( name )  , token( "PacketWriter" )
        {
            fd_ = fd;
            buffered_size =0;
            
            // Start me as a thread
            start_thread();
        }
        
        virtual ~PacketWriter()
        {
            
        }
        
        virtual void run()
        {
            while( true )
            {
                if( thread_should_quit() )
                    return;
                
                if( fd_->isDisconnected() )
                    return; // End of thread since socket is disconnected
                
                
                // Recover next packet if any
                P* packet = NULL;
                {
                    au::TokenTaker tt(&token); // Mutex protection
                    if( packets_.size() > 0 )
                        packet = packets_.front();
                }
                
                
                if( packet )
                {
                    // Recover the packet to be send ( do not remove from list )
                    au::Status s = packet->write(fd_);
                    
                    if( s != au::OK )
                        return; // End of thread since packet could not be sent
                    
                    // Release the packet from the list
                    {
                        au::TokenTaker tt(&token); // Mutex protection
                        packets_.pop_front();
                        buffered_size -= packet->getSize();
                    }
                    
                }
                else
                {
                    // TODO: This could be implemented in a better blocking way
                    usleep(100000);
                    
                }
                
            }
        }
        
        void extract_pending_packets( ObjectList<P>* packets )
        {
            while( true )
            {
                ObjectContainer<P> container;
                packets_.extract_front( container );
                
                P* p = container.getObject();
                
                if( !p )
                    return;

                // Push to the provided list
                packets->push_back( p );
            }
        }
        
        virtual void cancel_thread()
        {
            // TODO: This will wake up thread when implemented correctly using a blocking mechanism
        }
        
        void push( P * packet )
        {
            au::TokenTaker tt(&token); // Mutex protection
            packets_.push_back(packet);
            
            buffered_size += packet->getSize();
        }     
        
        size_t getBufferedSize()
        {
            return buffered_size;
        }

        
    };
    
    
    template <class P> // Class for the packet
    class PacketReaderWriter 
    {
        // Keep a pointer to fg to delete at the end
        au::FileDescriptor * fd_;
        
        PacketReader<P> *packet_reader_;
        PacketWriter<P> *packet_writer_;
        
    public:
        
        PacketReaderWriter( std::string name , au::FileDescriptor *fd , PacketReaderInteface<P>* interface )
        {
            fd_ = fd;
            packet_reader_ = new PacketReader<P>( name + "_reader" , fd , interface );
            packet_writer_ = new PacketWriter<P>( name + "_writer" , fd );
        }
        
        ~PacketReaderWriter()
        {
            // Just in case
            stop_threads();
            
            delete packet_writer_;
            delete packet_reader_;
            delete fd_;
        }
        
        void close()
        {
            fd_->close();
        }
        
        void push( P* packet )
        {
            packet_writer_->push( packet );
        }
        
        void stop_threads()
        {
            packet_reader_->stop_thread();
            packet_writer_->stop_thread();
        }
        
        bool isRunning()
        {
            if( packet_reader_->isRunning() )
                return true;
            if( packet_writer_->isRunning() )
                return true;
            
            return false;
        }
        
        bool isConnected()
        {
            return !fd_->isDisconnected();
        }
        
        size_t getOutputBufferedSize()
        {
            return packet_writer_->getBufferedSize();
        }
        
        void extract_pending_packets( ObjectList<P>* packets )
        {
            packet_writer_->extract_pending_packets( packets );
        }
        
    };
    
}} // End of namespace

#endif
