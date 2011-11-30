
/* ****************************************************************************
 *
 * FILE            status.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Definition of all the returned values in au library
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_STATUS
#define _H_AU_STATUS

#include "au/au_namespace.h"

NAMESPACE_BEGIN(au)

typedef enum Status
{
    OK,
    Error,
    
    Select,
    Timeout,
    
    GPB_Timeout,
    GPB_ClosedPipe,
    GPB_ReadError,
    GPB_CorruptedHeader,
    GPB_WrongReadSize,
    GPB_ReadErrorParsing,
    GPB_NotInitializedMessage,
    GPB_WriteErrorSerializing,
    GPB_WriteError,
    
} Status;

const char* status( Status code )
{
    switch (code) {
        case OK:            return "Ok";
        case Error:         return "Error";
            
        case Select:        return "Error in select call";
        case Timeout:       return "Timeout";
            
        case GPB_Timeout:               return "GPB Error: timeout";
        case GPB_ClosedPipe:            return "GPB Error: closed pipe";
        case GPB_ReadError:             return "GPB Error reading";
        case GPB_CorruptedHeader:       return "GPB Error: corrupted header";
        case GPB_WrongReadSize:         return "GPB Error: wrong read size";
        case GPB_ReadErrorParsing:      return "GPB Error: pasing google protocol message";
        case GPB_NotInitializedMessage: return "GPB Error: Not initialized google procool message ( required fields maybe )";
        case GPB_WriteErrorSerializing: return "GPB Error: serializing google procool message";
        case GPB_WriteError:            return "GPB Write Error";
    }
    
    return "Unknown";
}

NAMESPACE_END

#endif
