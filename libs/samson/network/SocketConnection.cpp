#include <stdio.h>
#include <sys/types.h>          // types needed by socket include files
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD
#include <errno.h>

#include "logMsg/logMsg.h"          // LM_X
#include "logMsg/traceLevels.h"

#include "au/TokenTaker.h"

#include "samson/common/MemoryTags.h"

#include "samson/common/status.h"

#include "samson/network/Packet.h"

#include "SocketConnection.h" // Own interface

namespace samson {
    
    // Disconnect
    void SocketConnection::close()
    {
        au::TokenTaker tt(&token);
        if( fd != -1 )
        {
            ::close(fd);
            fd = -1;
        }
    }
    
    bool SocketConnection::isDisconnected()
    {
        return ( fd == -1 );
    }
    
    Status SocketConnection::okToSend( )
    {
        int             fds;
        fd_set          wFds;
        struct timeval  timeVal;
        int             tryh;
        int             tries = 300;
        
        for (tryh = 0; tryh < tries; tryh++)
        {
            
            timeVal.tv_sec  = 1;
            timeVal.tv_usec = 0;
            
            FD_ZERO(&wFds);
            FD_SET(fd, &wFds);
            
            do
            {
                fds = select(fd + 1, NULL, &wFds, NULL, &timeVal);
            } while ((fds == -1) && (errno == EINTR));
            
            if ((fds == 1) && (FD_ISSET(fd, &wFds)))
                return OK;
            
            if( fds == -1 )
                LM_RE(Error, ("Select: %s", strerror(errno)));
            
            if( tryh > 3 )
                if( tryh%10 == 0 )
                    LM_W(("Problems to send to %s (%d/%d secs)", host.c_str() , tryh, tries ));
        }
        
        return Timeout;
    }      
    
    Status SocketConnection::partWrite( const void* dataP, int dataLen, const char* what )
    {
        int    nb;
        int    tot  = 0;
        char*  data = (char*) dataP;
        Status s;
        
        while (tot < dataLen)
        {
            if ((s = okToSend( ) ) != OK )
                LM_RE(s, ("Cannot write to '%s' (fd %d) (returning -2 as if it was a 'connection closed' ...)", host.c_str(), fd));
            
            nb = ::write(fd, &data[tot], dataLen - tot);
            if (nb == -1)
                LM_RE(WriteError, ("error writing to '%s' (fd: %d): %s", host.c_str(), fd, strerror(errno)));
            else if (nb == 0)
                LM_RE(WriteError, ("part-write written ZERO bytes to '%s' (total: %d)", host.c_str(), tot));
            
            tot += nb;
        }
        return OK;
    }
    
    Status SocketConnection::writePacket( Packet * packetP , size_t *size )
    {
        Status           s;
        Message::Header  header;

        *size =0;
        
        LM_T(LmtSocketConnection, ("SocketConnection %s: Sending Paket '%s' " , str().c_str() , packetP->str().c_str() ));
        
        //
        // Preparing header
        //
        
        memset(&header, 0, sizeof(header));
        
        header.code        = packetP->msgCode;
        header.gbufLen     = packetP->message->ByteSize();        
        header.kvDataLen   = packetP->buffer?packetP->buffer->getSize():0;

        // Set magic number
        header.setMagicNumber();
        
        //
        // Sending header
        //
        s = partWrite( &header, sizeof(header), "header");
        *size += sizeof(header);
        
        if (s != OK)
        {
            if (packetP->buffer != NULL)
                engine::MemoryManager::shared()->destroyBuffer(packetP->buffer);
            if (packetP != NULL)
                delete packetP;
            LM_RE(s, ("partWrite:header(%s): %s", host.c_str(), status(s)));
        }
        

        
        //
        // Sending Google Protocol Buffer
        //
        if ((packetP != NULL) && (packetP->message != NULL) && (packetP->message->ByteSize() != 0))
        {
            char* outputVec;
            
            outputVec = (char*) malloc(header.gbufLen + 2);
            if (outputVec == NULL)
                LM_XP(1, ("malloc(%d)", header.gbufLen));
            
            if (packetP->message->SerializeToArray(outputVec, header.gbufLen) == false)
                LM_X(1, ("SerializeToArray failed"));
            
            size_t size_message = packetP->message->ByteSize();
            s = partWrite( outputVec, size_message, "Google Protocol Buffer" );
            free(outputVec);
            if (s != OK)
                LM_RE(s, ("partWrite:GoogleProtocolBuffer(): %s", status(s)));
            *size += size_message;

        }
        
        if (packetP && (packetP->buffer != 0))
        {
            s = partWrite( packetP->buffer->getData(), packetP->buffer->getSize(), "KV data" );
            if (s != OK)
                LM_RE(s, ("partWrite returned %d and not the expected %d", s, packetP->buffer->getSize()));
            *size += packetP->buffer->getSize();
        }
        
        if (packetP != NULL)
        {
            if (packetP->buffer != NULL)
                engine::MemoryManager::shared()->destroyBuffer(packetP->buffer);
            delete packetP;
        }
        
        return OK;
    }    
    
