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

#include "StringComponents.h" // Own interface


namespace au 
{
    
    StringComponents::StringComponents()
    {
        max_size = 1024;
        internal_line = (char*) malloc( max_size );
    }
    
    StringComponents::~StringComponents()
    {
        if( internal_line )
            free( internal_line );
    }
    
    size_t StringComponents::process_line( const char * line , size_t max_length , char separator )
    {
        // Alloc more space if necessary
        if( ( max_length + 1 )  > max_size )
        {
            if( internal_line)
                free( internal_line );
            
            while( max_size < ( max_length+1 ) )
                max_size *= 2;
            
            // Alloc new space
            internal_line = (char*) malloc( max_size );
        }
        
        
        components.clear();
        if( max_length == 0 )
            return 0;
        
        bool new_component = true;
        
        for ( size_t i = 0 ; i < max_length ; i++ )
        {
            internal_line[i] = line[i];
            
            if( new_component )
            {
                new_component = false;
                components.push_back( &internal_line[i] );
            }
            
            // End of line string
            if( internal_line[i] == '\0' )
                return i+1;
            
            // End of line string
            if( internal_line[i] == '\n' )
            {
                internal_line[i] = '\0';
                return i+1;
            }
            
            // Separator found
            if( internal_line[i] == separator )
            {
                internal_line[i] = '\0';
                new_component = true;
            }
            
        }
        
        internal_line[max_length] = '\0';
        return max_length;
        
    }

}