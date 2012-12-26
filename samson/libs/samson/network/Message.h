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
#ifndef SAMSON_MESSAGE_H
#define SAMSON_MESSAGE_H

#include <cstring>

/* ****************************************************************************
*
* FILE                     Message.h - message definitions for all Samson IPC
*
*/

namespace samson
{
namespace Message
{



/* ****************************************************************************
*
* CODE - Legible message codes and types
*/
#define CODE(c1, c2, c3, c4) ((c4 << 24) + (c3 << 16) + (c2 << 8) + c1)
	
/* ****************************************************************************
*
* MessageCode
*/
typedef enum MessageCode
{
	Hello							= CODE('H', 'e', 'l', 'l'), 

    NetworkNotification             = CODE('N', 'o', 't', 'i'),
    
    StatusReport					= CODE('S', 'R', 'p', ' '),
    
    Alert                           = CODE('A', 'l', 'e', 'r'),

    WorkerCommand                   = CODE('W', 'C', '-', ' '),
    WorkerCommandResponse           = CODE('W', 'C', 'R', ' '),

    PushBlock                       = CODE('P', 'B', 'l', ' '),
    PushBlockResponse               = CODE('P', 'B', 'r', ' '),
    PopQueue                        = CODE('P', 'Q', 'r', ' '),
    PopQueueResponse                = CODE('P', 'Q', 'R', ' '),
    
    StreamOutQueue                  = CODE('S', 'O', 'Q', ' '),
    
	Message							= CODE('M', 'e', 's', 'g'),
    
    Unknown                         = CODE('U', 'n', 'k', ' ')
} MessageCode;



/* ****************************************************************************
*
* Header - 
*/
typedef struct Header
{
	size_t magic;
	MessageCode    code;
	size_t gbufLen;
	size_t kvDataLen;
    
    bool check()
    {
        if( magic != 4050769273219470657 )
            return false;
        
        if( gbufLen > 10000000 )
            return false;
        
        if( kvDataLen > ( 200 * 1024 * 1024 ) )
            return false;
        
        return true;
    }
    
    void setMagicNumber()
    {
        magic = 4050769273219470657;
    }
    
} Header;


/* ****************************************************************************
*
* messageCode - 
*/
extern char* messageCode(MessageCode code);

}
}

#endif