    Status SocketConnection::msgAwait( int secs, int usecs, const char* what )
    {
        struct timeval  tv;
        struct timeval* tvP;
        int             fds;
        fd_set          rFds;
        
        do
        {
            if (secs == -1)
                tvP = NULL;
            else
            {
                tv.tv_sec  = secs;
                tv.tv_usec = usecs;
                
                tvP        = &tv;
            }
            
            FD_ZERO(&rFds);
            FD_SET(fd, &rFds);
            fds = select(fd + 1, &rFds, NULL, NULL, tvP);
        } while ((fds == -1) && (errno == EINTR));
        
        if (fds == -1)
            LM_RP(SelectError, ("select error awaiting '%s' from '%s", what, host.c_str()));
        else if (fds == 0)
        {
            //LM_RE(Timeout, ("timeout awaiting '%s' from '%s' (%d.%06d seconds)", what, host.c_str(), secs, usecs));
            return Timeout;
        }
        else if ((fds > 0) && (!FD_ISSET(fd, &rFds)))
            LM_RE(Error, ("some other fd has a read pending - this is impossible ! (awaiting '%s' from '%s')", what, host.c_str()));
        else if ((fds > 0) && (FD_ISSET(fd, &rFds)))
            return OK;
        
        LM_X(1, ("Very strange error awaiting '%s' from '%s'", what, host.c_str()));
        
        return Error;
    }
    
    
    Status SocketConnection::writeLine( const char* line )
    {
        return partWrite(line, strlen(line), "write line");
    }
    
    Status SocketConnection::readLine( char* line, size_t max_size , int max_seconds )
    {
        au::Cronometer c;
        size_t tot = 0;
        
        while (true)
        {
            // Read a byte
            Status s = partRead(line + tot , 1 , "web line character", max_seconds - c.diffTimeInSeconds() );
            
            if( s!= OK )
                return s;


            if ( ( line[tot] == '\n' ) || ( line[tot] == '\r' ) )
            {
                line[tot] = '\0';
                return OK;
            }
            
            // A new letter has been read
            tot++;
            
            // Check excesive line
            if( tot >= ( max_size - 2 ) )
                return Error;
        }
        
        
    }
    
    
    Status SocketConnection::partRead( void* vbuf, long bufLen , const char* what , int max_seconds )
    {
        ssize_t  tot = 0;
        Status   s;
        char*    buf = (char*) vbuf;

        // Cronometer to measure absolute max time
        au::Cronometer cronometer;
        
        while (tot < bufLen)
        {
            ssize_t nb;
            
            // Wait until OK to read
            do
            {
                s = msgAwait(1, 0, what); // Continous try with 1 second timeout to check max_seconds
                
                if( ( s!= OK ) && ( s!= Timeout ) )
                    return s; // Different error, just report
                
                // Report timeout if max seconds is excedded
                if( cronometer.diffTime() > max_seconds )
                    return Timeout;
                
            } while ( s!= OK );

            // Read call
            nb = read(fd, (void*) &buf[tot] , bufLen - tot);

            // Report connection closed if so
            if (nb == -1)
            {
                if (errno == EBADF)
                    LM_RE(ConnectionClosed, ("read: %s (treating as Connection Closed), expecting '%s' from %s"
                                             , strerror(errno), what, host.c_str()));
                
                LM_RE(ConnectionClosed, ("read: %s, expecting '%s' from %s", strerror(errno), what, host.c_str()));
            }
            else if (nb == 0)
                return ConnectionClosed;
            
            tot += nb;
        }
        
        return OK;
    }
    
    
    
