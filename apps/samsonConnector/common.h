

#ifndef _H_SAMSON_CONNECTOR_COMMON
#define _H_SAMSON_CONNECTOR_COMMON

#include "au/tables/Table.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "engine/BufferContainer.h"


// ----------------------------------------------------------------
#define SC_CONSOLE_PORT 5467
#define SC_WEB_PORT     9999
#define SAMSON_CONNECTOR_INTERCHANNEL_PORT 9890

// ----------------------------------------------------------------

namespace stream_connector {

    // ConnectionType
    // ------------------------------------------
    
    typedef enum
    {
        connection_input,
        connection_output
    } ConnectionType;

    // Get string for each connection type
    const char * str_ConnectionType( ConnectionType type );
    
}

#endif