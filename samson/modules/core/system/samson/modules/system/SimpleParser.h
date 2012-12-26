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

#ifndef _H_SAMSON_SIMPLE_PARSER
#define _H_SAMSON_SIMPLE_PARSER


#include <samson/module/samson.h>

namespace samson{
namespace system{


class SimpleParser : public samson::Parser
{

public:

    virtual void parseLine( char * line , samson::KVWriter *writer )=0;

    void run( char *data , size_t length , samson::KVWriter *writer )
    {

        size_t line_begin = 0;
        size_t offset = 0;

        while( offset < length )
        {

            if( data[offset] == '\n' || data[offset] == '\0' )
            {
                data[offset] = '\0';

                parseLine( data+line_begin , writer );
                line_begin = offset+1;
            }

            offset++;
        }

        if( line_begin < (length-1) )
        {
            data[length-1] = '\0';
            parseLine( data+line_begin , writer );
        }

    }

    static void split_in_words( char *line , std::vector<char*>& words )
    {
        split_in_words( line, words, ' ');
    }

    static void split_in_words( char *line , std::vector<char*>& words , char separator )
    {
        size_t pos = 0;
        size_t previous = 0;

        bool finish = false;


        // Clear words vector
        words.clear();

        while( !finish )
        {

            if( ( line[pos] == separator ) || ( line[pos] == '\0' ) )
            {
                if(( line[pos] == '\0' )|| (line[pos] == '\n'))
                    finish = true;

                // Artifical termination of string
                line[pos] = '\0';

                // Add the found word
                words.push_back(  &line[previous] );

                // Point to the next words
                // Jumps blank spaces
                pos++;

                // To avoid valgrind detected error when checking after the end of the buffer
                if (!finish)
                {
                    while (line[pos] == ' ')
                    {
                        pos++;
                    }
                }
                previous = pos;
            }
            else
                pos++;
        }
    }



};

} // end of namespace system
} // end of namespace samson

#endif
