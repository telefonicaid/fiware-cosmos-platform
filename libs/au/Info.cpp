
/* ****************************************************************************
 *
 * FILE            Info .cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            7/11/11
 *
 * DESCRIPTION
 *
 *  Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#include <iostream>                     // std::stringstream
#include <string>                       // std::getline

#include "au/Format.h"                  // au::Format::indent

#include "Info.h"                       // Own interface



size_t findArgumentSeparatorPosition( std::string txt )
{
    size_t pos = std::string::npos;
    
    const char* str = txt.c_str();
    
    int level = 0 ;
    for ( size_t i = 0 ; i < txt.length() ; i++ )
    {
        if( ( level == 0 ) && ( str[i]==',' ) )
            return i;
        
        if( str[i] == '(' )
            level++;
        
        if( str[i] == ')' )
            level--;
        
        if( level < 0 )
            return std::string::npos;
        
    }
    
    return pos;
}


namespace au {

    Info::Info(  )
    {
        name = "no-name";
    }
    
    Info::Info( std::string _name )
    {
        name = _name;
    }
    
    Info::Info( std::string _name , std::string children_name  )
    {
        name = _name;
        set( children_name );
    }
    
    
    Info::~Info()
    {
        clear();
    }
    
    void Info::clear()
    {
        sub.clearMap();         // Clear the children nodes
    }

    void Info::set( std::string property , std::string value )
    {
        get(property)->set( value );        
    }
    
    void Info::set( std::string property , Info* info )
    {
        Info * previous = sub.extractFromMap( property );
        if( previous )
            delete previous;
     
        // Set the name for the children
        info->name = property;
        
        sub.insertInMap( property , info );
    }
    
    void Info::set( std::string value )
    {
        get( value );
    }
    
    
    Info *Info::clone( )
    {
        Info *tmp = new Info( name );
        
        au::map<std::string , Info >::iterator s;
        for (s = sub.begin() ; s != sub.end() ; s++)
        {
            std::string name = s->first;
            tmp->sub.insertInMap( name , s->second->clone() );
        }
        
        return tmp;
    }

    Info* Info::match( Info* path )
    {
        Info *tmp = new Info("results");
        _match( path , tmp );
        return tmp;
    }
    
    void Info::_match( Info* path , Info* results )
    {
        au::map<std::string , Info >::iterator s_path;
        
        for ( s_path = path->sub.begin() ; s_path != path->sub.end() ; s_path++ )
        {
            std::string s_path_name = s_path->first;
            Info* s_path_info       = s_path->second;
            
            if( ( s_path_name == "*" ) || (s_path_name == name) )
            {
                if( s_path_info->sub.size() == 0 )
                    results->add_result( clone() );
                else
                {
                    // Add all my childrens..
                    au::map<std::string , Info >::iterator s;
                    for ( s = sub.begin() ; s != sub.end() ; s++ )
                    {
                        s->second->_match( s_path_info, results );
                    }
                }
            }
        }
    }
    
    
    
    std::string Info::str(  )
    {
        return str( 0 );
    }
    
    std::string Info::str( int level )
    {
        std::ostringstream output;

        // Value for this property
        for (int i = 0 ; i < level ; i++ )
            output << "  ";

        output << name << "\n";
        
        // Children elements
        au::map<std::string , Info >::iterator s;
        for (s = sub.begin() ; s != sub.end() ; s++)
            output << s->second->str( level + 1 );
        
        return output.str();
    }


    void Info::addPath( std::string path )
    {
        size_t pos = path.find(".");
        
        if( pos == std::string::npos )
        {
            // Create a children with name "path"
            get(path);
        }
        else
        {
            std::string name = path.substr( 0 , pos );
            std::string rest_path = path.substr( pos+1 , path.length() - (pos+1) );

            get(name)->addPath( rest_path );
        }

    }
    
    void Info::addQuery( std::string query )
    {
        size_t pos_begin    = query.find_first_of("(");
        size_t pos_end      = query.find_last_of(")");

        // Nothing to do
        if( ( pos_begin == std::string::npos ) || (pos_end == std::string::npos) )
        {
            set("command" , "select");
            Info *path = new Info();
            path->addPath( query ); 
            set("argument_0",path);
            return;
        }
        else
        {
            set("command", query.substr( 0 , pos_begin ));

            std::string sub_query = query.substr(pos_begin+1 , pos_end - pos_begin -1 );
            
            
            // Separated by commas
            int num_argument = 0 ;

            while( sub_query.length() > 0 )
            {
                size_t pos_argumen = findArgumentSeparatorPosition( sub_query ); 
                
                if( pos_argumen == std::string::npos )
                {
                    Info *argument = new Info();
                    argument->addQuery(sub_query);
                    set( au::Format::string( "argument_%d" , num_argument++) , argument);
                    sub_query = "";
                }
                else
                {
                    std::string sub_sub_query = sub_query.substr( 0 , pos_argumen );
                    std::string rest_sub_query = sub_query.substr( pos_argumen+1 , sub_query.length() - (pos_argumen+1) );
                    
                    Info *argument = new Info();
                    argument->addQuery(sub_sub_query);
                    set( au::Format::string( "argument_%d" , num_argument++) , argument);
                    
                    // Keep the rest to continue
                    sub_query = rest_sub_query;
                    
                }
            }            
                
        }
        
    }
    
    
    Info* Info::query( std::string query )
    {
        Info * info_query = new Info("query");
        info_query->addQuery( query );
        
        Info *info_result = runQuery( info_query );
        
        delete info_query;
        
        return info_result;
    }
    
    Info* Info::runQuery( Info *query )
    {
        std::string operation = query->get("command")->get(); 

        // Select query ( just select the macth elements )
        if( operation == "select" )
        {
            Info* path = query->get("argument_0");
            
            // Return a set of results that match pattern
            return match( path );
        }
        
        if( operation == "sum" )
        {

            Info* sub_query = query->get("argument_0");

            Info* result_sub_query = runQuery(sub_query);
            
            size_t total = 0 ;
            
            au::map<std::string , Info >::iterator s;
            
            for ( s = result_sub_query->sub.begin() ; s != result_sub_query->sub.end() ; s++ )
            {
                std::string value = s->second->get();
                total += atoll( value.c_str() ); 
            }
            
            delete result_sub_query;

            // Result a unique value with the total sum
            Info *results_info  = new Info("results");
            results_info->add_result( new Info("res_0" , au::Format::string("%lu", total) ) );
            return results_info;
        }
        
        if( operation == "cut" )
        {
            
            Info * sub_query = query->get("argument_0");

            int level = 1;  // Default value

            if( query->isDefined("argument_1") )
            {
                Info * levels_info = query->get("argument_1");
                level = atoi( levels_info->get("argument_0")->get().c_str() );
            }
            
            Info* result_sub_query = runQuery(sub_query);

            result_sub_query->cut( level );
            
            return result_sub_query;
        }
        
        
        
        Info *info = new Info("error");
        info->set( au::Format::string("Unkown command '%s'" , operation.c_str() ));
        return info;
        
    }
    
    void Info::add_result( Info*tmp )
    {
        set( au::Format::string("result_%lu" , sub.size() ) , tmp );
    }
    
    
    
    
}
