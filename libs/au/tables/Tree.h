/* ****************************************************************************
 *
 * FILE            Tree.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Usefull classes to get tree data-sets
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_TREE
#define _H_AU_TREE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/containers/simple_map.h"
#include "au/CommandLine.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/containers/map.h"                 // au::map
#include "au/string.h"              // au::str(...)

#include "au/namespace.h"


NAMESPACE_BEGIN(au)
NAMESPACE_BEGIN(tables)

class Table;
class TreeItem;
class KVTreeItem;
class CollectionItem;
class Collection;
class SelectTreeInformation;

// Collection of trees ( only used internally at Tree )

class TreeCollection
{
    std::vector<TreeItem*> trees;
    
    friend class TreeItem;
    
    public:
    
    ~TreeCollection();
    
    void add( TreeCollection* tree_collection);
    void add( TreeItem* tree );
    
    size_t getNumTrees();
    TreeItem* getTree( size_t pos );

    std::string str( size_t limit );

    void replaceToAll( std::string name , std::string value );
    void addSpetialTags( TreeItem* tree_item );
    
};

class TreeItem
{
    std::string value;                   // Content of this node
    std::vector<TreeItem*> items;        // Sub nodes
    
public:
    
    TreeItem( std::string _value );
    TreeItem( TreeItem * tree );
    
    ~TreeItem();
    
    // Get content
    std::string getValue();
    std::string getFirstNodeValue();
    std::string getFirstNodeValue( std::string node_name );

    // Add nodes to this tree
    TreeItem* add( TreeItem* item );
    TreeItem* add( std::string value );
    TreeItem* add( std::string property , std::string value );

    // Replace of add
    void replace( std::string property , std::string value );
    
    // Change the value of this tree node
    void setValue( std::string _value )
    {
        value = _value;
    }
    
    // Select for a particular tree
    TreeCollection* getTreesFromPath(std::string path);
    TreeCollection* selectTrees( SelectTreeInformation* select );    
    TreeItem* getFirstTreeFromPath(std::string path);
    
    // Transform into strings
    std::string str( int max_level = 0 );
    void str( std::ostringstream& output , int level , int max_level );
    
    // Get information
    size_t getNumItems();
    size_t getNumItems( std::string name );
    size_t getTotalNumNodes();
    size_t getMaxDepth();

    // Other
    bool isValue( std::string _value );
    bool hasNode( std::string node_name );
    bool isFinalProperty();
    void getNodesFrom( TreeItem *tree );

    // Get a collection of three items from 
    Table* getTableFromPath( std::string path );
    Collection *getCollectionFromPath( std::string path );
    
    // Transform an item into a table
    Table* getTable();
    
    // Get a collection item
    CollectionItem* getCollectionItem();
    
    // Parse xml content to get a tree
    static TreeItem *getTreeFromXML( std::string &data );

    // Replace the first appearance of a node ( if any ) by the new. Otherwise, just inserted
    void replaceNode( TreeItem* item );
    
};

NAMESPACE_END
NAMESPACE_END

#endif
