
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

    

#pragma mark Interaction with the network::...
    
    
    void Info::fill(au::Info*_info, std::string name , network::Info *info )
    {
        info->set_name( name );

        // Get a list of all my properties
        std::set<std::string> properties;
        _info->getChildrens(properties);
        
        std::set<std::string>::iterator p;
        for ( p = properties.begin() ; p != properties.end() ; p++ )
            fill( _info->get(*p) , *p ,  info->add_info() );
        
    }
    
    
    void Info::get( au::Info*_info, const network::Info& info)
    {

        // Get all the childrens
        for( int i = 0 ; i < info.info_size() ; i++ )
        {
            au::Info* tmp = new au::Info();
            get(tmp, info.info(i) );
            
            std::string name  = info.info(i).name();
            _info->set( name , tmp );
        }
    }


}
