
/* ****************************************************************************
 *
 * FILE            StringComponent
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Class used as a a pool of instances of particular class
 *
 * COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#ifndef _H_AU_STRING_COMPONENT
#define _H_AU_STRING_COMPONENT

#include <cstring>
#include <string>    // std::String
#include <time.h>
#include <list>
#include <math.h>

#include "au/xml.h"
#include "au/Cronometer.h"
#include "au/string.h"

#include "au/namespace.h"


namespace au 
{
    class StringComponents
    {
        char *internal_line;
        size_t max_size;
        
    public:
        
        // Components of the last split
        std::vector<char*> components;
        
        StringComponents()
        {
            max_size = 1024;
            internal_line = (char*) malloc( max_size );
        }
        
        ~StringComponents()
        {
            if( internal_line )
                free( internal_line );
        }

        size_t process_line( const char * line , size_t max_length , char separator )
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
        
        
    };
    
}

#endif