    Status SocketConnection::readPacket( Packet * packetP , size_t *size )
    {
        Status s;
        Message::Header  header;

        LM_T(LmtSocketConnection, ("SocketConnection %s: Reading packet" , str().c_str() ));
        *size = 0;
        
        s = partRead(&header, sizeof(Message::Header), "Header" , 300 );  // Timeout 300 secs for next packet
        if (s != OK)
        {
            close();
            return s;
        }
        *size += sizeof(Message::Header);
        
        // Check header
        if ( !header.check() )
        {
            close();
            return WrongPacketHeader;
        }
        
        packetP->msgCode = header.code;
        
        if (header.gbufLen != 0)
        {
            char* dataP = (char*) calloc(1, header.gbufLen + 1);
            
            s = partRead(dataP, header.gbufLen, "Google Protocol Buffer" , 300);
            if (s != OK)
            {
                close();
                free(dataP);
                return s;
            }
            *size += header.gbufLen;

            LM_T(LmtSocketConnection, ("Read %d bytes of GOOGLE DATA from '%s'", header.gbufLen, host.c_str() ));
            
            // Decode the google protocol buffer message
            packetP->message->ParseFromArray(dataP, header.gbufLen);
            
            if ( packetP->message->IsInitialized() == false )
            {
                LM_E(("Error parsing Google Protocol Buffer of %d bytes because a message %s is not initialized!",
                         header.gbufLen, samson::Message::messageCode(header.code)));
                
                close();
                return ErrorParsingGoogleProtocolBuffers;
            }
            
            free(dataP);
        }
        
        if (header.kvDataLen != 0)
        {
            // Alloc a buffer to read buffer of data
            std::string buffer_name = au::str("Network Buffer from %s" , host.c_str() );
            packetP->buffer = engine::MemoryManager::shared()->newBuffer( buffer_name , header.kvDataLen, samson::MemoryOutputDisk  );
            
            char*  kvBuf  = packetP->buffer->getData();
            s = partRead(kvBuf, header.kvDataLen , "Key-Value Data" , 300);
            
            if (s != OK)
            {
                // Release buffer
                engine::MemoryManager::shared()->destroyBuffer( packetP->buffer );
                packetP->buffer = NULL;                
                
                close();
                return s;
            }
            LM_T(LmtSocketConnection, ("Read %d bytes of KV DATA from '%s'", header.kvDataLen, host.c_str()));
            *size += header.kvDataLen;
            
            packetP->buffer->setSize( header.kvDataLen );
        }
        
        return OK;
    }
    
    Status SocketConnection::newSocketConnection( std::string host , int port , SocketConnection** socket_connection )
    {
        int fd;
        struct hostent*     hp;
        struct sockaddr_in  peer;
        
        if ((hp = gethostbyname( host.c_str() )) == NULL )
            LM_RE(GetHostByNameError, ("gethostbyname(%s): %s", host.c_str() , strerror(errno)));
        
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            LM_RE(SocketError, ("socket: %s", strerror(errno)));
        
        memset((char*) &peer, 0, sizeof(peer));
        
        peer.sin_family      = AF_INET;
        peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
        peer.sin_port        = htons(port);
        
        LM_T(LmtSocketConnection, ("Connecting to %s at %s:%d", host.c_str(), port));

        // Try several times....
        int retries = 10;
        int tri     = 0;
        
        while (1)
        {
            if ( ::connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1 )
            {
                usleep(50000);
                if (tri > retries)
                {
                    ::close(fd);
                    LM_RE(ConnectError, ("Cannot connect to %s, port %d (even after %d retries)", host.c_str(), port, retries));
                }
                
                ++tri;
            }
            else
                break;
        }
        
        *socket_connection = new SocketConnection( fd , host, port ); 
        return OK;
    }
    
    
}