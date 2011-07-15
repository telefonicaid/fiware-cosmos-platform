
/* ****************************************************************************
 *
 * FILE            Info.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            7/11/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_Info
#define _H_AU_Info


#include <sstream>
#include "au/map.h"                         // au::map


namespace au {
    
    class Info
    {
        
        std::string name;
        au::map<std::string , Info > sub;       // Children nodes
        
	public:
        

        // Constructor and destructor
        Info( std::string name , std::string children_name  );
        Info( std::string name );
        Info( );
        ~Info();

        // --------------------------------------------------------------------------------------------
        // SET VALUE FUNCTION
        // --------------------------------------------------------------------------------------------
        
        // Main function to set a value
        void set( std::string value );
        
        // Set the value
        void set( std::string property , std::string value );
        
        // Spetial cases with numbers
        template <typename T>
        void set( std::string property , T _value )
        {
            std::ostringstream str_value;
            str_value << _value;
    
            set( property , str_value.str() );
        }

        void set( std::string property , Info *info );
        
        // --------------------------------------------------------------------------------------------
        // GET VALUE FUNCTION
        // --------------------------------------------------------------------------------------------
        
        size_t getNumChildrens()
        {
            return sub.size();
        }
        
        void getChildrens( std::set<std::string>& childrens )
        {
            sub.getKeys( childrens );
        }
        
        // Access a particular child
        Info* get( std::string _name )
        {
            Info *tmp = sub.findInMap(_name);
            if( !tmp )
            {
                tmp = new Info( _name );
                sub.insertInMap(_name , tmp );
            }
            
            return tmp;
        }
        
        std::string get()
        {
            // Get the value of the first child if any
            if( sub.size() > 0 )
                return sub.begin()->first;
            else
                return "";
        }
        
        void cut( int level )
        {
            if( level == -1 )
                sub.clearMap();
            else
            {
                au::map<std::string , Info >::iterator s;
                for ( s = sub.begin() ; s != sub.end() ; s++ )
                    s->second->cut( level-1 );
            }
        }
        
        
        bool isDefined( std::string name )
        {
            return (sub.findInMap(name) != NULL );
        }
        
        // --------------------------------------------------------------------------------------------
        // QUERY METHODS
        // --------------------------------------------------------------------------------------------
        
        // Get query over the tree
        Info* query( std::string path );
        
        Info* runQuery( Info *query );
        
        // --------------------------------------------------------------------------------------------
        
        
        // Duplicate me and my childs
        Info *clone( );
        
        // Get a collection of elements matching this path
        Info* match( Info* path );
        void _match( Info* path , Info* results );
        
        // Get a string with all the content
        std::string str( );
        
    public:
        
        // Clear content of this info structure
        void clear();
        
    private:
        
        // Internal recursive function to compute the string
        std::string str( int pos );
        
    public:
        
        // Construct a Info structure from a string
        void addPath( std::string path );

        // Construct a query from string
        void addQuery( std::string query );
        
    private:
        
        void add_result( Info*tmp );
        
        
    };


    
}
#endif
