

/* ****************************************************************************
 *
 * FILE            Pugi.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         pugi library
 *
 * DATE            7/15/11
 *
 * DESCRIPTION
 *
 *  Usefull functions to simplify development with pugi library
 *  Everything ready for running xpath queries over xml data
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_xml_Pugi
#define _H_xml_Pugi


#include <string>           // std::string
#include <sstream>          // std::ostringstream
#include <vector>
#include <set>

#include "au/string.h"          
#include "au/tables/pugixml.hpp"     // pugi::...

namespace au
{
    namespace tables
    {
        class TreeItem;
    }
}

 
namespace pugi 
{
    // Working with TreeItems
    au::tables::TreeItem* treeItemFromDocument(  const xml_document& xml_doc );
    au::tables::TreeItem* treeItemFromNode(  const xml_node& xml_node );
    
}
#endif
