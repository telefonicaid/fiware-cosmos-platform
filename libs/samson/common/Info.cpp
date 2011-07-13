
/* ****************************************************************************
 *
 * FILE            Info .cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
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

namespace samson {

    Info::Info( std::string _value )
    {
        value = _value;
    }
    
    Info::Info()
    {
    }
    
    Info::~Info()
    {
        clear();
    }
    
    void Info::clear()
    {
        value = "";             // Clear the value
        sub.clearMap();         // Clear the children nodes
    }

    void Info::set( std::string property , std::string value )
    {
        set( property , new Info( value ) );
        
    }
    
    void Info::set( std::string property , Info* info )
    {
        Info * previous = sub.extractFromMap( property );
        if( previous )
            delete previous;
        
        sub.insertInMap( property , info );
    }
    
    
    Info* Info::get( std::string path )
    {
        // Compote the path components
        PathComponents pc;
        getPathComponents( path , pc );
     
        // Recursive component
        return get( pc , 0 );
    }
    
    Info* Info::get( PathComponents& pc , int pos )
    {
        if( pos == (int)pc.size() )
            return this;
        
        Info *info = sub.findInMap( pc[pos] );
        
        if( !info )
        {
            // Create and continue
            info = new Info();
            sub.insertInMap(pc[pos], info );
        }

        return  info->get(pc, pos+1 );
    }
    
    std::string Info::str(  )
    {
        return str( "TOP"  , 0 );
    }
    
    std::string Info::str( std::string top ,  int level )
    {
        std::ostringstream output;

        // Value for this property
        for (int i = 0 ; i < level ; i++ )
            output << "  ";
        output << top << ": " << value << "\n";
        
        // Children elements
        au::map<std::string , Info >::iterator s;
        for (s = sub.begin() ; s != sub.end() ; s++)
            output << s->second->str( s->first , level + 1 );
        
        return output.str();
    }

    ValuesCollection Info::getValues( std::string path )
    {
        // Collection of values that will be returned
        ValuesCollection vc;
        
        // Split path to get the path components
        PathComponents pc;
        getPathComponents( path , pc );
        
        getValues( pc , 0 , vc );
        
        return vc;
    }

    void Info::getValues( PathComponents & pc , int pos , ValuesCollection& vc )
    {
        //LM_M(("GetValues ---> %s -->  " , components[pos].c_str() ));
        
        if( pos == (int) (pc.size() -1 ) )
            if( pc[pos] == "*" )
            {
                // Include all children's names as values
                au::map<std::string , Info >::iterator s;
                for ( s = sub.begin() ; s != sub.end() ; s++ )
                    vc.add( s->first );
                
                return;
            }
        
        
        if( pos == (int) ( pc.size() ) )
        {
            vc.add( value );
        }
        else
        {
            if( pc[pos] == "*" )
            {
                // Include all childrens
                au::map<std::string , Info >::iterator s;
                for ( s = sub.begin() ; s != sub.end() ; s++ )
                    s->second->getValues( pc , pos + 1 , vc );
            }
            else
            {
                Info *sub_info = sub.findInMap( pc[pos] );
                if( sub_info )
                    sub_info->getValues(pc, pos+1  , vc );
            }
            
        }
        
    }
    

#pragma mark Interaction with the network::...
    
    
    void Info::fill( std::string name , network::Info *info )
    {
        info->set_name( name );
        info->set_value( value );
        
        au::map<std::string , Info >::iterator s;
        for (s = sub.begin() ; s != sub.end() ; s++)
            s->second->fill( s->first ,  info->add_info() );
    }
    
    
    void Info::get( const network::Info& info)
    {
        // Get the value
        value = info.value();

        // Get all the childrens
        for( int i = 0 ; i < info.info_size() ; i++ )
        {
            Info* tmp = new Info();
            tmp->get( info.info(i) );
            
            std::string name  = info.info(i).name();
            sub.insertInMap( name , tmp );
        }
    }

    
    // Static funcitons to copy environment
    

    
    void Info::getPathComponents( std::string path , PathComponents & components )
    {
        std::stringstream ss(path);
        std::string item;
        while(std::getline(ss, item, '.')) {
            components.push_back(item);
        }
        
    }

}
