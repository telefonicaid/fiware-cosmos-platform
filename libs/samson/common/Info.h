
/* ****************************************************************************
 *
 * FILE            Info.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            7/11/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_Info
#define _H_Samson_Info


#include "au/Environment.h"                 // au::Environment
#include "au/map.h"                         // au::map

#include "samson/common/samson.pb.h"        // network::...

namespace samson {
    
    class Info;
    class InfoCollection;
    
    typedef std::vector<std::string> PathComponents;

    class ValuesCollection : public std::vector< std::string >
    {
        
    public:
        
        
        void add( std::string value )
        {
            push_back( value );
        }
        
        size_t sumSizeT()
        {
            size_t total = 0;
            for (size_t i = 0 ; i < size() ; i++)
                total += atoll( (*this)[i].c_str() );
            return total;
        }
        
    };
    
    
    class Info
    {
        
        std::string value;                      // Value of this node
        
        au::map<std::string , Info > sub;       // Children nodes
                
	public:

        Info(std::string value);
        Info();
        ~Info();

        // Set the value
        void set( std::string property , std::string value );
        
        // Spetial cases with numbers
        template <typename T>
        void set( std::string property , T _value )
        {
            std::ostringstream str_value;
            str_value << _value;
            
            set( property , new Info( str_value.str() ) );
            
        }
        
        // Set a children
        void set( std::string property , Info* info );
        
        // Function to get a particular children full path
        Info* get( std::string path );
        
        // Duplicate an info-tree following a particular path
        ValuesCollection getValues( std::string path );

        // Get a string with all the content
        std::string str( );
        
    public:
        
        // Clear content of this info structure
        void clear();
        
    private:
     
        // Internal recursice function to get a particular field
        Info* get( std::vector<std::string> &components , int pos );
        
        // Internal recursive function to get value
        void getValues( PathComponents & pc , int pos , ValuesCollection& vc );
        
        // Internal recursive function to compute the string
        std::string str( std::string top , int pos );
        
    public:
        
        // Function to interact with the network message
        void fill( std::string name , network::Info *info );
        void get( const network::Info& info); 

    private:
        
        void static getPathComponents( std::string path , PathComponents& components );
        
    };


    
}
#endif
