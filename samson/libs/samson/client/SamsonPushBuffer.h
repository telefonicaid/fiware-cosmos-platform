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
#ifndef _H_SamsonPushBuffer_SamsonClient
#define _H_SamsonPushBuffer_SamsonClient

#include <string>
#include "au/mutex/Token.h"
#include "au/Rate.h"

#include "engine/Notification.h"

namespace  samson 
{
    
    class SamsonClient;
    
    
    /*
     Class used to push data to a txt queue in buffer mode
     */
    
    class SamsonPushBuffer
    {
        
        SamsonClient *client;
        std::string queue;
        
        // Buffer
        char *buffer;
        size_t max_buffer_size;
        
        size_t size;    // Current size of the buffer
        
        au::Token token;
        
    public:
        
        au::rate::Rate rate; // Statistics about rate
        
        SamsonPushBuffer( SamsonClient *client , std::string queue );
        ~SamsonPushBuffer();
        
        void push( const char *data , size_t length , bool flushing );
        void flush();
        
        // Recevie notifications
        void notify( engine::Notification* notification );
        
    private:
        
        void _flush();      // No token protected flush operation
        
        
    };
    
}

#endif