

#include "au/Tree.h"    // Own interface

namespace au
{
    TreeItem::TreeItem( std::string _value )
    {
        value = _value;
    }
    
    std::string TreeItem::getValue()
    {
        return value;
    }
    
    std::string TreeItem::getChildrenValue()
    {
        if( items.size() == 0)
            return "";
        return items[0]->getValue();
    }
    
    std::string TreeItem::getFirstItemValue()
    {
        if( items.size() == 0)
            return "";
        else
            return items[0]->getValue();
    }
    
    bool TreeItem::isValue( std::string _value )
    {
        return ( value == _value );
    }

    TreeItem* TreeItem::getItem( std::string name , int position )
    {
        for ( size_t i = 0 ; i < items.size() ; i++ )
            if( items[i]->isValue( name ) )
            {
                position--;
                if( position < 0 )
                    return items[i];
            }
        return NULL;
    }
    
    std::string TreeItem::getProperty( std::string name )
    {
        TreeItem * item = getItem( name , 0 );  // Get first children with this name
        if (!item )
            return "";
        else
            return item->getFirstItemValue();            
    }
    
    void TreeItem::add( TreeItem* item)
    {
        items.push_back( item );
    }
    
    size_t TreeItem::getNumItems()
    {
        return items.size();
    }
    
    TreeItem* TreeItem::getItem( size_t pos )
    {
        if( pos >= items.size() )
            return NULL;
        
        return items[pos];
    }
    
    void TreeItem::str( std::ostringstream& output , int level )
    {
        for ( int i = 0 ; i < level ; i++ )
            output << " ";
        output << "* " << value << "\n";
        
        for ( size_t i = 0 ; i < items.size() ; i++)
            items[i]->str( output , level+1 ); 
    }
    
    std::string TreeItem::str()
    {
        std::ostringstream output;
        str( output , 0);
        return output.str();
    }
    
    TreeItem* TreeItem::getItemFromPath( std::string path )
    {
        size_t pos = path.find('/', 0 );

        std::string item_name;
        
        if( pos == std::string::npos )
        {
            // Final field
            return getItemFromFieldDescription( path );
        }
        else
        {
            // Partical field
            std::string field_description = path.substr( 0 , pos );
            std::string next_path = path.substr( pos+1 , path.length() - (pos+1) );

            LM_M(("getItemFromPath %s --> %s %s " , path.c_str() , field_description.c_str() , next_path.c_str() ));
            
            TreeItem *item = getItemFromFieldDescription(field_description);
            if( item )
                return item->getItemFromPath( next_path );
            else
                return NULL;
        }
        
    }

    TreeItem* TreeItem::getItemFromFieldDescription( std::string field_description )
    {
        LM_M(("getItemFromFieldDescription %s " , field_description.c_str() ));
        
        size_t pos_1 = field_description.find('[', 0 );
        size_t pos_2 = field_description.find(']', 0 );
        
        if( pos_1 == std::string::npos )
        {
            TreeItem *item = getItem(field_description, 0);
            LM_M(("Returning %s" , item?item->getValue().c_str():"NULL")); 
            return item;
        }
        
        if( pos_2 == std::string::npos )
            return NULL;
        
        if( pos_1 >= pos_2 )
            return  NULL;
        
        std::string field_name = field_description.substr( 0 , pos_1 );
        size_t pos = atoll( field_description.substr( pos_1+1 , pos_2 - pos_1 - 1 ).c_str() );
        
        return getItem( field_name , pos );
    }
    
    size_t TreeItem::getUInt64( std::string path )
    {
        return atoll( get(path).c_str() );
    }
    
    std::string TreeItem::get( std::string path )
    {
        TreeItem* item = getItemFromPath( path );
        if( item )
            return item->getChildrenValue();
        else
            return "";
    }

}