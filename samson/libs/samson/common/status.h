#ifndef SAMSON_STATUS_H
#define SAMSON_STATUS_H

/* ****************************************************************************
*
* FILE                     status.h
*
* DESCRIPTION              Status values for most of samson methods
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jun 06 2011
*
*/

#include <string>
#include "au/Status.h"

namespace samson
{

    
    /* ****************************************************************************
     *
     * Status - 
     */
    
    typedef enum Status
    {
        OK,
        NotImplemented,
        
        BadMsgType,
        NullHost,
        BadHost,
        NullPort,
        Duplicated,
        KillError,
        NotHello,
        NotAck,
        NotMsg,
        
        Error,
        ConnectError,
        AcceptError,
        NotListener,
        SelectError,
        SocketError,
        GetHostByNameError,
        BindError,
        ListenError,
        ReadError,
        WriteError,
        Timeout,
        ConnectionClosed,
        PThreadError,
        WrongPacketHeader,
        
        ErrorParsingGoogleProtocolBuffers
    } Status;
    
    
    // Convert status from au::Status
    Status au_status( au::Status s );
    
    
    /* ****************************************************************************
     *
     * status - returns a textual representation of a Status value
     */
    extern const char* status(Status s);
    
    
    
}


#endif
