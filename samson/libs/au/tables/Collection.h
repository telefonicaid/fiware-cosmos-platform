
#ifndef _H_AU_TABLE_COLLECTION
#define _H_AU_TABLE_COLLECTION

#include "au/containers/simple_map.h"


namespace au {
namespace tables {

class Table;

class CollectionItem
{
    friend class Collection;
    au::simple_map<std::string,std::string> values;

public:
    
    CollectionItem();
    CollectionItem( CollectionItem* collection_item );
    
    void add( std::string name , std::string value );
    void add( CollectionItem* collection_item );
    std::string get( std::string name );
    bool match( CollectionItem* filter );
    bool match( std::string name , std::string value );

    void add_prefix( std::string name );
    
};

class Collection
{
    std::vector<CollectionItem*> items;
    
public:
    
    Collection(){}
    Collection( Collection* collection );
    ~Collection();
    
    void add( CollectionItem* item );
    void add( Collection *collection );
    size_t getNumItems();
    
    
    // Get a table from this collection
    Table* getTable();
    
    // Get a filtered version of this collection
    Collection *getCollection( CollectionItem *filter );

    // Transform to string
    std::string str( std::string title );
    
    // Remove if match
    void remove( std::string name , std::string value);
    
};

}
}

#endif