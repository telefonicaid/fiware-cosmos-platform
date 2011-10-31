

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
    
    TreeItem* TreeItem::getFilteredItem( std::string name , std::string filter_name , std::string filter_value )
    {
        LM_M(("Get filtered value %s element name=%s filter=%s value=%s" , value.c_str(),  name.c_str() , filter_name.c_str() , filter_value.c_str() ));
        
        for ( size_t i = 0 ; i < items.size() ; i++ )
            if( items[i]->isValue( name ) )
            {
                TreeItem *_item = items[i]->getItemFromPath(filter_name);
                                
                if( _item && _item->getChildrenValue() == filter_value )
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
    
    size_t TreeItem::getNumItems( std::string name )
    {
        size_t total = 0;

        for ( size_t  i = 0 ; i < items.size() ; i++ )
            if( items[i]->value == name )
                total ++;
        
        return total;
    }

    
    TreeItem* TreeItem::getItem( size_t pos )
    {
        if( pos >= items.size() )
            return NULL;
        
        return items[pos];
    }
    
    void TreeItem::str( std::ostringstream& output , int level , int max_level )
    {
        if( max_level > 0 )
            if ( level >= max_level )
                return;
        
        for ( int i = 0 ; i < level ; i++ )
            output << " ";
        output << "* " << value << "\n";
        
        for ( size_t i = 0 ; i < items.size() ; i++)
            items[i]->str( output , level+1 , max_level ); 
    }
    
    std::string TreeItem::str( int max_level )
    {
        std::ostringstream output;
        str( output , 0 , max_level);
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

            //LM_M(("getItemFromPath %s --> %s %s " , path.c_str() , field_description.c_str() , next_path.c_str() ));
            
            TreeItem *item = getItemFromFieldDescription(field_description);
            if( item )
                return item->getItemFromPath( next_path );
            else
                return NULL;
        }
        
    }

    TreeItem* TreeItem::getItemFromFieldDescription( std::string field_description )
    {
        //LM_M(("getItemFromFieldDescription %s " , field_description.c_str() ));
        
        size_t pos_1 = field_description.find('[', 0 );
        size_t pos_2 = field_description.find(']', 0 );
        
        if( pos_1 == std::string::npos )
        {
            TreeItem *item = getItem(field_description, 0);
            //LM_M(("Returning %s" , item?item->getValue().c_str():"NULL")); 
            return item;
        }
        
        if( pos_2 == std::string::npos )
            return NULL;
        
        if( pos_1 >= pos_2 )
            return  NULL;
        
        std::string field_name = field_description.substr( 0 , pos_1 );
        

        std::string field_filter = field_description.substr( pos_1+1 , pos_2 - pos_1 - 1 );
        
        size_t pos_equal = field_filter.find('=',0);
        if( pos_equal == std::string::npos )
        {
            size_t pos = atoll( field_filter.c_str() );
            return getItem( field_name , pos );
        }
        else
        {
            std::string filter_name  = field_filter.substr( 0 , pos_equal );
            std::string filter_value = field_filter.substr( pos_equal+1 , field_filter.length() - ( pos_equal+1 ) );
            
            return getFilteredItem ( field_name , filter_name , filter_value );
            
        }
    }
    
    size_t TreeItem::getUInt64( std::string path )
    {
        return atoll( get(path).c_str() );
    }
    
    int TreeItem::getInt( std::string path )
    {
        return atoi( get(path).c_str() );
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