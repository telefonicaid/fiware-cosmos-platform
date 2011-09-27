

#include "au/xml.h"                             // au::xml....

#include "samson/stream/BlockList.h"            // samson::stream::BlockList

#include "BlockBreak.h"         // Own interface

/*
namespace samson {
    namespace stream
    {
        
        
        BlockBreakItem::BlockBreakItem( size_t _id , int _num_divisions )
        {
            id = _id;
            num_divisions  = _num_divisions ;
            list = new BlockList( au::str("BlockBreak_id_%lu_with_%d_divisions" , id , num_divisions) );
            ready = false;
        }
        
        BlockBreakItem::~BlockBreakItem()
        {
            delete list;
        }

        void BlockBreakItem::update( BlockList *_list )
        {
            if( ready )
                return;
            
            ready = true;
            
            list->clearBlockList();
            list->copyFrom( _list );
        }
        
        // Get xml information
        void BlockBreakItem::getInfo( std::ostringstream &output )
        {
            au::xml_open( output , "block_break_item" );
            
            au::xml_simple(output, "num_divisions", num_divisions);
            
            list->getInfo(output);
            
            au::xml_close( output , "block_break_item" );
        }
        
        // Check if ready
        bool BlockBreakItem::isReady()
        {
            return ready;
        }
        
        // Get list
        BlockList* BlockBreakItem::getBlockList()
        {
            return list;
        }
        
#pragma mark
        
        BlockBreak::BlockBreak( size_t _id  )
        {
            id = _id;
        }
        
        BlockBreak::~BlockBreak()
        {
            items.clearMap();
        }
        
        // Get xml information
        void BlockBreak::getInfo( std::ostringstream &output )
        {
            au::xml_open( output , "block_break" );
            
            au::xml_simple(output, "id", id);

            au::xml_iterate_map( output , "block_break_items" , items );
            
            au::xml_close( output , "block_break" );
        }
        
        int BlockBreak::getMaxDivision()
        {
            int max_division = 0;
            au::map< int , BlockBreakItem >::iterator i;
            for ( i = items.begin() ; i != items.end() ; i++ )
            {
                if( max_division < i->first )
                    max_division = i->first;
            }
            
            return max_division;
        }

        int BlockBreak::getMaxDivisionReady()
        {
            int max_division = 0;
            au::map< int , BlockBreakItem >::iterator i;
            for ( i = items.begin() ; i != items.end() ; i++ )
            {
                if( i->second->isReady() )
                    if( max_division < i->first )
                        max_division = i->first;
            }
            
            return max_division;
        }

        BlockList *BlockBreak::getMaxDivisionBlockList()
        {
            int num_divisions = getMaxDivisionReady();
            if( num_divisions == 0)
                return NULL;
            
            return getItem( num_divisions )->getBlockList();
        }
        
        
        void BlockBreak::addNumDivisions( int num_divisions )
        {
            getItem( num_divisions );
        }
        
        void BlockBreak::update( int num_divisions , BlockList *_list )
        {
            getItem( num_divisions )->update(_list);
            
        }

        void BlockBreak::removeSmallerDivisions()
        {
            if( items.size() < 2 )
                return;
            
            int max_division = getMaxDivisionReady();

            if( max_division == 0)
                return;
            
            while( items.size() > 1 )
            {
                int _num_divisions = items.begin()->first;
                if( _num_divisions < max_division )
                {
                    items.removeInMap( _num_divisions );
                }
            }
            
        }
        
        
        BlockBreakItem* BlockBreak::getItem( int num_divisions )
        {
            BlockBreakItem* item = items.findInMap( num_divisions );
            
            if( ! item )
            {
                item = new BlockBreakItem( id , num_divisions );
                items.insertInMap( num_divisions , item );
            }
            
            return item;
            
        }
        
        
                
        
    }
}

*/